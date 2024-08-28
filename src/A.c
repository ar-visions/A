#include <A>
#include <ffi.h>
#include <sys/stat.h>
#include <dirent.h>

static global_init_fn* call_after;
static num             call_after_alloc;
static num             call_after_count;
static A_f**           types;
static num             types_alloc;
static num             types_len;

void A_lazy_init(global_init_fn fn) {
    if (call_after_count == call_after_alloc) {
        global_init_fn* prev      = call_after;
        num            alloc_prev = call_after_alloc;
        call_after_alloc          = 32 + (call_after_alloc << 1);
        call_after                = calloc(call_after_alloc, sizeof(global_init_fn));
        if (prev) {
            memcpy(call_after, prev, sizeof(global_init_fn) * alloc_prev);
            free(prev);
        }
    }
    call_after[call_after_count++] = fn;
}

void A_push_type(A_f* type) {
    if (types_alloc == types_len) {
        A_f** prev = types;
        num   alloc_prev = types_alloc;
        types_alloc = 128 + (types_alloc << 1);
        types = calloc(types_alloc, sizeof(A_f*));
        if (alloc_prev) {
            memcpy(types, prev, sizeof(A_f*) * alloc_prev);
            free(prev);
        }
    }
    types[types_len++] = type;
}

A_f** A_types(num* length) {
    *length = types_len;
    return types;
}

A A_construct(AType type, int n_args, ...) {
    A res = A_alloc(type, 1);
    va_list  vargs;
    va_start(vargs, n_args);
    array args = construct(array, with_sz, n_args);
    for (int i = 0; i < n_args; i++) {
        A arg = va_arg(vargs, A);
        call(args, push, arg);
    }
    va_end(vargs);

    AType arg0_type = isa(args->elements[0]);
    type_member_t* mem = A_constructor(type, arg0_type);
    assert(mem && mem->method);
    return method_call(mem->method, args);
}

A A_new(AType type) {
    A res = A_alloc(type, 1);
    // todo: do not call init in A_alloc
    return res;
}

A A_alloc(AType type, num count) {
    A a = calloc(1, (type == typeof(A) ? 0 : sizeof(struct A)) + type->size * count);
    a->type   = type;
    a->origin = a;
    a->data   = &a[1];
    a->count  = count;
    a->alloc  = count;
    AType a_type = &A_type;
    AType current = type;
    while (current) {
        if (current->init) /// init not being set on here somehow, even though it should be emitting string_type.init = &A_init
            current->init(a->data);
        if (current == a_type)
            break;
        current = current->parent_type;
    }
    return a->data; /// fields(a) == this operation
}

A A_realloc(A a, num alloc) {
    A obj = fields(a);
    assert(obj->type->traits == A_TRAIT_PRIMITIVE);
    A   re    = calloc(1, sizeof(struct A) + obj->type->size * alloc);
    num count = obj->count < alloc ? obj->count : alloc;
    memcpy(&re[1], obj->data, obj->type->size * count);
    if (obj->data != &obj[1])
        free(&obj->data[-1]);
    re->origin = obj; /// we do not want to keep the counts/alloc in sync between these
    obj->data  = &re[1];
    obj->count = count;
    obj->alloc = alloc;
    return obj->data;
}

void A_push(A a, A value) {
    A   obj = fields(a);
    assert(obj->type->traits == A_TRAIT_PRIMITIVE);
    num sz  = obj->type->size;
    if (obj->count == obj->alloc)
        A_realloc(a, 32 + (obj->alloc << 1));
    memcpy(&((u8*)obj->data)[obj->count++ * sz], value, sz);
}

method_t* method_with_address(handle address, AType rtype, array atypes, AType method_owner) {
    const num max_args = 8;
    method_t* method = calloc(1, sizeof(method_t));
    method->ffi_cif  = calloc(1,        sizeof(ffi_cif));
    method->ffi_args = calloc(max_args, sizeof(ffi_type*));
    method->atypes   = new(array);
    method->rtype    = rtype;
    ffi_type **ffi_args = (ffi_type**)method->ffi_args;
    for (num i = 0; i < atypes->len; i++) {
        A_f* a_type   = atypes->elements[i];
        bool is_prim  = a_type->traits & A_TRAIT_PRIMITIVE;
        ffi_args[i]   = is_prim ? a_type->arb : &ffi_type_pointer;
        M(array, push, method->atypes, a_type);
    }
    ffi_status status = ffi_prep_cif(
        (ffi_cif*) method->ffi_cif, FFI_DEFAULT_ABI, atypes->len,
        (ffi_type*)(rtype->traits & A_TRAIT_ABSTRACT) ? method_owner->arb : rtype->arb, ffi_args);
    assert(status == FFI_OK);
    return method;
}

