#include <A.h>

static global_init_fn* call_after;
static num             call_after_alloc;
static num             call_after_count;
static A_f**           types;
static num             types_alloc;
static num             types_len;

void A_lazy_init(global_init_fn fn) {
    if (call_after_count == call_after_alloc) {
        global_init_fn prev       = call_after;
        num            alloc_prev = call_after_alloc;
        call_after                = calloc(32 + (call_after_alloc << 1), sizeof(global_init_fn));
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
        types_alloc = 32 + (types_alloc << 1);
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

A A_alloc(A_f* type, num count) {
    A a = calloc(1, (type == typeof(A) ? 0 : sizeof(struct A)) + type->size * count);
    a->type   = type;
    a->origin = a;
    a->data   = &a[1];
    a->count  = count;
    a->alloc  = count;
    A_f* a_type = &A_type;
    A_f* current = type;
    while (current) {
        if (current->init) /// init not being set on here somehow, even though it should be emitting string_type.init = &A_init
            current->init(a->data);
        if (current == a_type)
            break;
        current = current->parent;
    }
    return a->data; /// object(a) == this operation
}

A A_realloc(A a, num alloc) {
    A obj = object(a);
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
    A   obj = object(a);
    assert(obj->type->traits == A_TRAIT_PRIMITIVE);
    num sz  = obj->type->size;
    if (obj->count == obj->alloc)
        A_realloc(a, 32 + obj->alloc << 1);
    memcpy(&((u8*)obj->data)[obj->count++ * sz], value, sz);
}

method_t* method_with_address(handle address, AType rtype, array atypes) {
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
        (ffi_type*)rtype->arb, ffi_args);
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
    member_t* mem = A_member(type, A_TYPE_IMETHOD | A_TYPE_SMETHOD, method_name);
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

        /// for each member of type
        for (num m = 0; m < type->member_count; m++) {
            member_t* mem = &type->members[m];
            if (mem->member_type & (A_TYPE_IMETHOD | A_TYPE_SMETHOD | A_TYPE_CONSTRUCT)) {
                void* address = 0;
                memcpy(&address, &((u8*)type)[mem->offset], sizeof(void*));
                assert(address);
                array args = construct(array, sz, mem->args.count);
                for (num i = 0; i < mem->args.count; i++)
                    args->elements[i] = ((A_f**)&mem->args.arg_0)[i];
                args->len = mem->args.count;
                mem->method = method_with_address(address, mem->type, args);
            }
        }
    }
}

member_t* A_member(A_f* type, enum A_TYPE member_type, char* name) {
    for (num i = 0; i < type->member_count; i++) {
        member_t* mem = &type->members[i];
        if (mem->member_type & member_type && strcmp(mem->name, name) == 0)
            return mem;
    }
    return 0;
}

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
static A   A_new_default(A_t type, num count) {
    return A_alloc(type, count);
}
static void A_init      (A a) { }
static void A_destructor(A a) { }
static u64  A_hash      (A a) { return (u64)(size_t)a; }
static bool A_boolean   (A a) { return (bool)(size_t)a; }

A A_method(A_f* type, char* method_name, array args);

/// these pointers are invalid for A since they are in who-knows land, but the differences would be the same
static i32 A_compare(A a, A b) {
    return (i32)(a - b);
}

static void  string_destructor(string a) { free(a->chars); }
static num   string_compare(string a, string b) { return strcmp(a->chars, b->chars); }
static array string_split(string a, A sp) {
    return null;
}
static num   string_index_of(string a, cstr cs) {
    char* f = strstr(a->chars, cs);
    return f ? (num)(f - a->chars) : (num)-1;
}

u64 fnv1a_hash(const void* data, size_t length, u64 hash) {
    const u8* bytes = (const u8*)data;
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];  // xor bottom with current byte
        hash *= FNV_PRIME; // multiply by FNV prime
    }
    return hash;
}

static u64 field_hash(field f) {
    return M(A, hash, f->key);
}

static u64 string_hash(string a) {
    if (a->h) return a->h;
    a->h = fnv1a_hash(a->chars, a->len, OFFSET_BASIS);
    return a->h;
}

string string_with_sz(string a, sz size) {
    a->alloc = size;
    a->chars = (char*)calloc(1, a->alloc);
    return a;
}

string string_with_cstr(string a, cstr value, num len) {
    if (len == -1) len = strlen(value);
    a->alloc = len + 1;
    a->len   = len;
    a->chars = (char*)calloc(1, a->alloc);
    memcpy(a->chars, value, len);
    a->chars[len] = 0;
    return a;
}

/// collection -------------------------
static void collection_push(collection a, A b) {
    assert(false);
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
            type = type->parent;
        }
        free(aa);
    }
}

A object(A instance) {
    return (instance - 1)->origin;
}

A data(A instance) {
    A obj = object(instance);
    return obj->data;
}

/// list -------------------------
static void list_push(list a, A e) {
    item n = new(item);
    n->element = e; /// held already by caller
    if (a->last) {
        a->last->next = n;
        n->prev       = a->last;
    } else {
        a->first      = n;
    }
    a->last = n;
    a->count++;
}

static A list_remove(list a, num index) {
    num i = 0;
    for (item ai = a->first; ai; ai = ai->next) {
        if (i++ == index) {
            if (ai == a->first) a->first = ai->next;
            if (ai == a->last)  a->last  = ai->prev;
            if (ai->prev)       ai->prev->next = ai->next;
            if (ai->next)       ai->next->prev = ai->prev;
            a->count--;
        }
    }
}