A method_call(method_t* a, array args) {
    const num max_args = 8;
    void* arg_values[max_args];
    assert(args->len == a->atypes->len);
    for (num i = 0; i < args->len; i++) {
        A_f* arg_type = a->atypes->elements[i];
        arg_values[i] = (arg_type->traits & (A_TRAIT_PRIMITIVE | A_TRAIT_ENUM)) ? 
            (void*)args->elements[i] : (void*)&args->elements[i];
    }
    void* result[8]; /// enough space to handle all primitive data
    ffi_call((ffi_cif*)a->ffi_cif, a->address, result, arg_values);
    if (a->rtype->traits & A_TRAIT_PRIMITIVE)
        return A_primitive(a->rtype, result);
    else if (a->rtype->traits & A_TRAIT_ENUM)
        return A_enum(a->rtype, *(i32*)result);
    else
        return (A) result[0];
}

/// this calls type methods
A A_method(A_f* type, cstr method_name, array args) {
    type_member_t* mem = A_member(type, A_TYPE_IMETHOD | A_TYPE_SMETHOD, method_name);
    assert(mem->method);
    return method_call(mem->method, args);
}

void A_finish_types() {
    int remaining = call_after_count;
    while (remaining)
        for (int i = 0; i < call_after_count; i++) {
            global_init_fn fn = call_after[i];
            if (fn && fn()) {
                call_after[i] = null;
                remaining--;
            }
        }

    num         types_len;
    A_f**       types = A_types(&types_len);
    const num   max_args = 8;

    /// iterate through types
    for (num i = 0; i < types_len; i++) {
        A_f* type = types[i];
        if (type->traits & A_TRAIT_ABSTRACT) continue;
        /// for each member of type
        for (num m = 0; m < type->member_count; m++) {
            type_member_t* mem = &type->members[m];
            if (mem->member_type & (A_TYPE_IMETHOD | A_TYPE_SMETHOD | A_TYPE_CONSTRUCT)) {
                void* address = 0;
                memcpy(&address, &((u8*)type)[mem->offset], sizeof(void*));
                assert(address);
                array args = construct(array, sz, mem->args.count);
                for (num i = 0; i < mem->args.count; i++)
                    args->elements[i] = ((A_f**)&mem->args.meta_0)[i];
                args->len = mem->args.count;
                mem->method = method_with_address(address, mem->type, args, type);
            }
        }
    }
}

type_member_t* A_constructor(AType type, AType first_arg) {
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        if (mem->member_type == A_TYPE_CONSTRUCT && mem->args.meta_0 == first_arg)
            return mem;
    }
    return 0;
}

type_member_t* A_member(A_f* type, enum A_TYPE member_type, char* name) {
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        if (mem->member_type & member_type && strcmp(mem->name, name) == 0)
            return mem;
    }
    return 0;
}

/// can we have i32_with_i16(i32 a, i16 b)
/// primitives are based on A alone
/// i wonder if we can add more constructors or even methods to the prims

A A_primitive(A_f* type, void* data) {
    assert(type->traits & A_TRAIT_PRIMITIVE);
    A copy = A_alloc(type, type->size);
    memcpy(copy, data, type->size);
    return copy;
}

A A_enum(A_f* type, i32 data) {
    assert(type->traits & A_TRAIT_ENUM);
    assert(type->size == sizeof(i32));
    A copy = A_alloc(type, type->size);
    memcpy(copy, &data, type->size);
    return copy;
}

A A_i8(i8 data)     { return A_primitive(&i8_type,  &data); }
A A_u8(u8 data)     { return A_primitive(&u8_type,  &data); }
A A_i16(i16 data)   { return A_primitive(&i16_type, &data); }
A A_u16(u16 data)   { return A_primitive(&u16_type, &data); }
A A_i32(i32 data)   { return A_primitive(&i32_type, &data); }
A A_u32(u32 data)   { return A_primitive(&u32_type, &data); }
A A_i64(i64 data)   { return A_primitive(&i64_type, &data); }
A A_u64(u64 data)   { return A_primitive(&u64_type, &data); }
A A_f32(f32 data)   { return A_primitive(&f32_type, &data); }
A A_f64(f64 data)   { return A_primitive(&f64_type, &data); }
A A_cstr(cstr data) { return A_primitive(&cstr_type, &data); }
A A_none()          { return A_primitive(&none_type, &data); }
A A_bool(bool data) { return A_primitive(&bool_type, &data); }

/// A -------------------------
A           A_hold(A a) { ++a->refs; return a; }
static A   A_new_default(AType type, num count) {
    return A_alloc(type, count);
}
static void A_init      (A a) { }
static void A_destructor(A a) {
    // go through objects type fields/offsets; when type is A-based, we release
    AType type = isa(a);
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* m = &type->members[i];
        if ((m->member_type == A_TYPE_PROP || m->member_type == A_TYPE_PRIV) && !(m->type->traits & A_TRAIT_PRIMITIVE)) {
            u8* ptr = (u8*)a + m->offset;
            A   ref = ptr;
            drop(ref);
            *((A*)&ptr) = null;
        }
    }
}
static u64  A_hash      (A a) { return (u64)(size_t)a; }
static bool A_cast_bool (A a) { return (bool)(size_t)a; }

static A A_with_cstr(A a, cstr cs, num len) {
    A        f = fields(a);
    AType type = f->type;
    if      (type == typeof(f64)) sscanf(cs, "%lf",  (f64*)a);
    else if (type == typeof(f32)) sscanf(cs, "%f",   (f32*)a);
    else if (type == typeof(i32)) sscanf(cs, "%i",   (i32*)a);
    else if (type == typeof(u32)) sscanf(cs, "%u",   (u32*)a);
    else if (type == typeof(i64)) sscanf(cs, "%lli", (i64*)a);
    else if (type == typeof(u64)) sscanf(cs, "%llu", (u64*)a);
    else {
        printf("implement construct cstr for %s\n", f->type->name);
        exit(-1);
    }

    return a;
}

#define set_v() \
    AType type = isa(a); \
    if (type == typeof(i8))  *(i8*) a = (i8) v; \
    if (type == typeof(i16)) *(i16*)a = (i16)v; \
    if (type == typeof(i32)) *(i32*)a = (i32)v; \
    if (type == typeof(i64)) *(i64*)a = (i64)v; \
    if (type == typeof(u8))  *(u8*) a = (u8) v; \
    if (type == typeof(u16)) *(u16*)a = (u16)v; \
    if (type == typeof(u32)) *(u32*)a = (u32)v; \
    if (type == typeof(u64)) *(u64*)a = (u64)v; \
    if (type == typeof(f32)) *(f32*)a = (f32)v; \
    if (type == typeof(f64)) *(f64*)a = (f64)v; \
    return a;

static A numeric_with_i8 (A a, i8   v) { set_v(); }
static A numeric_with_i16(A a, i16  v) { set_v(); }
static A numeric_with_i32(A a, i32  v) { set_v(); }
static A numeric_with_i64(A a, i64  v) {
    AType type = isa(a);
    if (type == typeof(i8))  *(i8*) a = (i8) v;
    if (type == typeof(i16)) *(i16*)a = (i16)v;
    if (type == typeof(i32)) *(i32*)a = (i32)v; \
    if (type == typeof(i64)) *(i64*)a = (i64)v; \
    if (type == typeof(u8))  *(u8*) a = (u8) v; \
    if (type == typeof(u16)) *(u16*)a = (u16)v; \
    if (type == typeof(u32)) *(u32*)a = (u32)v; \
    if (type == typeof(u64)) *(u64*)a = (u64)v; \
    if (type == typeof(f32)) *(f32*)a = (f32)v; \
    if (type == typeof(f64)) *(f64*)a = (f64)v; \
    return a;
}
static A numeric_with_u8 (A a, u8   v) { set_v(); }
static A numeric_with_u16(A a, u16  v) { set_v(); }
static A numeric_with_u32(A a, u32  v) { set_v(); }
static A numeric_with_u64(A a, u64  v) { set_v(); }
static A numeric_with_f32(A a, f32  v) { set_v(); }
static A numeric_with_f64(A a, f64  v) { set_v(); }
static A numeric_with_bool(A a, bool v) { set_v(); }
static A numeric_with_num(A a, num  v) { set_v(); }

A A_method(A_f* type, char* method_name, array args);

/// these pointers are invalid for A since they are in who-knows land, but the differences would be the same
static i32 A_compare(A a, A b) {
    return (i32)(a - b);
}

string format(cstr template, ...) {
    va_list args;
    va_start(args, template);
    char buf[1024];
    sz len = vsnprintf(buf, sizeof(buf), template, args);
    va_end(args);
    return construct(string, cstr, buf, len);
}