static num list_compare(list a, list b) {
    num diff  = a->count - b->count;
    if (diff != 0)
        return diff;
    for (item ai = a->first, bi = b->first; ai; ai = ai->next, bi = bi->next) {
        A_f* ai_t = *(A_f**)&((A)ai->element)[-1];
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
            return i->element;
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

static bool array_cast_bool(array a) {
    return a->len > 0;
}

bool is_meta(A a) {
    AType t = typeid(a);
    return t->meta.count > 0;
}

bool is_meta_compatible(A a, A b) {
    AType t = typeid(a);
    if (is_meta(a)) {
        AType bt = typeid(b);
        num found = 0;
        for (num i = 0; i < t->meta.count; i++) {
            AType mt = ((AType*)&t->meta.arg_0)[i];
            if (mt == bt)
                found++;
        }
        return found > 0;
    }
    return false;
}

void array_push(array a, A b) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    AType t = typeid(a);
    if (is_meta(a))
        assert(is_meta_compatible(a, b));
    a->elements[a->len++] = b;
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
    return array_push(a, b);
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

static void array_push_symbols(array a, char* f, ...) {
    va_list args;
    va_start(args, f);
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
    va_start(args, NULL);

    for (;;) {
        A arg = va_arg(args, A);
        if (!arg)
            break;
        assert(!validate || validate == typeid(arg));
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
        if (a -> elements[i] == b)
            return i;
    }
    return -1;
}

static bool array_boolean(array a) { return a && a->len > 0; }

static array array_with_sz(array a, sz size) {
    array_alloc_sz(a, size);
}

static u64 fn_hash(fn f) {
    return (u64)f->method->address;
}

static A fn_call(fn f, array args) {
    return method_call(f->method, args);
}

static fn fn_with_ATypes(fn f, ATypes atypes, AType rtype, handle address) {
    assert(atypes->len <= 8);
    f->method = method_with_address(address, rtype, atypes);
    return f;
}

define_class(fn)


static void vector_push(vector a, A b) {
    A obj = object(a);
    if (obj->alloc == obj->count)
        A_realloc(obj, 32 + (obj->alloc < 1));
    u8* dst = obj->data;
    num sz = obj->type->size;
    memcpy(&dst[sz * obj->count++], b, sz);
}

/// the only thing we would look out for here is a generic 
/// user of 'collection' calling object on this return result
static A vector_pop(vector a) {
    A obj = object(a);
    assert(obj->count > 0);
    u8* dst = obj->data;
    num sz = obj->type->size;
    return (A)&dst[sz * --obj->count];
}

static num vector_compare(vector a, vector b) {
    A a_object = object(a);
    A b_object = object(b);
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
    A a_object = object(a);
    u8* a_data = data(a);
    num sz = a_object->type->size;
    return (A)&a_data[i * sz];
}

static num vector_count(vector a) {
    A a_object = object(a);
    return a_object->count;
}

static num vector_index_of(vector a, A b) {
    A a_object = object(a);
    u8* a_data = data(a);
    u8* b_data = data(b);
    num sz = a_object->type->size;
    for (num i = 0; i < a_object->count; i++) {
        if (memcmp(&a_data[sz * i], b_data, sz) == 0)
            return i;
    }
    return -1;
}

static bool vector_boolean(vector a) {
    A a_object = object(a);
    return a_object->count > 0;
}

u64 vector_hash(vector a) {
    A obj = object(a);
    return fnv1a_hash(obj->data, obj->type->size * obj->count, OFFSET_BASIS);
}

path path_with_cstr(path a, cstr path) {
    num len = strlen(path);
    a->chars = calloc(len + 1, 1);
    memcpy(a->chars, path, len + 1);
    return a;
}

bool path_exists(path a) {
    FILE *file = fopen(a->chars, "r");
    if (file) {
        fclose(file);
        return true; // File exists
    }
    return false; // File does not exist
}

u64 path_hash(path a) {
    return fnv1a_hash(a->chars, strlen(a->chars), OFFSET_BASIS);
}

// implement several useful 
A path_read(path a, AType type) {
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

field field_with_cstr(field f, cstr key, A val) {
    f->key = construct(string, cstr, key, strlen(key));
    f->val = hold(val);
    return f;
}

define_class(A)

define_primitive( u8,  ffi_type_uint8,  A_TRAIT_INTEGRAL)
define_primitive(u16,  ffi_type_uint16, A_TRAIT_INTEGRAL)
define_primitive(u32,  ffi_type_uint32, A_TRAIT_INTEGRAL)
define_primitive(u64,  ffi_type_uint64, A_TRAIT_INTEGRAL)
define_primitive( i8,  ffi_type_sint8,  A_TRAIT_INTEGRAL)
define_primitive(i16,  ffi_type_sint16, A_TRAIT_INTEGRAL)
define_primitive(i32,  ffi_type_sint32, A_TRAIT_INTEGRAL)
define_primitive(i64,  ffi_type_sint64, A_TRAIT_INTEGRAL)
define_primitive(f32,  ffi_type_float,  A_TRAIT_REALISTIC)
define_primitive(f64,  ffi_type_double, A_TRAIT_REALISTIC)
define_primitive(f128, ffi_type_longdouble, A_TRAIT_REALISTIC)
define_primitive(cstr, ffi_type_pointer, 0)
define_primitive(bool, ffi_type_uint32, A_TRAIT_INTEGRAL)
define_primitive(num,  ffi_type_sint64, A_TRAIT_INTEGRAL)
define_primitive(sz,   ffi_type_sint64, A_TRAIT_INTEGRAL)
define_primitive(none, ffi_type_void, 0)
define_primitive(AType, ffi_type_pointer, 0)
define_primitive(handle, ffi_type_pointer, 0)

define_enum(OPType)
define_class(path)
define_class(string)
define_class(item)
define_class(field)
define_proto(collection)
define_class(list)
define_class(array)
define_class(vector)

define_alias(array, ATypes, AType)




















#include <A.h>

static array           keywords;

#define EType_meta(X,Y) \
    enum_value(X,Y, Undefined) \
    enum_value(X,Y, Statements) \
    enum_value(X,Y, Assign) \
    enum_value(X,Y, AssignAdd) \
    enum_value(X,Y, AssignSub) \
    enum_value(X,Y, AssignMul) \
    enum_value(X,Y, AssignDiv) \
    enum_value(X,Y, AssignOr) \
    enum_value(X,Y, AssignAnd) \
    enum_value(X,Y, AssignXor) \
    enum_value(X,Y, AssignShiftR) \
    enum_value(X,Y, AssignShiftL) \
    enum_value(X,Y, AssignMod) \
    enum_value(X,Y, If) \
    enum_value(X,Y, For) \
    enum_value(X,Y, While) \
    enum_value(X,Y, DoWhile) \
    enum_value(X,Y, Break) \
    enum_value(X,Y, LiteralReal) \
    enum_value(X,Y, LiteralInt) \
    enum_value(X,Y, LiteralStr) \
    enum_value(X,Y, LiteralStrInterp) \
    enum_value(X,Y, Array) \
    enum_value(X,Y, AlphaIdent) \
    enum_value(X,Y, Var) \
    enum_value(X,Y, Add) \
    enum_value(X,Y, Sub) \
    enum_value(X,Y, Mul) \
    enum_value(X,Y, Div) \
    enum_value(X,Y, Or) \
    enum_value(X,Y, And) \
    enum_value(X,Y, Xor) \
    enum_value(X,Y, MethodCall) \
    enum_value(X,Y, MethodReturn)
declare_enum(EType)
define_enum(EType)

#define ident_meta(X,Y,Z) \
    i_intern(X,Y,Z, string,    value) \
    i_intern(X,Y,Z, path,      fname) \
    i_intern(X,Y,Z, array,     members_cache) \
    i_intern(X,Y,Z, int,       line_num) \
    i_intern(X,Y,Z, u64,       h) \
    i_method(X,Y,Z, array,     split_members) \
    i_method(X,Y,Z, EType,     is_numeric) \
    i_method(X,Y,Z, EType,     is_string) \
    i_method(X,Y,Z, EType,     is_alpha) \
    i_construct(X,Y,Z,         cstr, path, num) \
    i_override_m(X,Y,Z, u64, hash) \
    i_override_m(X,Y,Z, num, compare)
declare_class(ident)

#define enode_meta(X,Y,Z) \
    i_intern  (X,Y,Z, EType,      etype) \
    i_intern  (X,Y,Z, A,          value) \
    i_intern  (X,Y,Z, array,      operands) \
    i_intern  (X,Y,Z, array,      references) \
    s_method(X,Y,Z, enode,      create_operation,   EType, array, array) \
    s_method(X,Y,Z, enode,      create_value,       EType, A) \
    s_method(X,Y,Z, enode,      method_call,        ident, array) \
    s_method(X,Y,Z, A,          lookup,             array, ident, bool) \
    s_method(X,Y,Z, string,     string_interpolate, A, array) \
    i_method(X,Y,Z, A,          exec,               array) \
    i_override_m(X,Y,Z, bool, boolean)
declare_class(enode)

#define MemberType_meta(X,Y) \
    enum_value(X,Y, Undefined) \
    enum_value(X,Y, Variable) \
    enum_value(X,Y, Lambda) \
    enum_value(X,Y, Method) \
    enum_value(X,Y, Constructor)
declare_enum(MemberType)
define_enum(MemberType)

/// silver types are identified by module member
typedef struct define_t* silver_t;

/// everything a member can use; it could be expressed with unions in a more succinct way
#define member_def_meta(X,Y,Z) \
    i_intern(X,Y,Z,  bool,            is_template) \
    i_intern(X,Y,Z,  bool,            intern) \
    i_intern(X,Y,Z,  bool,            is_static) \
    i_intern(X,Y,Z,  bool,            is_public) \
    i_intern(X,Y,Z,  fn_t,            resolve) \
    i_intern(X,Y,Z,  MemberType,      member_type) \
    i_intern(X,Y,Z,  string,          name) \
    i_intern(X,Y,Z,  array,           args) \
    i_intern(X,Y,Z,  silver_t,        type) \
    i_intern(X,Y,Z,  string,          base_class) \
    i_intern(X,Y,Z,  array,           type_tokens) \
    i_intern(X,Y,Z,  array,           group_tokens) \
    i_intern(X,Y,Z,  array,           value) \
    i_intern(X,Y,Z,  array,           base_forward) \
    i_intern(X,Y,Z,  bool,            is_ctr) \
    i_intern(X,Y,Z,  enode,           translation)
declare_class(member_def)

#define EMembership_meta(X,Y) \
    enum_value(X,Y, normal) \
    enum_value(X,Y, internal)
declare_enum(EMembership)
define_enum(EMembership)

#define Parser_meta(X,Y,Z) \
    i_intern(X,Y,Z, array,  tokens) \
    i_intern(X,Y,Z, string, fname) \
    i_intern(X,Y,Z, num,    cur) \
    i_intern(X,Y,Z, EMembership, membership) \
    i_intern(X,Y,Z, string, keyword) \
    i_method(X,Y,Z, ident,  token_at,          num) \
    i_method(X,Y,Z, ident,  next) \
    i_method(X,Y,Z, ident,  pop) \
    i_method(X,Y,Z, num,    consume) \
    i_method(X,Y,Z, array,  parse_args, A) \
    i_method(X,Y,Z, EType,  expect,            ident, array) \
    i_method(X,Y,Z, ident,  relative,          num) \
    i_method(X,Y,Z, EType,  is_assign,         ident) \
    i_method(X,Y,Z, member_def, read_member,     A, member_def) \
    i_method(X,Y,Z, enode,  parse_statements) \
    i_method(X,Y,Z, enode,  parse_expression) \
    i_method(X,Y,Z, array,  parse_raw_block) \
    i_method(X,Y,Z, enode,  parse_statement) \
    i_method(X,Y,Z, i64,    parse_numeric,     ident) \
    i_method(X,Y,Z, EType,  is_var,            ident) \
    i_method(X,Y,Z, enode,  parse_add) \
    i_method(X,Y,Z, enode,  parse_mult) \
    i_method(X,Y,Z, enode,  parse_primary) \
    i_method(X,Y,Z, array,  read_type) \
    i_construct(X,Y,Z,      array, path)
declare_class(Parser)

#define module_t_meta(X,Y,Z) \
    i_intern(X,Y,Z, array,  tokens) \
    i_intern(X,Y,Z, path,   module_name) \
    i_intern(X,Y,Z, array,  imports) \
    i_intern(X,Y,Z, array,  types) \
    i_intern(X,Y,Z, handle, app) \
    i_intern(X,Y,Z, array,  defines) \
    i_construct(X,Y,Z,      path) \
    i_method(X,Y,Z, A, find_implement, ident) \
    i_method(X,Y,Z, A, find_class,     ident) \
    i_method(X,Y,Z, A, find_struct,    ident) \
    i_method(X,Y,Z, none,   graph) \
    i_method(X,Y,Z, none,   c99) \
    i_method(X,Y,Z, none,   run)
declare_class(module_t)

#define define_t_meta(X,Y,Z) \
    i_intern(X,Y,Z,    A_f*,           type) \
    i_intern(X,Y,Z,    array,          tokens) \
    i_intern(X,Y,Z,    string,         name) \
    i_intern(X,Y,Z,    string,         keyword) \
    i_intern(X,Y,Z,    EMembership,    membership) \
    i_intern(X,Y,Z,    module_t,       module) \
    i_intern(X,Y,Z,    bool,           is_translated) \
    i_method(X,Y,Z,  none, read_members) \
    i_method(X,Y,Z,  none, resolve_members) \
    i_construct(X,Y,Z, Parser)
declare_class(define_t)

/// dont need this if we are utilizing our own types
/// its trivial to know if we created the type with silver, or not
#define class_model_meta(X,Y) \
    enum_value(X,Y, allocated) \
    enum_value(X,Y, boolean_32) \
    enum_value(X,Y, unsigned_8) \
    enum_value(X,Y, unsigned_16) \
    enum_value(X,Y, unsigned_32) \
    enum_value(X,Y, unsigned_64) \
    enum_value(X,Y, signed_8) \
    enum_value(X,Y, signed_16) \
    enum_value(X,Y, signed_32) \
    enum_value(X,Y, signed_64) \
    enum_value(X,Y, real_32) \
    enum_value(X,Y, real_64) \
    enum_value(X,Y, real_128)
declare_enum(class_model)
define_enum(class_model)

#define struct_t_meta(X,Y,Z)  define_t_meta(define_t,Y,Z) \
    i_intern(X,Y,Z,    array,  members) \
    i_override_ctr(X,Y,Z,      Parser)
declare_mod(struct_t, define_t)

#define var_t_meta(X,Y,Z)  define_t_meta(define_t,Y,Z) \
    i_intern(X,Y,Z,    A, member)
declare_mod(var_t, define_t)

#define class_t_meta(X,Y,Z)  define_t_meta(define_t,Y,Z) \
    i_intern(X,Y,Z,    class_model, model) \
    i_intern(X,Y,Z,    string, from) \
    i_intern(X,Y,Z,    array,  members) \
    i_intern(X,Y,Z,    array,  friends) \
    i_override_m(X,Y,Z, none, read_members) \
    i_override_m(X,Y,Z, none, resolve_members) \
    i_override_ctr(X,Y,Z,      Parser) \
    i_construct(X,Y,Z,         AType)
declare_mod(class_t, define_t)

#define enum_t_meta(X,Y,Z) define_t_meta(define_t,Y,Z) \
    i_intern(X,Y,Z,    array,  symbols) \
    i_override_ctr(X,Y,Z,      Parser)
declare_mod(enum_t, define_t)

#define import_t_meta(X,Y,Z)  define_t_meta(define_t,Y,Z) \
    i_intern(X,Y,Z,    string, import_name) \
    i_intern(X,Y,Z,    string, source) \
    i_intern(X,Y,Z,    string, shell) \
    i_intern(X,Y,Z,    array,  links) \
    i_intern(X,Y,Z,    array,  includes) \
    i_intern(X,Y,Z,    array,  defines) \
    i_intern(X,Y,Z,    string, isolate_namespace) \
    i_intern(X,Y,Z,    string, module_path) \
    i_override_ctr(X,Y,Z,      Parser)
declare_mod(import_t, define_t)

EType ident_is_alpha(ident a) {
    char* t = a->value->chars;
    return isalpha(*t);
}

EType ident_is_string(ident a) {
    char* t = a->value->chars;
    return t[0] == '"' ? EType_LiteralStr : t[0] == '\'' ? EType_LiteralStrInterp : EType_Undefined;
}

EType ident_is_numeric(ident a) {
    char* t = a->value->chars; /// even a null string can get valid pointer; a pointer to the length which is set to 0 is a null string
    return (t[0] >= '0' && t[0] <= '9') ? (strchr(t, '.') ? 
        EType_LiteralReal : EType_LiteralInt) : EType_Undefined;
}

num ident_compare(ident a, ident b) {
    return M(string, compare, a->value, b->value);
}

u64 ident_hash(ident a) {
    if (!a->h) {
        u64 h = OFFSET_BASIS;
            h *= FNV_PRIME;
            h ^= M(string, hash, a->value);
        a->h = h;
    }
    return a->h;
}

array ident_split_members(ident a, A obj) {
    if (!a->members_cache)
        a->members_cache = M(string, split, a->value, obj);
    return a->members_cache;
}

string ident_with_cstr(ident a, cstr token, path fname, num line_num) {
    a->value = construct(string, cstr, token, strlen(token));
    a->fname = hold(fname);
    a->line_num = line_num;
}

void ident_with_string(ident a, string token, path fname, num line_num) {
    a->value = hold(token);
    a->fname = hold(fname);
    a->line_num = line_num;
}

enode enode_create_operation(EType etype, array ops, array references) {
    return null;
}

enode enode_create_value(EType etype, A value) {
    return null;
}

enode enode_method_call(EType etype, ident method, array args) {
    return null;
}

A enode_lookup(array stack, ident id, bool top_only) {
    return null;
}

string enode_string_interpolate(A input, array stack) {
    return null;
}

A enode_exec(enode op, array stack) {
    return null;
}

bool enode_boolean(enode a) {
    return a->etype != EType_Undefined;
}

cstr cs(string s) { return s ? s->chars : null; }

static ident parse_token(cstr start, num len, path fname, int line_num) {
    while (start[len - 1] == '\t' || start[len - 1] == ' ')
        len--;
    string all = construct(string, cstr, start, len);
    char t = all->chars[0];
    bool is_number = (t == '-' || (t >= '0' && t <= '9'));
    return construct(ident, cstr, cs(all), fname, line_num); /// mr b: im a token!  line-num can be used for breakpoints (need the file path too)
}

static void ws(char **cur) {
    while (**cur == ' ' || **cur == '\t') {
        ++(*cur);
    }
}

void assertion(Parser parser, bool is_true, cstr message, ...) {
    if (!is_true) {
        char buffer[1024];
        va_list args;
        va_start(args, message);
        vsprintf(buffer, message, args);
        va_end(args);
        printf("%s\n", buffer);
        exit(-1);
    }
}

ident next(Parser parser) {
    return M(Parser, next, parser);
}

ident next_n(Parser parser, num rel) {
    return M(Parser, relative, parser, rel);
}

bool next_is(Parser parser, cstr token) {
    ident id = M(Parser, next, parser);
    return strcmp(id->value->chars, token) == 0;
}

bool ident_is(ident i, cstr str) {
    return strcmp(i->value->chars, str) == 0;
}

bool pop_is(Parser parser, cstr token) {
    ident id = M(Parser, pop, parser);
    return strcmp(id->value->chars, token) == 0;
}

ident pop(Parser parser) {
    return M(Parser, pop, parser);
}

EType is_string(ident i) {
    return M(ident, is_string, i);
}

EType is_numeric(ident i) {
    return M(ident, is_numeric, i);
}

void consume(Parser parser) {
    M(Parser, consume, parser);
}

EType is_alpha(ident i) {
    return M(ident, is_alpha, i);
}

string ident_string(ident i) {
    return i->value;
}

bool ident_equals(ident a, ident b) {
    return strcmp(a->value->chars, b->value->chars) == 0;
}

bool file_exists(cstr filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true; // File exists
    }
    return false; // File does not exist
}

/// parsing functions for each keyword
/// -------------------------------------------
define_t define_t_with_Parser(define_t import, Parser parser) {
    assert(false);
    return null;
}

none define_t_read_members(define_t a) { }

none define_t_resolve_members(define_t a) { } /// this should output c99

import_t import_t_with_Parser(import_t import, Parser parser) {
    assertion(parser, pop_is(parser, "import"), "expected import");
    if (next_is(parser, "[")) {
        pop(parser);
        for (;;) {
            if (next_is(parser, "]")) {
                pop(parser);
                break;
            }
            ident arg_name = pop(parser);
            assertion(parser, is_alpha(arg_name), "expected identifier for import arg");
            assertion(parser, pop_is(parser, ":"), "expected : after import arg (argument assignment)");
            if (ident_is(arg_name, "name")) {
                ident token_name = pop(parser);
                assertion(parser, !is_string(token_name), "expected token for import name");
                import->name = ident_string(token_name);
            } else if (ident_is(arg_name, "links")) {
                assertion(parser, pop_is(parser, "["), "expected array for library links");
                for (;;) {
                    ident link = pop(parser);
                    
                    if (ident_is(link, "]")) break;
                    assertion(parser, is_string(link), "expected library link string");

                    array_push(import->links, ident_string(link));
                    if (next_is(parser, ",")) {
                        pop(parser);
                        continue;
                    } else {
                        assertion(parser, pop_is(parser, "]"), "expected ] in includes");
                        break;
                    }
                }
            } else if (ident_is(arg_name, "includes")) {
                assertion(parser, pop_is(parser, "["), "expected array for includes");
                for (;;) {
                    ident include = pop(parser);
                    if (ident_is(include, "]")) break;
                    assertion(parser, is_string(include), "expected include string");
                    array_push(import->includes, ident_string(include));
                    if (next_is(parser, ",")) {
                        pop(parser);
                        continue;
                    } else {
                        assertion(parser, pop_is(parser, "]"), "expected ] in includes");
                        break;
                    }
                }
            } else if (ident_is(arg_name, "source")) {
                ident token_source = pop(parser);
                assertion(parser, is_string(token_source), "expected quoted url for import source");
                import->source = ident_string(token_source);
            } else if (ident_is(arg_name, "shell")) {
                ident token_shell = pop(parser);
                assertion(parser, is_string(token_shell), "expected shell invocation for building");
                import->shell = ident_string(token_shell);
            } else if (ident_is(arg_name, "defines")) {
                // none is a decent name for null.
                assertion(parser, false, "not implemented");
            } else {
                assertion(parser, false, "unknown arg: %s", arg_name->value->chars);
            }

            if (next_is(parser, ","))
                pop(parser);
            else {
                assertion(parser, pop_is(parser, "]"), "expected ] after end of args, with comma inbetween");
                break;
            }
        }
    } else {
        ident module_name = pop(parser);
        ident as = next(parser);
        if (ident_is(as, "as")) {
            consume(parser);
            import->isolate_namespace = ident_string(pop(parser)); /// inlay overrides this -- it would have to; modules decide if their types are that important
        }
        //assertion(parser.is_string(mod), "expected type identifier, found {0}", { type });
        assertion(parser, is_alpha(module_name), "expected variable identifier, found %s", module_name->value->chars);
        import->name = hold(module_name->value);
    }
}

struct_t struct_t_with_Parser(struct_t a, Parser parser) {
    assert(false);
    return null;
}

/// class_t with AType for storage; this is for primitive registration
class_t class_t_with_AType(class_t a, AType storage) {
    a->membership    = EMembership_normal;
    a->keyword       = construct(string, cstr, "class", -1);
    return a;
}

/// this constructor overrides define_t
/// even when you dont override constructor, you are still given the type of class you new' with
class_t class_t_with_Parser(
        class_t cl, Parser parser) {
    cl->membership    = parser->membership;
    cl->keyword       = hold(parser->keyword);
    cl->members       = new(array);
    cl->tokens        = new(array);
    /// parse class members
    ident ikeyword = construct(ident, cstr, cs(cl->keyword), null, 0);
    ident inext = M(Parser, pop, parser);
    bool cmp = M(A, compare, inext, ikeyword);
    assertion(parser, cmp == 0, "expected %s", cl->keyword->chars);
    assertion(parser, is_alpha(next(parser)), "expected class identifier");
    ident iname = pop(parser);
    cl->name = ident_string(iname);
    if (next_is(parser, ":")) {
        consume(parser);
        cl->from = ident_string(pop(parser));
    }
    ident start = pop(parser);
    assertion(parser, ident_is(start, "["), "expected beginning of class");
    
    /// do not process contents yet
    M(array, push, cl->tokens, hold(start));
    int level = 1;
    for (;;) {
        ident t = pop(parser);
        if (!t->value) {
            assertion(parser, false, "expected ]");
            break;
        }
        M(array, push, cl->tokens, hold(t));
        if (ident_is(t, "[")) {
            level++;
        } else if (ident_is(t, "]")) {
            level--;
            if (level == 0) {
                break;
            }
        }
    }
    return cl;
}

/// we read members once other identies are registered
/// we need a read, then resolve per member
/// resolve will infact create C99 code

none class_t_read_members(class_t cl) {
    Parser parser = construct(Parser, array, cl->tokens, cl->module->module_name);
    /// we parse the tokens afterwards with another Parser instance
    assertion(parser, ident_is(pop(parser), "["), "expected [ after class");
    for (;;) {
        ident t = next(parser);
        if (!t || ident_is(t, "]"))
            break;
        member_def mlast = null;
        array_push(cl->members, M(Parser, read_member, parser, cl, mlast));
        mlast = M(array, last, cl->members);
        for (;;) {
            if (!next_is(parser, ","))
                break;
            pop(parser);
            array_push(cl->members, M(Parser, read_member, parser, cl, mlast));
            mlast = M(array, last, cl->members);
        }
    }
    ident n = pop(parser);
    assertion(parser, ident_is(n, "]"), "expected end of class");
    drop(parser);
}

///
none class_t_resolve_members(class_t cl) {

}

/// take these args and shove them in Parser state
enum_t enum_t_with_Parser(enum_t a, Parser parser) {
    ident token_name = pop(parser);
    assertion(parser, is_alpha(token_name),
        "expected qualified name for enum, found {0}",
        token_name);
    a->name = token_name;
    assertion(parser, ident_is(pop(parser), "["), "expected [ in enum statement");
    i64  prev_value = 0;
    for (;;) {
        ident symbol = pop(parser);
        enode exp = M(Parser, parse_expression, parser); /// this will pop tokens until a valid expression is made
        if (ident_is(symbol, "]"))
            break;
        assertion(parser, is_alpha(symbol),
            "expected identifier in enum, found %s", symbol->value->chars);
        ident peek = next(parser);
        if (ident_is(peek, ":")) {
            pop(parser);
            enode enum_expr = M(Parser, parse_expression, parser);
            A enum_value = M(enode, exec, enum_expr, null);
            assertion(parser, typeid(enum_value)->traits & A_TRAIT_INTEGRAL,
                "expected integer value for enum symbol %s, found %i", symbol->value->chars, *(i32*)enum_value);
            prev_value = *(i32*)enum_value;
            assertion(parser, prev_value >= INT32_MIN && prev_value <= INT32_MAX,
                "integer out of range in enum %s for symbol %s (%i)",
                    a->name->chars, symbol->value->chars, (i32)prev_value);
        } else {
            prev_value += 1;
        }
        A f = construct(field, cstr, symbol, A_i32(prev_value));
        operator(array, assign_add, a->symbols, f);
        drop(f);

    }
}

/// parse tokens from string, referenced Parser in C++
static array parse_tokens(string input, path fname) {
    string        sp         = construct(string, cstr, "$,<>()![]/+*:\"\'#", -1); /// needs string logic in here to make a token out of the entire "string inner part" without the quotes; those will be tokens neighboring
    char          until      = 0; /// either ) for $(script) ", ', f or i
    num           len        = input->len;
    char*         origin     = input->chars;
    char*         start      = 0;
    char*         cur        = origin - 1;
    int           line_num   = 1;
    bool          new_line   = true;
    bool          token_type = false;
    bool          found_null = false;
    bool          multi_comment = false;
    array         tokens     = new(array);
    ///
    while (*(++cur)) {
        bool is_ws = false;
        if (!until) {
            if (new_line)
                new_line = false;
            /// ws does not work with new lines
            if (*cur == ' ' || *cur == '\t' || *cur == '\n' || *cur == '\r') {
                is_ws = true;
                ws(&cur);
            }
        }
        if (!*cur) break;
        bool add_str = false;
        char *rel = cur;
        if (*cur == '#') { // comment
            if (cur[1] == '#')
                multi_comment = !multi_comment;
            while (*cur && *cur != '\n')
                cur++;
            found_null = !*cur;
            new_line = true;
            until = 0; // requires further processing if not 0
        }
        if (until) {
            if (*cur == until && *(cur - 1) != '/') {
                add_str = true;
                until = 0;
                cur++;
                rel = cur;
            }
        }// else if (cur[0] == ':' && cur[1] == ':') { /// :: is a single token
        //    rel = ++cur;
        //}
        if (!until && !multi_comment) {
            char ch[2] = { cur[0], 0 };
            int type = M(string, index_of, sp, ch);
            new_line |= *cur == '\n';

            if (start && (is_ws || add_str || (token_type != (type >= 0) || token_type) || new_line)) {
                ident token = parse_token(start, (size_t)(rel - start), fname, line_num);
                M(array, push, tokens, token);
                if (!add_str) {
                    if (*cur == '$' && *(cur + 1) == '(') // shell
                        until = ')';
                    else if (*cur == '"') // double-quote
                        until = '"';
                    else if (*cur == '\'') // single-quote
                        until = '\'';
                }
                if (new_line) {
                    start = null;
                } else {
                    ws(&cur);
                    start = cur;
                    if (start[0] == ':' && start[1] == ':') /// double :: is a single token
                        cur++;
                    token_type = (type >= 0);
                }
            }
            else if (!start && !new_line) {
                start = cur;
                token_type = (type >= 0);
            }
        }
        if (new_line) {
            until = 0;
            line_num++;
            if (found_null)
                break;
        }
    }
    if (start && (cur - start)) {
        ident token = parse_token(start, cur - start, fname, line_num);
        M(array, push, tokens, token);
    }
    drop(sp);
    return tokens;
}

static Parser  Parser_with_array(Parser a, array tokens, path fname) {
    a->fname = hold(fname);
    a->tokens = hold(tokens);
    return a;
}

static ident  Parser_token_at(Parser a, num r) {
    return a->tokens->elements[a->cur + r];
}

static ident  Parser_next(Parser a) {
    return M(Parser, token_at, a, 0);
}

static ident  Parser_pop(Parser a) {
    if (a->cur < a->tokens->len)
        a->cur++;
    return a->tokens->elements[a->cur - 1];
}

static num    Parser_consume(Parser a) {
    M(Parser, pop, a);
    return a->cur;
}

static EType  Parser_expect(Parser a, ident token, array tokens) {
    return M(array, index_of, tokens, token);
}

static EType  Parser_is_alpha_ident(Parser a, ident token) {
    char t = token ? token->value->chars[0] : 0;
    return (isalpha(t) && M(array, index_of, keywords, token) == -1) ?
        EType_AlphaIdent : EType_Undefined;
}

static ident  Parser_relative(Parser a, num pos) {
    return a->tokens->elements[a->cur + pos];
}

static array assign;

static EType  Parser_is_assign(Parser a, ident token) {
    num id = M(array, index_of, assign, token);
    return (id >= 0) ? EType_Assign : EType_Undefined;
}

static enode  Parser_parse_statements(Parser parser) {
}

static enode  Parser_parse_expression(Parser parser) {
}

static enode  Parser_parse_statement(Parser parser) {
}

static i64    Parser_parse_numeric(Parser parser, ident token) {
}

static EType  Parser_is_var(Parser parser, ident) {
}

static enode  Parser_parse_add(Parser parser) {
}

static enode  Parser_parse_mult(Parser parser) {
}

static enode  Parser_parse_primary(Parser parser) {
}

array Parser_parse_raw_block(Parser parser) {
    if (!ident_is(next(parser), "["))
        return M(array, of_objects, null, pop(parser), null);
    assertion(parser, ident_is(next(parser), "["), "expected beginning of block [");
    array res = new(array);
    operator(array, assign_add, res, pop(parser));
    int level = 1;
    for (;;) {
        if (ident_is(next(parser), "[")) {
            level++;
        } else if (ident_is(next(parser), "]")) {
            level--;
        }
        operator(array, assign_add, res, pop(parser));
        if (level == 0)
            break;
    }
    return res;
}

/// parse members from a block
array Parser_parse_args(Parser parser, A object) {
    array result = new(array);
    assertion(parser, ident_is(pop(parser), "["), "expected [ for arguments");
    /// parse all symbols at level 0 (levels increased by [, decreased by ]) until ,

    /// # [ int arg, int arg2[int, string] ]
    /// # args look like this, here we have a lambda as a 2nd arg
    /// 
    while (next(parser) && !ident_is(next(parser), "]")) {
        member_def a = M(Parser, read_member, parser, object, null); /// we do not allow type-context in args but it may be ok to try in v2
        ident n = next(parser);
        assertion(parser, ident_is(n, "]") || ident_is(n, ","), ", or ] in arguments");
        if (ident_is(n, "]"))
            break;
        pop(parser);
    }
    assertion(parser, ident_is(pop(parser), "]"), "expected end of args ]");
    return result;
}

/// run this only when we have read all module entries and are ready to resolve
array Parser_read_type(Parser parser) {
    array res = new(array);
    if (ident_is(next(parser), "ref"))
        array_push(res, pop(parser));
    
    for (;;) {
        ident t_id = pop(parser);
        
        assertion(parser, is_alpha(t_id), "expected type identifier");
        array_push(res, t_id);
        bool is_bracket = ident_is(next_n(parser, 0), "[");
        bool is_array   = is_bracket && ident_is(next_n(parser, 1), "]");
        /// if its an array we dont need recursion
        if (is_array) {

        } else {
            /// could be a lambda, which requires a name to differentiate from map
        }
        if (ident_is(next(parser), "[")) {
            array_push(res, pop(parser));
            continue;
        }
        break;
    }
    return res;
};

void print_tokens(array tokens) {
    for (int i = 0; i < tokens->len; i++) {
        printf("%s\n", ((ident)tokens->elements[i])->value->chars);
    }
}

/// we need to change this a bit, i want it to add the tokens for all parsing to be done after we 'load' the other types

member_def Parser_read_member(Parser parser, A obj, member_def peer) {
    struct_t st = null;
    class_t  cl = null;
    string  this_name;
    A       obj_type = obj ? object(obj) : null;

    bool is_intern = false;
    if (next_is(parser, "intern")) {
        pop(parser);
        is_intern = true;
    }
    bool is_public = false;
    if (next_is(parser, "public")) {
        pop(parser);
        is_public = true;
    }
    bool is_static = false;
    if (next_is(parser, "static")) {
        pop(parser);
        is_static = true;
    }

    if (obj) {
        if (obj_type->type == typeof(struct_t))
            this_name = ((struct_t)obj)->name;
        else if (obj_type->type == typeof(class_t))
            this_name = ((class_t)obj)->name;
    }
    member_def result  = new(member_def);
    bool       is_ctr  = false;
    bool       is_init = false;
    bool       no_args = false;
    ident      ntop    = next(parser);
    
    /// is constructor
    if (!peer && ident_is(ntop, this_name->chars)) {
        assertion(parser, obj_type->type == typeof(class_t),
            "expected class when defining constructor");
        result->name = hold(pop(parser));
        is_ctr = true;
    /// is init method [this is where the type is]
    } else if (ident_is(ntop, "init")) {
        no_args = true;
        /// int[] [ ...code ... ]
        /// int[] method[...args...] [...code...]
        /// [read-type][check-token for method or cast]

        result->name = hold(ntop);
        result->value = M(Parser, parse_raw_block, parser);
    /// is method with args
    } else {
        if (peer) {
            result->type_tokens = peer->type_tokens;
        } else {
            no_args = true;
            /// handle init, index (subscript) operator (not yet decided on the name)
            /// our own definitions are types themselves, without knowing anything else about their membership -- unique identity by defines member
            /// 1st pass: read all members, including var data and method names (we cannot map construction yet on members)
            /// after step 1, we can register the classes
            /// 2nd pass: resolve types on those member signatures (this resolution is useful for mapping type conversion)
            /// 3rd pass: translate methods
            ///
            
            /// currently we are skipping the reading of members until later
            /// is it acceptable to read everything after consuming the module imports and reading through your own classes?
            /// 
            result->type_tokens = M(Parser, read_type, parser);

            /// the first arg must be a type, i believe
            string type_string = ident_string(ntop);
            result->name = 
            result->value = M(Parser, parse_raw_block, parser);

            /// map operator methods by setting the function pointer on the enum table


            
            printf("--------------------\n");
            print_tokens(result->type_tokens);


            if (ident_is(next(parser), "[")) {
                assertion(parser, result->type_tokens->len == 1, "name identifier expected for automatic return type method");
                result->name = hold(result->type_tokens->elements[0]);
                result->type_tokens->elements[0] = construct(ident, cstr, cs(this_name), null, 0);
            }
        }

        if (!result->name) {
            assertion(parser, is_alpha(next(parser)),
                "%s:%d: expected identifier for member, found %s", 
                next(parser)->fname->chars, next(parser)->line_num, next(parser)->value->chars);
            result->name = pop(parser);
        }
    }
    ident n = next(parser);
    assertion(parser, (ident_is(n, "[") && is_ctr) || !is_ctr, "invalid syntax for constructor; expected [args]");
    if (ident_is(n, "[")) {
        // [args] this is a lambda or method
        if (!no_args)
            result->args = M(Parser, parse_args, parser, obj);
        //var_bind args_vars;
        //for (member_def& member: result->args) {
        //    args_vars->vtypes += member->member_type;
        //    args_vars->vnames += member->name;
        //}
        int line_num_def = n->line_num;
        if (is_ctr) {
            if (ident_is(next(parser), ":")) {
                pop(parser);
                ident class_name = pop(parser);
                assertion(parser,
                    ident_is(class_name, cl->from->chars) ||
                    ident_is(class_name, this_name->chars), "invalid constructor base call");
                result->base_class = class_name; /// should be assertion checked above
                result->base_forward = M(Parser, parse_raw_block, parser);
            }
            result->member_type = MemberType_Constructor;
            ident n = next(parser);
            if (ident_is(n, "[")) {
                result->value = M(Parser, parse_raw_block, parser);
            } else {
                array_push(result, construct(ident, cstr, "[", null, 0)); /// with ctrs of name, the #2 and on args can be optional.  this is a decent standard.  we would select the first defined to match
                assert(obj);
                assert((obj_type->type == typeof(class_t)));
                class_t cl = ((class_t)obj);
                
                each(array, result->args, member_def, arg) {
                    bool found = false;
                    each(array, cl->members, member_def, m) {
                        if (m->name == arg->name) {
                            found = true;
                            break;
                        }
                    }
                    assertion(parser, found, "arg cannot be found in membership");
                    operator(array, assign_add, result->value, construct(ident, cstr, cs(arg->name), null, 0));
                    operator(array, assign_add, result->value, construct(ident, cstr, ":", null, 0));
                    array_push(result->value, construct(ident, cstr, arg->name->chars, null, 0));
                }
                array_push(result, construct(ident, cstr, "]", null, 0));
            }
            /// the automatic constructor we'll for-each for the args
        } else {
            ident next_token = next(parser);
            if (!ident_is(next_token, "return") && (ident_is(next_token, ":") || !ident_is(next_token, "["))) {
                result->member_type = MemberType_Lambda;
                if (ident_is(next_token, ":"))
                    pop(parser); /// lambda is being assigned, we need to set a state var
            } else {
                result->member_type = MemberType_Method;
            }
            ident n = next(parser);
            if (result->member_type == MemberType_Method || ident_is(n, "[")) {
                // needs to be able to handle trivial methods if we want this supported
                if (ident_is(n, "return")) {
                    assertion(parser, n->line_num == line_num_def, "single line return must be on the same line as the method definition");
                    for (;;) {
                        if (n->line_num == next(parser)->line_num) {
                            array_push(result->value, pop(parser));
                        } else 
                            break;
                    }
                } else {
                    assertion(parser, ident_is(n, "["), "expected [method code block], found %s", n->value->chars);
                    result->value = M(Parser, parse_raw_block, parser);
                }
            }
        }
    } else if (ident_is(n, ":")) {
        pop(parser);
        result->value = M(Parser, parse_raw_block, parser);
    } else {
        // not assigning variable
    }

    result->intern    = is_intern;
    result->is_static = is_static;
    result->is_public = is_public;
    return result;
}


#define token_cstr(T)   T->value->chars
#define token_cmp(T,S)  strcmp(T->value->chars, S)

void push_define(module_t m, Parser parser, ident keyword, define_t mm) {
    for (num i = 0; i < m->defines->len; i++) {
        define_t def = m->defines->elements[i];
        if (strcmp(mm->name->chars, def->name->chars) == 0) {
            assertion(parser, false, "duplicate identifier for %s: %s",
                keyword->value->chars, mm->name->chars);
        }
    }
    A obj = object(mm);
    M(array, push, m->defines, hold(mm)); /// anything 'new' gets a base ref count, so anything pushed should be held if not new
    mm->module = (module_t)hold(m);
    if (strcmp(mm->name->chars, "app") == 0)
        m->app = (class_t)hold(mm);
};

module_t module_t_with_path(module_t m, path fname) {
    string contents = M(path, read, fname, typeof(string));
    m->module_name = hold(fname);
    m->tokens = parse_tokens(contents, fname);
    m->defines = new(array);
    drop(contents);
    Parser parser = construct(Parser, array, m->tokens, fname);
    int   imports  = 0;
    int   includes = 0;
    bool  inlay    = false;
    ///
    for (;;) {
        ident token = M(Parser, next, parser);
        if (!token)
            break;
        if (token_cmp(token, "intern") == 0) {
            M(Parser, consume, parser);
            token  = M(Parser, pop, parser);
            parser->membership = EMembership_internal;
        } else {
            parser->membership = EMembership_normal;
        }
        drop(parser->keyword);
        parser->keyword = ident_string(token);
        if (token_cmp(token, "import") == 0) {
            assertion(parser, parser->membership == EMembership_normal, 
                "intern keyword not applicable to import");
            parser->membership = EMembership_normal;
            parser->keyword = null;
            import_t import = construct(import_t, Parser, parser);
            imports++;
            push_define(m, parser, token, import);
            M(array, push, m->imports, import);

            /// load silver module if name is specified without source
            if (import->name && !import->source) {
                string  loc = construct(string, cstr, "{1}{0}.si", -1);
                array attempt = new(array);
                M(array, push_symbols, attempt, "", "spec/", null);
                bool exists = false;
                for (int ia = 0; ia < attempt->len; ia++) {
                    string pre = attempt->elements[ia];
                    char buf[1024];
                    sprintf(buf, "%s%s.si", pre->chars, import->name->chars);
                    path si_path = construct(path, cstr, buf);

                    //console.log("si_path = {0}", { si_path });
                    if (!M(path, exists, si_path))
                        continue;
                    import->module_path = si_path;
                    printf("module %s", si_path->chars);
                    import->module = construct(module_t, path, si_path);
                    exists = true;
                    break;
                }
                assertion(parser, exists, "path does not exist for silver module: %s", import->name->chars);
            }
        } else if (ident_is(token, "enum")) {
            assert(false);
        } else if (ident_is(token, "class")) {
            class_t cl = construct(class_t, Parser, parser);
            push_define(m, parser, token, cl);
            drop(cl);
        } else if (ident_is(token, "proto")) {
            assert(false);
        } else if (ident_is(token, "struct")) {
            assert(false);
        } else {
            assert(false);
        }
    }

    /// step 1. read members in all definitions in this module
    /// this means their name and type which we can be aware of, but not how to initialize it if its in our own module
    each(array, m->defines, define_t, def) {
        AType type = typeid(def);
        M(define_t, read_members, def); 
    }

    /// step 2. resolve members to their 
    each(array, m->defines, define_t, def) {
        AType type = typeid(def);
        /// set identity on types specified from R-Type and any args
        M(define_t, resolve_members, def);
    }

    return m;
}

/// call this within the enode translation; all referenced types must be forged
/// resolving type involves designing, so we may choose to forge a type that wont forge at all.  thats just a null
static silver_t forge_type(module_t module, array type_tokens) {
    /// need array or map feature on this type, 
    /// which must reference a type thats found
    
    return null;
}

module_t module_t_find_module(module_t a, string name) {
    each(array, a->imports, import_t, e) {
        if (e->name == name)
            return e->module;
    }
    return null;
}

/// find_implement is going to be called with a as the source parser module
/// objects can be nullable, thats default for array but not for members
A module_t_find_implement(module_t a, ident iname) {
    module_t m       = a;
    array   sp      = M(string, split, iname->value, ".");
    int     n_len   = sp->len;
    string  ns      = n_len ? sp->elements[0] : null;
    string  name    = sp->elements[(n_len > 1) ? 1 : 0];
    each(array, a->imports, import_t, e) {
        if (!ns || e->isolate_namespace == ns && e->module)
            each(array, e->module->defines, define_t, mm) {
                if (mm->name == name)
                    return mm;
            }
    }
    each(array, a->defines, define_t, mm) {
        if (mm->name == name)
            return mm;
    }
    return null;
}

A module_t_find_class(module_t a, ident name) {
    A impl = M(module_t, find_implement, a, name);
    if (typeid(impl) == typeof(class_t))
        return (class_t)impl;
    return null;
}

A module_t_find_struct(module_t a, ident name) {
    A impl = M(module_t, find_implement, a, name);
    if (typeid(impl) == typeof(struct_t))
        return (struct_t)impl;
    return null;
}

void module_t_graph(module_t m) {
}

void module_t_c99(module_t m) {
    int test = 0;
    test++;
}

void module_t_run(module_t m) {
}

define_class(Parser)
define_class(ident)
define_class(enode)
define_class(define_t)
define_class(module_t)
define_class(member_def)

define_mod(class_t,  define_t)
define_mod(enum_t,   define_t)
define_mod(import_t, define_t)
define_mod(struct_t, define_t)
define_mod(var_t,    define_t)

declare_alias(array, array_i32)
 define_alias(array, array_i32, i32)

int main(int argc, char **argv) {
    A_finish_types();

    array_i32 ints = new(array_i32);

    keywords = new(array);
    M(array, push_symbols, keywords, 
        "class",  "proto",  "struct",
        "import", "return", "asm", "if",
        "switch", "while",  "for", "do", null);

    assign = new(array);
    M(array, push_symbols, assign, 
        ":", "+=", "-=", "*=", "/=", "|=",
        "&=", "^=", ">>=", "<<=", "%=", null);

    chdir("spec");
    path module_path = construct(path, cstr, "basic.si"); 
    module_t m = construct(module_t, path, module_path);
    M(module_t, graph, m);
    M(module_t, c99,   m);
    return 0;
}