static void  string_destructor(string a) { free(a->chars); }
static num   string_compare(string a, string b) { return strcmp(a->chars, b->chars); }

static array string_split(string a, cstr sp) {
    cstr next = a->chars;
    sz   slen = strlen(sp);
    while (next) {
        cstr   n = strstr(&next[1], sp);
        string v = construct(string, cstr, next, n ? (sz)(next - n) : -1);
        next = n ? n + slen : null;
    }
    return null;
}

static void string_alloc_sz(string a, sz alloc) {
    char* chars = calloc(alloc, sizeof(char));
    memcpy(chars, a->chars, sizeof(char) * a->len);
    chars[a->len] = 0;
    free(a->chars);
    a->chars = chars;
    a->alloc = alloc;
}

static string string_mid(string a, num start, num len) {
    return construct(string, cstr, &a->chars[start], len);
}

static none  string_append(string a, cstr b) {
    sz blen = strlen(b);
    string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy(&a->chars[a->len], b, blen);
    a->len += blen;
    a->chars[a->len] = 0;
}

static num   string_index_of(string a, cstr cs) {
    char* f = strstr(a->chars, cs);
    return f ? (num)(f - a->chars) : (num)-1;
}

static bool string_cast_bool(string a) {
    return a->len > 0;
}

u64 fnv1a_hash(const void* data, size_t length, u64 hash) {
    const u8* bytes = (const u8*)data;
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];  // xor bottom with current byte
        hash *= FNV_PRIME; // multiply by FNV prime
    }
    return hash;
}

static u64 item_hash(item f) {
    return M(A, hash, f->key ? f->key : f->val);
}

static u64 string_hash(string a) {
    if (a->h) return a->h;
    a->h = fnv1a_hash(a->chars, a->len, OFFSET_BASIS);
    return a->h;
}

static string string_with_sz(string a, sz size) {
    a->alloc = size;
    a->chars = (char*)calloc(1, a->alloc);
    return a;
}

static string string_with_cstr(string a, cstr value, num len) {
    if (len == -1) len = strlen(value);
    a->alloc = len + 1;
    a->len   = len;
    a->chars = (char*)calloc(1, a->alloc);
    memcpy(a->chars, value, len);
    a->chars[len] = 0;
    return a;
}

static item hashmap_fetch(hashmap a, A key) {
    u64 h = fnv1a_hash(key, strlen(key), OFFSET_BASIS);
    u64 k = h % a->alloc;
    list bucket = a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (call(f->key, compare, key) == 0)
            return f;
    item n = call(bucket, push, null);
    n->key = hold(key);
    a->count++;
    return n;
}

static item hashmap_lookup(hashmap a, A key) {
    u64 h = fnv1a_hash(key, strlen(key), OFFSET_BASIS);
    u64 k = h % a->alloc;
    list bucket = a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (call(f->key, compare, key) == 0)
            return f;
    return null;
}

static none hashmap_set(hashmap a, A key, A value) {
    item i = call(a, fetch, key);
    A prev = i->val;
    i->val = hold(value);
    drop(prev);
}

static A hashmap_get(hashmap a, A key) {
    item i = call(a, lookup, key);
    return i ? i->val : null;
}

static bool hashmap_contains(hashmap a, A key) {
    item i = call(a, lookup, key);
    return i != null;
}

static none hashmap_remove(hashmap a, A key) {
    u64 h = fnv1a_hash(key, strlen(key), OFFSET_BASIS);
    u64 k = h % a->alloc;
    list bucket = a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (call(f->key, compare, key) == 0) {
            call(bucket, remove_item, f);
            a->count--;
            break;
        }
}

static bool hashmap_cast_bool(hashmap a) {
    return a->count > 0;
}

static A hashmap_index_A(hashmap a, A key) {
    return call(a, get, key);
}

static hashmap hashmap_with_sz(hashmap a, sz size) {
    a->alloc = size;
    a->data  = (list*)calloc(size, sizeof(list));
    return a;
}

define_class(hashmap)


/// collection -------------------------
static A collection_push(collection a, A b) {
    assert(false);
    return null;
}

static A  collection_pop(collection a) {
    assert(false);
    return null;
}

static num collection_compare(array a, collection b) {
    assert(false);
    return 0;
}

A hold(A a) {
    if (a) (a - 1)->refs++;
    return a;
}

void drop(A a) {
    if (a && --(a - 1)->refs == -1) {
        A aa = (a - 1);
        A_f*  type = aa->type;
        void* prev = null;
        while (type) {
            if (prev != type->destructor) {
                type->destructor(a);
                prev = type->destructor;
            }
            if (type == &A_type)
                break;
            type = type->parent_type;
        }
        free(aa);
    }
}

A fields(A instance) {
    return (instance - 1)->origin;
}

A data(A instance) {
    A obj = fields(instance);
    return obj->data;
}

bool A_inherits(A inst, AType type) {
    AType t  = type;
    AType it = isa(inst); 
    while (t) {
        if (it == t)
            return true;
        t = t->parent_type; 
    }
    return false;
}

/// list -------------------------
static item list_push(list a, A e) {
    item n = new(item);
    n->val = e; /// held already by caller
    if (a->last) {
        a->last->next = n;
        n->prev       = a->last;
    } else {
        a->first      = n;
    }
    a->last = n;
    a->count++;
    return n;
}

static A list_remove(list a, num index) {
    num i = 0;
    item res = null;
    for (item ai = a->first; ai; ai = ai->next) {
        if (i++ == index) {
            res = ai;
            if (ai == a->first) a->first = ai->next;
            if (ai == a->last)  a->last  = ai->prev;
            if (ai->prev)       ai->prev->next = ai->next;
            if (ai->next)       ai->next->prev = ai->prev;
            a->count--;
            res->prev = null;
            res->next = null;
        }
    }
    return res;
}

static A list_remove_item(list a, item ai) {
    item res = null;
    num i = 0;
    if (ai) {
        if (ai == a->first) a->first = ai->next;
        if (ai == a->last)  a->last  = ai->prev;
        if (ai->prev)       ai->prev->next = ai->next;
        if (ai->next)       ai->next->prev = ai->prev;
        a->count--;
        res->prev = null;
        res->next = null;
    }
    return res;
}

static num list_compare(list a, list b) {
    num diff  = a->count - b->count;
    if (diff != 0)
        return diff;
    for (item ai = a->first, bi = b->first; ai; ai = ai->next, bi = bi->next) {
        A_f* ai_t = *(A_f**)&((A)ai->val)[-1];
        num  cmp  = ai_t->compare(ai, bi);
        if (cmp != 0) return cmp;
    }
    return 0;
}

static A list_pop(list a) {
    item l = a->last;
    a->last = a->last->prev;
    if (!a->last)
        a->first = null;
    l->prev = null;
    a->count--;
    return l;
}

static A list_get(list a, num at_index) {
    num index = 0;
    for (item i = a->first; i; i = i->next) {
        if (at_index == index)
            return i->val;
        index++;
    }
    assert(false);
    return null;
}

static num list_count(list a) {
    return a->count;
}

static void array_alloc_sz(array a, sz alloc) {
    A* elements = (A*)calloc(alloc, sizeof(struct A*));
    memcpy(elements, a->elements, sizeof(struct A*) * a->len);
    free(a->elements);
    a->elements = elements;
    a->alloc = alloc;
}

/// array -------------------------
static void array_expand(array a) {
    num alloc = 32 + (a->alloc << 1);
    array_alloc_sz(a, alloc);
}

bool is_meta(A a) {
    AType t = isa(a);
    return t->meta.count > 0;
}

bool is_meta_compatible(A a, A b) {
    AType t = isa(a);
    if (is_meta(a)) {
        AType bt = isa(b);
        num found = 0;
        for (num i = 0; i < t->meta.count; i++) {
            AType mt = ((AType*)&t->meta.meta_0)[i];
            if (mt == bt)
                found++;
        }
        return found > 0;
    }
    return false;
}

static A array_push(array a, A b) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    AType t = isa(a);
    if (is_meta(a))
        assert(is_meta_compatible(a, b));
    a->elements[a->len++] = b;
    return b;
}

static A array_index_num(array a, num i) {
    return a->elements[i];
}

static A array_remove(array a, num b) {
    A before = a->elements[b];
    for (num i = b; i < a->len; i++) {
        A prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
        drop(prev);
    }
    a->elements[--a->len] = null;
    return before;
}

static void array_operator_assign_add(array a, A b) {
    array_push(a, b);
}

static void array_operator_assign_sub(array a, num b) {
    array_remove(a, b);
}

static A array_first(array a) {
    assert(a->len);
    return a->elements[a->len - 1];
}

static A array_last(array a) {
    assert(a->len);
    return a->elements[a->len - 1];
}

static void array_push_symbols(array a, ...) {
    va_list args;
    va_start(args, a);
    char* value;
    while ((value = va_arg(args, char*)) != null) {
        string s = construct(string, cstr, value, strlen(value));
        M(array, push, a, s);
    }
    va_end(args);
}

static void array_push_objects(array a, A f, ...) {
    va_list args;
    va_start(args, f);
    A value;
    while ((value = va_arg(args, A)) != null)
        M(array, push, a, value);
    va_end(args);
}

static array array_of_objects(AType validate, ...) {
    array a = new(array);
    va_list args;
    va_start(args, validate);
    assert(false);
    for (;;) {
        A arg = va_arg(args, A);
        if (!arg)
            break;
        assert(!validate || validate == isa(arg));
        M(array, push, a, arg);
    }
    return a;
}

static A array_pop(array a) {
    assert(a->len > 0);
    return a->elements[a->len--];
}

static num array_compare(array a, array b) {
    num diff = a->len - b->len;
    if (diff != 0)
        return diff;
    for (num i = 0; i < a->len; i++) {
        num cmp = M(A, compare, a->elements[i], b->elements[i]);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

static A array_get(array a, num i) {
    return a->elements[i];
}

static num array_count(array a) {
    return a->len;
}

/// index of element that compares to 0 diff with item
static num array_index_of(array a, A b) {
    for (num i = 0; i < a->len; i++) {
        if (call(a -> elements[i], compare, b) == 0)
            return i;
    }
    return -1;
}

static bool array_cast_bool(array a) { return a && a->len > 0; }

static array array_with_sz(array a, sz size) {
    array_alloc_sz(a, size);
    return a;
}

static u64 fn_hash(fn f) {
    return (u64)f->method->address;
}

static A fn_call(fn f, array args) {
    return method_call(f->method, args);
}

static fn fn_with_ATypes(fn f, ATypes atypes, AType rtype, handle address) {
    assert(atypes->len <= 8);
    f->method = method_with_address(address, rtype, atypes, null);
    return f;
}

define_class(fn)


static A vector_push(vector a, A b) {
    A obj = fields(a);
    if (obj->alloc == obj->count)
        A_realloc(obj, 32 + (obj->alloc < 1));
    u8* dst = obj->data;
    num sz = obj->type->size;
    A   cp = &dst[sz * obj->count++];
    memcpy(cp, b, sz);
    return cp;
}

/// the only thing we would look out for here is a generic 
/// user of 'collection' calling object on this return result
static A vector_pop(vector a) {
    A obj = fields(a);
    assert(obj->count > 0);
    u8* dst = obj->data;
    num sz = obj->type->size;
    return (A)&dst[sz * --obj->count];
}

static num vector_compare(vector a, vector b) {
    A a_object = fields(a);
    A b_object = fields(b);
    num diff = a_object->count - b_object->count;
    if (diff != 0)
        return diff;
    assert(a_object->type == b_object->type);
    u8* a_data = data(a);
    u8* b_data = data(b);
    num sz = a_object->type->size;
    for (num i = 0; i < a_object->count; i++) {
        num cmp = memcmp(&a_data[sz * i], &b_data[sz * i], sz);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

static A vector_get(vector a, num i) {
    A a_object = fields(a);
    u8* a_data = data(a);
    num sz = a_object->type->size;
    return (A)&a_data[i * sz];
}

static num vector_count(vector a) {
    A a_object = fields(a);
    return a_object->count;
}

static num vector_index_of(vector a, A b) {
    A a_object = fields(a);
    u8* a_data = data(a);
    u8* b_data = data(b);
    num sz = a_object->type->size;
    for (num i = 0; i < a_object->count; i++) {
        if (memcmp(&a_data[sz * i], b_data, sz) == 0)
            return i;
    }
    return -1;
}

static bool vector_cast_bool(vector a) {
    A a_object = fields(a);
    return a_object->count > 0;
}

u64 vector_hash(vector a) {
    A obj = fields(a);
    return fnv1a_hash(obj->data, obj->type->size * obj->count, OFFSET_BASIS);
}

static path path_with_cstr(path a, cstr path, num sz) {
    num len = sz == -1 ? strlen(path) : sz;
    a->chars = calloc(len + 1, 1);
    memcpy(a->chars, path, len + 1);
    return a;
}

static bool path_make_dir(path a) {
    cstr cs  = a->chars; /// this can be a bunch of folders we need to make in a row
    sz   len = strlen(cs);
    for (num i = 1; i < len; i++) {
        if (cs[i] == '/' || i == len - 1) {
            bool set = false;
            if (cs[i] == '/') { cs[i] = 0; set = true; }
            mkdir(cs, 0755);
            if (set) cs[i] = '/';
        }
    }
    struct stat st = {0};
    return stat(cs, &st) == 0 && S_ISDIR(st.st_mode);
}

static bool path_is_empty(path a) {
    int    n = 0;
    struct dirent *d;
    DIR   *dir = opendir(a->chars);

    if (dir == NULL)  // Not a directory or doesn't exist
        return false;

    while ((d = readdir(dir)) != NULL) {
        if (++n > 2)
            break;
    }
    closedir(dir);
    return n <= 2;  // Returns true if the directory is empty (only '.' and '..' are present)
}

static string path_stem(path a) {
    cstr cs  = a->chars; /// this can be a bunch of folders we need to make in a row
    sz   len = strlen(cs);
    string res = construct(string, sz, 256);
    sz     dot = 0;
    for (num i = len - 1; i >= 0; i--) {
        if (cs[i] == '.')
            dot = i;
        if (cs[i] == '/' || i == 0) {
            int offset = cs[i] == '/';
            cstr start = &cs[i + offset];
            int n_bytes = (dot > 0 ? dot : len) - (i + offset);
            memcpy(res->chars, start, n_bytes);
            res->len = n_bytes;
            break;
        }
    }
    return res;
}

static string path_filename(path a) {
    cstr cs  = a->chars; /// this can be a bunch of folders we need to make in a row
    sz   len = strlen(cs);
    string res = construct(string, sz, 256);
    for (num i = len - 1; i >= 0; i--) {
        if (cs[i] == '/' || i == 0) {
            cstr start = &cs[i + cs[i] == '/'];
            int n_bytes = len - i;
            memcpy(res->chars, start, n_bytes);
            res->len = n_bytes;
            break;
        }
    }
    return res;
}

static path path_absolute(path a) {
    path  result   = new(path);
    result->chars  = strdup(realpath(a->chars, null));
    return result;
}

static path path_directory(path a) {
    path result = new(path);
    result->chars = dirname(strdup(a->chars));
    return result;
}

static path path_parent(path a) {
    int len = strlen(a->chars);
    char *cp = calloc(len + 4, 1);
    memcpy(cp, a->chars, len);
    if (a->chars[len - 1] == '\\' || a->chars[len - 1] == '/')
        memcpy(&cp[len], "..", 3);
    else
        memcpy(&cp[len], "/..", 4);
    char *dir_name = dirname(cp);
    path  result   = new(path);
    result->chars  = strdup(dir_name);
    return result;
}

static path path_change_ext(path a, cstr ext) {
    int   e_len = strlen(ext);
    int     len = strlen(a->chars);
    int ext_pos = -1;
    for (int i = len - 1; i >= 0; i--) {
        if (a->chars[i] == '/')
            break;
        if (a->chars[i] == '.') {
            ext_pos = i;
            break;
        }
    }
    path res = new(path);
    res->chars = calloc(32 + len + e_len, 1);
    if (ext_pos >= 0) {
        memcpy( res->chars, a->chars, ext_pos + 1);
        if (e_len)
            memcpy(&res->chars[ext_pos + 1], ext, e_len);
        else
            res->chars[ext_pos] = 0;
    } else {
        memcpy( res->chars, a->chars, len);
        if (e_len) {
            memcpy(&res->chars[len], ".", 1);
            memcpy(&res->chars[len + 1], ext, e_len);
        }
    }
    return res;
}

static path path_cwd(sz size) {
    path a = new(path);
    a->chars = calloc(size, 1);
    char* res = getcwd(a->chars, size);
    assert(res != null);
    return a;
}

static bool path_exists(path a) {
    FILE *file = fopen(a->chars, "r");
    if (file) {
        fclose(file);
        return true; // File exists
    }
    return false; // File does not exist
}

static u64 path_hash(path a) {
    return fnv1a_hash(a->chars, strlen(a->chars), OFFSET_BASIS);
}

// implement several useful 
static A path_read(path a, AType type) {
    FILE* f = fopen(a->chars, "rb");
    if (!f) return null;
    if (type == typeof(string)) {
        fseek(f, 0, SEEK_END);
        sz flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        string a = construct(string, sz, flen + 1);
        size_t n = fread(a->chars, 1, flen, f);
        fclose(f);
        assert(n == flen);
        a->len   = flen;
        return a;
    }
    assert(false);
    return null;
}

item item_with_symbol(item f, symbol key, A val) {
    f->key = construct(string, cstr, (cstr)key, strlen(key));
    f->val = hold(val);
    return f;
}

void* primitive_ffi_arb(AType ptype) {
    if (ptype == typeof(u8)) return &ffi_type_uint8;
    if (ptype == typeof(i8)) return &ffi_type_sint8;
    if (ptype == typeof(u16)) return &ffi_type_uint16;
    if (ptype == typeof(i16)) return &ffi_type_sint16;
    if (ptype == typeof(u32)) return &ffi_type_uint32;
    if (ptype == typeof(i32)) return &ffi_type_sint32;
    if (ptype == typeof(u64)) return &ffi_type_uint64;
    if (ptype == typeof(i64)) return &ffi_type_sint64;
    if (ptype == typeof(f32)) return &ffi_type_float;
    if (ptype == typeof(f64)) return &ffi_type_double;
    //if (ptype == typeof(f128)) return &ffi_type_longdouble;
    if (ptype == typeof(cstr)) return &ffi_type_pointer;
    if (ptype == typeof(symbol)) return &ffi_type_pointer;
    if (ptype == typeof(bool)) return &ffi_type_uint32;
    if (ptype == typeof(num)) return &ffi_type_sint64;
    if (ptype == typeof(sz)) return &ffi_type_sint64;
    if (ptype == typeof(none)) return &ffi_type_void;
    if (ptype == typeof(AType)) return &ffi_type_pointer;
    if (ptype == typeof(handle)) return &ffi_type_pointer;
    assert(false);
    return null;
}

define_class(A)

define_abstract(numeric)
define_abstract(string_like)
define_abstract(nil)
define_abstract(raw)
define_abstract(imported)

define_primitive( u8,    numeric, A_TRAIT_INTEGRAL)
define_primitive(u16,    numeric, A_TRAIT_INTEGRAL)
define_primitive(u32,    numeric, A_TRAIT_INTEGRAL)
define_primitive(u64,    numeric, A_TRAIT_INTEGRAL)
define_primitive( i8,    numeric, A_TRAIT_INTEGRAL)
define_primitive(i16,    numeric, A_TRAIT_INTEGRAL)
define_primitive(i32,    numeric, A_TRAIT_INTEGRAL)
define_primitive(i64,    numeric, A_TRAIT_INTEGRAL)
define_primitive(bool,   numeric, A_TRAIT_INTEGRAL)
define_primitive(num,    numeric, A_TRAIT_INTEGRAL)
define_primitive(sz,     numeric, A_TRAIT_INTEGRAL)
define_primitive(f32,    numeric, A_TRAIT_REALISTIC)
define_primitive(f64,    numeric, A_TRAIT_REALISTIC)
//define_primitive(f128,   numeric, A_TRAIT_REALISTIC)
define_primitive(cstr,   string_like, 0)
define_primitive(symbol, string_like, 0)
define_primitive(none,   nil, 0)
define_primitive(AType,  raw, 0)
define_primitive(handle, raw, 0)

define_enum(OPType)
define_class(path)
define_class(string)
define_class(item)
define_proto(collection)
define_class(list)
define_class(array)
define_class(vector)

define_alias(array, ATypes, AType)

/*
map map_parse(map a, int argc, cstr *argv, map def) {
    map iargs = new(map);
    for (int ai = 0; ai < argc; ai++) {
        cstr ps = argv[ai];
        ///
        if (ps[0] == '-') {
            bool   is_single = ps[1] != '-';
            mx key {
                cstr(&ps[is_single ? 1 : 2]), typeof(char)
            };
            field* found;
            if (is_single) {
                for (field &df: def.fields()) {
                    symbol s = symbol(df.key.mem->origin);
                    if (ps[1] == s[0]) {
                        found = &df;
                        break;
                    }
                }
            } else found = def->lookup(key);
            ///
            if (found) {
                str     aval = str(argv[ai + 1]);
                type_t  type = found->value.mem->type;
                /// from_string should use const str&
                /// 
                iargs[key] = mx::from_string((cstr)aval.mem->origin, type); /// static method on mx that performs the busy work of this
            } else {
                printf("arg not found: %s\n", key.mem->get<char>(0)); // shouldnt do this dangerous thing with strings
                return {};
            }
        }
    }
    ///
    /// return results in order of defaults, with default value given
    map res = map();
    for(field &df:def.data->fields.elements<field>()) {
        field *ov = iargs->lookup(df.key);
        res.data->fields += field { ion::hold(df.key), ov ? ion::hold(ov->value) : ion::hold(df.value) };
    }
    return res;
}
*/