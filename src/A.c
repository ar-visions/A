#include <A>
#include <ffi.h>
#include <sys/stat.h>
#include <dirent.h>

__thread array     af_stack;
__thread   AF      af_top;

static global_init_fn* call_after;
static num             call_after_alloc;
static num             call_after_count;

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

static global_init_fn* call_last;
static num             call_last_alloc;
static num             call_last_count;

void A_module_init(bool(*fn)()) {
    /// these should be loaded after the types are loaded.. the module inits are used for setting module-members (not globals!)
    if (call_last_count == call_last_alloc) {
        global_init_fn* prev      = call_last;
        num            alloc_prev = call_last_alloc;
        call_last_alloc           = 32 + (call_last_alloc << 1);
        call_last                 = calloc(call_last_alloc, sizeof(global_init_fn));
        if (prev) {
            memcpy(call_last, prev, sizeof(global_init_fn) * alloc_prev);
            free(prev);
        }
    }
    call_last[call_last_count++] = fn;
}

static A_f**           types;
static num             types_alloc;
static num             types_len;

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

/// verify type is compatible with newly created object given from user
/// if a null is given by user, we do nothing
A A_verify(A instance, AType type) {
    if (!instance) return null;
    AType itype = isa(instance);
    do {
        if (itype == type)
            return instance;
        type = type->parent_type;
    } while (type && type != typeid(A));
    return null;
}

A_f** A_types(num* length) {
    *length = types_len;
    return types;
}

A A_new(AType type) {
    A res = A_alloc(type, 1, true);
    // todo: do not call init in A_alloc
    return res;
}

A A_initialize(A a) {
    A f           = A_fields(a);
    AType a_type  = &A_type;
    AType current = f->type;
    raw last_init = null;
    while (current) {
        if ((raw)current->init != last_init) {
            current->init(a);
            last_init = current->init;
        }
        if (current == a_type)
            break;
        current = current->parent_type;
    }
    return a;
}

A A_alloc(AType type, num count, bool af_pool) {
    A a           = calloc(1, sizeof(struct A) + type->size * count);
    a->refs       = af_pool ? 0 : 1;
    a->type       = type;
    a->origin     = a;
    a->data       = &a[1];
    a->count      = count;
    a->alloc      = count;
    if (af_pool) {
        a->ar_index = af_top->pool->len;
        M(af_top->pool, push, a->data);
    } else {
        a->ar_index = 0; // Indicate that this object is not in the auto-free pool
    }
    return a->data;
}


A A_realloc(A a, num alloc) {
    A obj = A_fields(a);
    assert(obj->type->traits == A_TRAIT_PRIMITIVE, "realloc must be called on a primitive type");
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
    A   obj = A_fields(a);
    assert(obj->type->traits == A_TRAIT_PRIMITIVE, "must be called on a primitive type");
    num sz  = obj->type->size;
    if (obj->count == obj->alloc)
        A_realloc(a, 32 + (obj->alloc << 1));
    memcpy(&((u8*)obj->data)[obj->count++ * sz], value, sz);
}

#define iarray(I,M,...) array_ ## M(I, M, __VA_ARGS__)

/// array -------------------------
static void array_alloc_sz(array a, sz alloc) {
    A* elements = (A*)calloc(alloc, sizeof(struct A*));
    memcpy(elements, a->elements, sizeof(struct A*) * a->len);
    free(a->elements);
    a->elements = elements;
    a->alloc = alloc;
}

static void array_expand(array a) {
    num alloc = 32 + (a->alloc << 1);
    array_alloc_sz(a, alloc);
}

static A array_push_weak(array a, A b) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    AType t = isa(a);
    if (is_meta(a))
        assert(is_meta_compatible(a, b), "not meta compatible");
    a->elements[a->len++] = b;
    return b;
}

method_t* method_with_address(handle address, AType rtype, array atypes, AType method_owner) {
    const num max_args = 8;
    method_t* method = calloc(1, sizeof(method_t));
    method->ffi_cif  = calloc(1,        sizeof(ffi_cif));
    method->ffi_args = calloc(max_args, sizeof(ffi_type*));
    method->atypes   = allocate(array);
    method->rtype    = rtype;
    ffi_type **ffi_args = (ffi_type**)method->ffi_args;
    for (num i = 0; i < atypes->len; i++) {
        A_f* a_type   = atypes->elements[i];
        bool is_prim  = a_type->traits & A_TRAIT_PRIMITIVE;
        ffi_args[i]   = is_prim ? a_type->arb : &ffi_type_pointer;
        M(method->atypes, push_weak, a_type);
    }
    ffi_status status = ffi_prep_cif(
        (ffi_cif*) method->ffi_cif, FFI_DEFAULT_ABI, atypes->len,
        (ffi_type*)(rtype->traits & A_TRAIT_ABSTRACT) ? method_owner->arb : rtype->arb, ffi_args);
    assert(status == FFI_OK, "status == %i", (i32)status);
    return method;
}

/// should work on statics or member functions the same; its up to the caller to push the self in there
fn A_lambda(A target, Member member, A context) {
    fn f = new(fn,
        method,  member->method,
        target,  target,
        context, context);
    return f;
}

A A_method_call(method_t* a, array args) {
    const num max_args = 8;
    void* arg_values[max_args];
    assert(args->len == a->atypes->len, "arg count mismatch");
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
    assert(mem->method, "method not set");
    method_t* m = mem->method;
    A res = A_method_call(m, args);
    return res;
}

A A_convert(AType type, A input) {
    if (type == isa(input)) return input;
    assert(false, "not implemented");
    return input;
}

A A_method_vargs(A instance, cstr method_name, int n_args, ...) {
    AType type = isa(instance);
    type_member_t* mem = A_member(type, A_TYPE_IMETHOD | A_TYPE_SMETHOD, method_name);
    assert(mem->method, "method not set");
    method_t* m = mem->method;
    va_list  vargs;
    va_start(vargs, n_args);
    array args = new(array, alloc, n_args + 1);
    M(args, push, instance);
    for (int i = 0; i < n_args; i++) {
        A arg = va_arg(vargs, A);
        M(args, push, arg);
    }
    va_end(vargs);
    A res = A_method_call(m, args);
    return res;
}


void A_start() {
    int remaining = call_after_count;
    while (remaining)
        for (int i = 0; i < call_after_count; i++) {
            global_init_fn fn = call_after[i];
            if (fn && fn()) {
                call_after[i] = null;
                remaining--;
            }
        }

    remaining = call_last_count;
    while (remaining)
        for (int i = 0; i < call_last_count; i++) {
            global_init_fn fn = call_last[i];
            if (fn && fn()) {
                call_last[i] = null;
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
            if (mem->member_type & (A_TYPE_IMETHOD | A_TYPE_SMETHOD)) {
                void* address = 0;
                memcpy(&address, &((u8*)type)[mem->offset], sizeof(void*));
                assert(address, "no address");
                array args = allocate(array, alloc, mem->args.count);
                for (num i = 0; i < mem->args.count; i++)
                    args->elements[i] = ((A_f**)&mem->args.meta_0)[i];
                args->len = mem->args.count;
                mem->method = method_with_address(address, mem->type, args, type);
            }
        }
    }
}

type_member_t* A_member(A_f* type, enum A_TYPE member_type, char* name) {
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        if (mem->member_type & member_type && strcmp(mem->name, name) == 0)
            return mem;
    }
    return 0;
}

type_member_t* A_hold_members(A instance) {
    AType type = isa(instance);
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        A   *mdata = (A*)((cstr)instance + mem->offset);
        if (*mdata && mem->member_type & (A_TYPE_PROP | A_TYPE_PRIV | A_TYPE_INTERN))
            if (!(mem->type->traits & A_TRAIT_PRIMITIVE))
                A_hold(*mdata);
    }
    return 0;
}

A A_set_property(A instance, symbol name, A value) {
    AType type = isa(instance);
    type_member_t* m = A_member(type, (A_TYPE_PROP | A_TYPE_PRIV | A_TYPE_INTERN), name);
    assert(m, "%s not found on object %s", name, type->name);
    A   *mdata = (A*)((cstr)instance + m->offset);
    A prev = *mdata;
    *mdata = A_hold(value);
    A_drop(prev);
    return value;
}


/// should be adapted to work with schemas 
/// what a weird thing it would be to have map access to properties
/// everything should be A-based, and forget about the argument hacks?
map A_args(int argc, symbol argv[], map default_values, object default_key) {
    map result = new(map, hsize, 16);
    for (item ii = default_values->first; ii; ii = ii->next) {
        pair  hm = ii->value;
        object k = hm->key;
        object v = hm->value;
        M(result, set, k, v);
    }
    int    i = 1;
    bool found_single = false;
    while (i < argc) {
        symbol arg = argv[i];
        if (!arg) {
            i++;
            continue;
        }
        if (arg[0] == '-') {
            // -s or --silver
            bool doub  = arg[1] == '-';
            string s_key = new(string, chars, (cstr)&arg[doub + 1]);
            string s_val = new(string, chars, (cstr)&arg[doub + 1 + 1]);

            for (item f = default_values->first; f; f = f->next) {
                /// import A types from runtime
                pair      mi = (pair) f->value;
                object def_value = mi->value;
                AType   def_type = def_value ? isa(def_value) : typeid(string);
                assert(f->key == mi->key, "keys do not match"); /// make sure we copy it over from refs
                if ((!doub && strncmp(((string)f->key)->chars, s_key->chars, 1) == 0) ||
                    ( doub && M(f->key, compare, s_key) == 0)) {
                    /// inter-op with object-based A-type sells it.
                    /// its also a guide to use the same schema
                    object value = A_formatter(def_type, null, false, "%o", s_val);
                    assert(isa(value) == def_type, "");
                    M(result, set, f->key, value);
                }
            }
        } else if (!found_single && default_key) {
            A default_key_obj = A_fields(default_key);
            string s_val     = new(string, chars, (cstr)arg);
            object def_value = M(default_values, get, default_key);
            AType  def_type  = isa(def_value);
            object value     = A_formatter(def_type, null, false, "%o", s_val);
            M(result, set, default_key, value);
            found_single = true;
        }
        i++;
    }
    return result;
}

/// can we have i32_with_i16(i32 a, i16 b)
/// primitives are based on A alone
/// i wonder if we can add more constructors or even methods to the prims

A A_primitive(A_f* type, void* data) {
    assert(type->traits & A_TRAIT_PRIMITIVE, "must be primitive");
    A copy = A_alloc(type, type->size, true);
    memcpy(copy, data, type->size);
    return copy;
}

A A_enum(A_f* type, i32 data) {
    assert(type->traits & A_TRAIT_ENUM, "must be enum");
    assert(type->size == sizeof(i32), "enum size invalid");
    A copy = A_alloc(type, type->size, true);
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
A A_none()          { return A_primitive(&none_type, NULL); }
A A_bool(bool data) { return A_primitive(&bool_type, &data); }

/// A -------------------------
static A   A_new_default(AType type, num count) {
    return A_alloc(type, count, true);
}

//static A A_with_cereal(A a, cereal cs) {
//    fault("not implemented");
//    return a;
//}

static void A_init(A a) { }
static void A_destructor(A a) {
    // go through objects type fields/offsets; 
    // when type is A-based, we release; 
    // this part is 'auto-release', our pool is an AF pool, or auto-free.
    // when new() is made, the reference goes into a pool
    // 
    AType type = isa(a);
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* m = &type->members[i];
        if ((m->member_type == A_TYPE_PROP || m->member_type == A_TYPE_PRIV) && !(m->type->traits & A_TRAIT_PRIMITIVE)) {
            u8* ptr = (u8*)a + m->offset;
            A   ref = ptr;
            A_drop(ref);
            *((A*)&ptr) = null;
        }
    }
}
static u64  A_hash      (A a) { return (u64)(size_t)a; }
static bool A_cast_bool (A a) { return (bool)(size_t)a; }

static A A_with_cereal(A a, cereal cs) {
    sz len = strlen(cs);
    A        f = A_fields(a);
    AType type = f->type;
    if      (type == typeid(f64)) sscanf(cs, "%lf",  (f64*)a);
    else if (type == typeid(f32)) sscanf(cs, "%f",   (f32*)a);
    else if (type == typeid(i32)) sscanf(cs, "%i",   (i32*)a);
    else if (type == typeid(u32)) sscanf(cs, "%u",   (u32*)a);
    else if (type == typeid(i64)) sscanf(cs, "%lli", (i64*)a);
    else if (type == typeid(u64)) sscanf(cs, "%llu", (u64*)a);
    else if (type == typeid(string)) {
        string  res = a;
        sz     a_ln = len > -1 ? len : strlen(cs);
        res->chars  = calloc(a_ln + 1, 1);
        res->len    = a_ln;
        memcpy(res->chars, cs, a_ln);
        return res;
    }
    else {
        printf("implement ctr cstr for %s\n", f->type->name);
        exit(-1);
    }
    return a;
}

static void A_serialize(AType type, string res, A a) {
    //AType type = isa(a);
    if (type->traits & A_TRAIT_PRIMITIVE) {
        char buf[128];
        int len = 0;
        if      (type == typeid(i64)) len = sprintf(buf, "%lld", *(i64*)a);
        else if (type == typeid(num)) len = sprintf(buf, "%lld", *(i64*)a);
        else if (type == typeid(i32)) len = sprintf(buf, "%d",   *(i32*)a);
        else if (type == typeid(i16)) len = sprintf(buf, "%hd",  *(i16*)a);
        else if (type == typeid(i8))  len = sprintf(buf, "%hhd", *(i8*) a);
        else if (type == typeid(u64)) len = sprintf(buf, "%llu", *(u64*)a);
        else if (type == typeid(u32)) len = sprintf(buf, "%u",   *(u32*)a);
        else if (type == typeid(u16)) len = sprintf(buf, "%hu",  *(u16*)a);
        else if (type == typeid(u8))  len = sprintf(buf, "%hhu", *(u8*) a);
        else if (type == typeid(f64)) len = sprintf(buf, "%f",   *(f64*)a);
        else if (type == typeid(f32)) len = sprintf(buf, "%f",   *(f32*)a);
        else {
            fault("implement primitive cast to str: %s", type->name);
        }
        M(res, append, buf); // should allow for a -1 or len
    } else {
        string s = cast(a, string);
        if (s) {
            M(res, append, "\"");
            M(res, append, s->chars);
            M(res, append, "\"");
        } else
            M(res, append, "null");
    }
}

static string A_cast_string(A a) {
    AType type = isa(a);
    if (type == typeid(string))
        return a;
    bool  once = false;
    string res = new(string, alloc, 1024);
    if (type->traits & A_TRAIT_PRIMITIVE)
        A_serialize(type, res, a);
    else {
        M(res, append, type->name);
        M(res, append, "[");
        for (num i = 0; i < type->member_count; i++) {
            type_member_t* m = &type->members[i];
            // todo: intern members wont be registered
            if (m->member_type & (A_TYPE_PROP | A_TYPE_PRIV | A_TYPE_INTERN)) {
                if (once)
                    M(res, append, " ");
                u8* ptr = (u8*)a + m->offset;
                M(res, append, m->name);
                M(res, append, ":");
                A_serialize(m->type, res, ptr);
                once = true;
            }
        }
        M(res, append, "]");
    }
    return res;
}

#define set_v() \
    AType type = isa(a); \
    if (type == typeid(i8))  *(i8*) a = (i8) v; \
    if (type == typeid(i16)) *(i16*)a = (i16)v; \
    if (type == typeid(i32)) *(i32*)a = (i32)v; \
    if (type == typeid(i64)) *(i64*)a = (i64)v; \
    if (type == typeid(u8))  *(u8*) a = (u8) v; \
    if (type == typeid(u16)) *(u16*)a = (u16)v; \
    if (type == typeid(u32)) *(u32*)a = (u32)v; \
    if (type == typeid(u64)) *(u64*)a = (u64)v; \
    if (type == typeid(f32)) *(f32*)a = (f32)v; \
    if (type == typeid(f64)) *(f64*)a = (f64)v; \
    return a;

static A numeric_with_i8 (A a, i8   v) { set_v(); }
static A numeric_with_i16(A a, i16  v) { set_v(); }
static A numeric_with_i32(A a, i32  v) { set_v(); }
static A numeric_with_i64(A a, i64  v) {
    AType type = isa(a);
    if (type == typeid(i8))  *(i8*) a = (i8) v;
    if (type == typeid(i16)) *(i16*)a = (i16)v;
    if (type == typeid(i32)) *(i32*)a = (i32)v; \
    if (type == typeid(i64)) *(i64*)a = (i64)v; \
    if (type == typeid(u8))  *(u8*) a = (u8) v; \
    if (type == typeid(u16)) *(u16*)a = (u16)v; \
    if (type == typeid(u32)) *(u32*)a = (u32)v; \
    if (type == typeid(u64)) *(u64*)a = (u64)v; \
    if (type == typeid(f32)) *(f32*)a = (f32)v; \
    if (type == typeid(f64)) *(f64*)a = (f64)v; \
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

sz len(A a) {
    AType t = isa(a);
    if (t == typeid(string)) return ((string)a)->len;
    if (t == typeid(array))  return ((array) a)->len;
    if (t == typeid(cstr) || t == typeid(symbol) || t == typeid(cereal))
        return strlen(a);
    fault("len not handled for type %s", t->name);
    return 0;
}

num index_of_cstr(A a, cstr f) {
    AType t = isa(a);
    if (t == typeid(string)) return M((string)a, index_of, f);
    if (t == typeid(array))  return M((array)a,  index_of, str(f));
    if (t == typeid(cstr) || t == typeid(symbol) || t == typeid(cereal)) {
        cstr v = strstr(a, f);
        return v ? (num)(v - f) : (num)-1;
    }
    fault("len not handled for type %s", t->name);
    return 0;
}

Exists A_exists(A o) {
    AType t = isa(o);
    path  f = null;
    if (t == typeid(string))
        f = cast((string)t, path);
    else if (t == typeid(path))
        f = o;
    assert(f, "type not supported");
    bool is_dir = M(f, is_dir);
    bool r = M(f, exists);
    if (is_dir)
        return r ? Exists_dir  : Exists_no;
    else
        return r ? Exists_file : Exists_no;
}

/// these pointers are invalid for A since they are in who-knows land, but the differences would be the same
static i32 A_compare(A a, A b) {
    return (i32)((sz)(void*)a - (sz)(void*)b);
}

num parse_formatter(cstr start, cstr res, num sz) {
    cstr scan = start;
    num index = 0;
    if (*scan == '%') {
        if (index < sz - 1)
            res[index++] = *scan++;
        while (*scan) {
            if (strchr("diouxXeEfFgGaAcspn%", *scan)) {
                if (index < sz - 1) res[index++] = *scan++;
                break;  // end of the format specifier
            } else if (strchr("0123456789.-+lhjztL*", *scan)) {
                if (index < sz - 1) res[index++] = *scan++;
            } else
                break;
        }
    }
    res[index] = 0;
    return (num)(scan - start);
}

/// does not seem possible to proxy args around in C99, so we are wrapping this in a macro
object A_formatter(AType type, FILE* f, bool write_ln, cstr template, ...) {
    va_list args;
    va_start(args, template);
    string  res  = new(string, alloc, 1024);
    cstr    scan = template;

    while (*scan) {
        /// format %o as object's string cast
        if (*scan == '%' && *(scan + 1) == 'o') {
            A      arg = va_arg(args, A);
            string   a = cast(arg, string);
            num    len = a->len;
            M(res, reserve, len);
            memcpy(&res->chars[res->len], a->chars, len);
            res->len += len;
            scan     += 2; // Skip over %o
        } else {
            /// format with vsnprintf
            const char* next_percent = strchr(scan, '%');
            num segment_len = next_percent ? (num)(next_percent - scan) : (num)strlen(scan);
            M(res, reserve, segment_len);
            memcpy(&res->chars[res->len], scan, segment_len);
            res->len += segment_len;
            scan     += segment_len;
            if (*scan == '%') {
                if (*(scan + 1) == 'o')
                    continue;
                char formatter[128];
                int symbol_len = parse_formatter(scan, formatter, 128);
                for (;;) {
                    num f_len = 0;
                    num avail = res->alloc - res->len;
                    cstr  end = &res->chars[res->len];
                    if (strchr("fFgG", formatter[symbol_len - 1]))
                        f_len = snprintf(end, avail, formatter, va_arg(args, double));
                    else if (strchr("diouxX", formatter[symbol_len - 1]))
                        f_len = snprintf(end, avail, formatter, va_arg(args, int));
                    else if (strchr("c", formatter[symbol_len - 1]))
                        f_len = snprintf(end, avail, formatter, va_arg(args, int));
                    else
                        f_len = snprintf(
                            end, avail, formatter, va_arg(args, void*));
                    if (f_len > avail) {
                        M(res, reserve, res->alloc << 1);
                        continue;
                    }
                    res->len += f_len;
                    break;
                }
                scan += symbol_len;
            }
        }
    }
    va_end(args);
    if (f) {
        M(res, write, f, false);
        if (write_ln) {
            fwrite("\n", 1, 1, f);
            fflush(f);
        }
    }
    /// cereal is pretty available, and we only need this on string and path
    return type ? (A)type->with_cereal(A_alloc(type, 1, true), res->chars) : (A)res;
}

static void  string_destructor(string a)        { free(a->chars); }
static num   string_compare(string a, string b) { return strcmp(a->chars, b->chars); }
static num   string_cmp(string a, cstr b)       { return strcmp(a->chars, b); }
static bool  string_eq(string a, cstr b)        { return strcmp(a->chars, b) == 0; }

static i32   string_index_num(string a, num index) {
    if (index < 0)
        index += a->len;
    if (index >= a->len)
        return 0;
    return (i32)a->chars[index];
}

static array string_split(string a, cstr sp) {
    cstr next = a->chars;
    sz   slen = strlen(sp);
    while (next) {
        cstr   n = strstr(&next[1], sp);
        string v = new(string, chars, next, ref_length, n ? (sz)(1 + next - n) : 0);
        next = n ? n + slen : null;
    }
    return null;
}

static void string_alloc_sz(string a, sz alloc) {
    char* chars = calloc(1 + alloc, sizeof(char));
    memcpy(chars, a->chars, sizeof(char) * a->len);
    chars[a->len] = 0;
    free(a->chars);
    a->chars = chars;
    a->alloc = alloc;
}

static string string_mid(string a, num start, num len) {
    return new(string, chars, &a->chars[start], ref_length, len);
}

static none  string_reserve(string a, num extra) {
    if (a->alloc - a->len >= extra)
        return;
    string_alloc_sz(a, a->alloc + extra);
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

sz string_cast_sz(string a) {
    return a->len;
}

cstr string_cast_cstr(string a) {
    return a->chars;
}

static none string_write(string a, handle f, bool new_line) {
    FILE* output = f ? f : stdout;
    fwrite(a->chars, a->len, 1, output);
    if (new_line) fwrite("\n", 1, 1, output);
    fflush(output);
}

static path string_cast_path(string a) {
    return new(path, chars, a->chars);
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
    return M(f->key ? f->key : f->value, hash);
}

static u64 string_hash(string a) {
    if (a->h) return a->h;
    a->h = fnv1a_hash(a->chars, a->len, OFFSET_BASIS);
    return a->h;
}

static void string_init(string a) {
    cstr value = a->chars;
    if (a->alloc)
        a->chars = (char*)calloc(1, 1 + a->alloc);
    if (value) {
        sz len = a->ref_length ? a->ref_length : strlen(value);
        if (!a->alloc)
            a->alloc = len;
        if (a->chars == value)
            a->chars = (char*)calloc(1, len + 1);
        memcpy(a->chars, value, len);
        a->chars[len] = 0;
        a->len = len;
    }
}

static string string_with_cstr(string a, cstr value, num len) {
    if (len == -1) len = (value ? strlen(value) : 0);

    return a;
}

static bool string_has_suffix(string a, cstr value) {
    sz ln = strlen(value);
    if (!ln || ln > a->len) return false;
    return strcmp(&a->chars[a->len - ln], value) == 0;
}

static item hashmap_fetch(hashmap a, A key) {
    u64 h = M(key, hash);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (M(f->key, compare, key) == 0)
            return f;
    item n = list_type.push(bucket, null);
    n->key = A_hold(key);
    a->count++;
    return n;
}

static item hashmap_lookup(hashmap a, A key) {
    u64 h = M(key, hash);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (M(f->key, compare, key) == 0)
            return f;
    return null;
}

static none hashmap_set(hashmap a, A key, A value) {
    item i = M(a, fetch, key);
    A prev = i->value;
    i->value = A_hold(value);
    A_drop(prev);
}

static A hashmap_get(hashmap a, A key) {
    item i = M(a, lookup, key);
    return i ? i->value : null;
}

static bool hashmap_contains(hashmap a, A key) {
    item i = M(a, lookup, key);
    return i != null;
}

static none hashmap_remove(hashmap a, A key) {
    u64 h = M(key, hash);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (M(f->key, compare, key) == 0) {
            list_type.remove_item(bucket, f);
            a->count--;
            break;
        }
}

static bool hashmap_cast_bool(hashmap a) {
    return a->count > 0;
}

static A hashmap_index_A(hashmap a, A key) {
    return M(a, get, key);
}

static hashmap hashmap_init(hashmap a) {
    if (!a->alloc)
         a->alloc = 16;
    a->data  = (list)calloc(a->alloc, sizeof(struct list)); /// we can zero-init a vectorized set of objects with A-type
    return a;
}

static string hashmap_cast_string(hashmap a) {
    string res  = new(string, alloc, 1024);
    bool   once = false;
    for (int i = 0; i < a->alloc; i++) {
        list bucket = &a->data[i];
        for (item f = bucket->first; f; f = f->next) {
            string key = cast(f->key, string);
            string val = cast(f->value, string);
            if (once) M(res, append, " ");
            M(res, append, key->chars);
            M(res, append, ":");
            M(res, append, val->chars);
            once = true;
        }
    }
    return res;
}


static item map_fetch(map a, A key) {
    item i = M(a->hmap, fetch, key);
    return i;
}

static none map_set(map a, A key, A value) {
    item i  = M(a->hmap, fetch, key);
    pair mi = i->value;
    if (mi) {
        A before     = mi->value;
        mi->value    = A_hold(value);
        A_drop(before);
    } else {
        mi = i->value = new(pair, key, key, value, value); // todo: make pair originate in hash; remove the key from item
        mi->ref      = M(a, push, i);
        mi->ref->key = A_hold(key);
        mi->ref->value = mi;
    }
}

static A map_get(map a, A key) {
    item i = M(a->hmap, lookup, key);
    return i->value ? ((pair)i->value)->value : null;
}

static bool map_contains(map a, A key) {
    item i = M(a->hmap, lookup, key);
    return i != null;
}

/// must store a ref to the ref item in hashmap item's value; we may call this an object map_value
static none map_remove(map a, A key) {
    item i = M(a->hmap, lookup, key);
    if (i) {
        item ref = i->value;
        M(a, remove_item, ref);
    }
}

static bool map_cast_bool(map a) {
    return a->count > 0;
}

static A map_index_A(map a, A key) {
    item hash_item = M(a->hmap, get, key);
    return hash_item ? hash_item->value : null;
}

static map map_with_sz(map a, sz size) {
    a->hsize = size;
    return a;
}

static void map_init(map a) {
    if (!a->hsize) a->hsize = 1024;
    a->hmap  = new(hashmap, alloc, a->hsize);
}

static string map_cast_string(map a) {
    string res  = new(string, alloc, 1024);
    bool   once = false;
    for (item i = a->first; i; i = i->next) {
        pair   kv    = i->value;
        string key   = cast(kv->key,   string);
        string value = cast(kv->value, string);
        if (once) M(res, append, " ");
        M(res, append, key->chars);
        M(res, append, ":");
        M(res, append, value->chars);
        once = true;
    }
    return res;
}

/// copy member by member; bit copy the primitives
A A_copy(A a) {
    A f = A_fields(a);
    assert(f->count > 0, "invalid count");
    AType type = isa(a);
    A b = A_alloc(type, f->count, true);
    for (num i = 0; i < type->member_count; i++) { /// test this, its been a while -- intern cannot be copied
        type_member_t* mem = &type->members[i];
        if (mem->member_type) {
            return mem;
        }
    }
    return b;
}

A A_hold(A a) {
    if (a) {
        A f = a - 1;
        if (f->refs++ == 1 && f->ar_index > 0)
            af_top->pool->elements[f->ar_index] = null; // index of 0 is occupied by the pool itself; just a sentinel 
    }
    return a;
}

void A_free(A a) {
    A       aa = (a - 1);
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

void A_drop(A a) {
    if (a && --(a - 1)->refs == -1)
        A_free(a);
}

A A_fields(A instance) {
    return (instance - 1)->origin;
}

A A_data(A instance) {
    A obj = A_fields(instance);
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
    n->value = e; /// held already by caller
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

/// we need index_of_element and index_of
/// this is not calling compare for now, and we really need to be able to control that if it did (argument-based is fine)
static num list_index_of_element(list a, A value) {
    num index = 0;
    for (item ai = a->first; ai; ai = ai->next) {
        if (ai->value == value)
            return index;
        index++;
    }
    return -1;
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
        A_f* ai_t = *(A_f**)&((A)ai->value)[-1];
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

static A list_get(list a, object at_index) {
    num index = 0;
    assert(isa(at_index) == typeid(i32), "invalid indexing type");
    i32 at = *(i32*)at_index;
    for (item i = a->first; i; i = i->next) {
        if (at == index)
            return i->value;
        index++;
    }
    assert(false, "could not fetch item at index %i", at);
    return null;
}

static num list_count(list a) {
    return a->count;
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
        assert(is_meta_compatible(a, b), "not meta compatible");
    a->elements[a->len++] = A_hold(b);
    return b;
}

static none array_concat(array a, array b) {
    each(b, A, e) array_push(a, e);
}

static A array_index_num(array a, num i) {
    if (i < 0)
        i += a->len;
    if (i >= a->len)
        return 0;
    return a->elements[i];
}

static A array_remove(array a, num b) {
    A before = a->elements[b];
    for (num i = b; i < a->len; i++) {
        A prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
        A_drop(prev);
    }
    a->elements[--a->len] = null;
    return before;
}

static A array_remove_weak(array a, num b) {
    A before = a->elements[b];
    for (num i = b; i < a->len; i++) {
        A prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
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
    assert(a->len, "no items");
    return a->elements[a->len - 1];
}

static A array_last(array a) {
    assert(a->len, "no items");
    return a->elements[a->len - 1];
}

static void array_push_symbols(array a, ...) {
    va_list args;
    va_start(args, a);
    char* value;
    while ((value = va_arg(args, char*)) != null) {
        string s = new(string, chars, value);
        M(a, push, s);
    }
    va_end(args);
}

static void array_push_objects(array a, A f, ...) {
    va_list args;
    va_start(args, f);
    A value;
    while ((value = va_arg(args, A)) != null)
        M(a, push, value);
    va_end(args);
}

map map_of(cstr first_key, ...) {
    map a = new(map, hsize, 16);
    va_list args;
    va_start(args, first_key);
    cstr key = first_key;
    for (;;) {
        A arg = va_arg(args, A);
        M(a, set, str(key), arg);
        key = va_arg(args, cstr);
        if (key == null)
            break;
    }
    return a;
}

array array_of(AType validate, ...) {
    array a = new(array);
    va_list args;
    va_start(args, validate);
    for (;;) {
        A arg = va_arg(args, A);
        if (!arg)
            break;
        assert(!validate || validate == isa(arg), "validation failure");
        M(a, push, arg);
    }
    return a;
}

array array_of_cstr(cstr first, ...) {
    array a = allocate(array);
    va_list args;
    va_start(args, first);
    for (;;) {
        cstr arg = va_arg(args, A);
        if (!arg)
            break;
        M(a, push, allocate(string, chars, arg));
    }
    return a;
}

void  array_weak_push(array a, A obj) {
    a->elements[a->len++] = obj;
}

static A array_pop(array a) {
    assert(a->len > 0, "no items");
    return a->elements[a->len--];
}

static num array_compare(array a, array b) {
    num diff = a->len - b->len;
    if (diff != 0)
        return diff;
    for (num i = 0; i < a->len; i++) {
        num cmp = M(a->elements[i], compare, b->elements[i]);
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
        if (M(a -> elements[i], compare, b) == 0)
            return i;
    }
    return -1;
}

static bool array_cast_bool(array a) { return a && a->len > 0; }

static none array_init(array a) {
    if (a->alloc)
        array_alloc_sz(a, a->alloc);
}

static void AF_init(AF a) {
    af_top = a;
    a->pool = allocate(array, alloc, a->start_size ? a->start_size : 1024);
    M(a->pool, push_weak, a); // push self to pool, now all indices are > 0; obviously we dont want to free this though

    if (!af_stack) af_stack = allocate(array, alloc, 16);
    M(af_stack, push_weak, a);
}

AF AF_create(sz start_size) {
    AF a = allocate(AF);
    a->start_size = start_size;
    AF_init(a);
    return a;
}

AF A_pool(sz start_size) {
    return AF_create(start_size);
}

static void AF_destructor(AF a) {
    int f = M(af_stack, index_of, a);
    assert(f >= 0, "invalid af-stack index");
    M(af_stack, remove_weak, f);
    if (af_top == a)
        af_top = af_stack->len ? af_stack->elements[af_stack->len - 1] : null;
    for (int i = 1; i < a->pool->len; i++) {
        A ref = a->pool->elements[i];
        if (ref) {
            A f = A_fields(ref);
            A_free(ref);
        }
    }
    A_free(a->pool);
}

static AF AF_fetch(num index) {
    if (af_stack && abs((int)index) < af_stack->len)
        return index < 0 ? af_stack->elements[af_stack->len + index] :
                           af_stack->elements[index];
    else
        return null;
}

static u64 fn_hash(fn f) {
    return (u64)f->method->address;
}

static A fn_call(fn f, array args) {
    return A_method_call(f->method, args);
}

bool create_symlink(path target, path link) {
    bool is_err = symlink(target, link) == -1;
    return !is_err;
}

static none path_init(path a) {
    cstr arg = a->chars;
    num  len = arg ? strlen(arg) : 0;
    a->chars = calloc(len + 1, 1);
    if (arg)
        memcpy(a->chars, arg, len + 1);
}

static path path_with_string(path a, string s) {
    return new(path, chars, s->chars);
}

static sz path_cast_sz(path a) {
    return strlen(a->chars);
}

static string path_cast_cstr(path a) {
    return a->chars;
}

static string path_cast_string(path a) {
    return new(string, chars, a->chars);
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

static bool path_is_dir(path a) {
    DIR   *dir = opendir(a->chars);
    if (dir == NULL)
        return false;
    closedir(dir);
    return true;
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
    string res = new(string, alloc, 256);
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
    string res = new(string, alloc, 256);
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
    assert(res != null, "getcwd failure");
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
    if (type == typeid(string)) {
        fseek(f, 0, SEEK_END);
        sz flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        string a = new(string, alloc, flen);
        size_t n = fread(a->chars, 1, flen, f);
        fclose(f);
        assert(n == flen, "could not read enough bytes");
        a->len   = flen;
        return a;
    }
    assert(false, "not implemented");
    return null;
}

void* primitive_ffi_arb(AType ptype) {
    if (ptype == typeid(u8)) return &ffi_type_uint8;
    if (ptype == typeid(i8)) return &ffi_type_sint8;
    if (ptype == typeid(u16)) return &ffi_type_uint16;
    if (ptype == typeid(i16)) return &ffi_type_sint16;
    if (ptype == typeid(u32)) return &ffi_type_uint32;
    if (ptype == typeid(i32)) return &ffi_type_sint32;
    if (ptype == typeid(u64)) return &ffi_type_uint64;
    if (ptype == typeid(i64)) return &ffi_type_sint64;
    if (ptype == typeid(f32)) return &ffi_type_float;
    if (ptype == typeid(f64)) return &ffi_type_double;
    //if (ptype == typeid(f128)) return &ffi_type_longdouble;
    if (ptype == typeid(cstr)) return &ffi_type_pointer;
    if (ptype == typeid(symbol)) return &ffi_type_pointer;
    if (ptype == typeid(cereal)) return &ffi_type_pointer;
    if (ptype == typeid(bool)) return &ffi_type_uint32;
    if (ptype == typeid(num)) return &ffi_type_sint64;
    if (ptype == typeid(sz)) return &ffi_type_sint64;
    if (ptype == typeid(none)) return &ffi_type_void;
    if (ptype == typeid(AType)) return &ffi_type_pointer;
    if (ptype == typeid(handle)) return &ffi_type_pointer;
    if (ptype == typeid(Member)) return &ffi_type_pointer;
    if (ptype == typeid(ARef))   return &ffi_type_pointer;
    if (ptype == typeid(raw))    return &ffi_type_pointer;
    assert(false, "primitive_ffi_arb not implemented: %s", ptype->name);
    return null;
}

define_class(A)

define_abstract(numeric)
define_abstract(string_like)
define_abstract(nil)
define_abstract(raw)
define_abstract(ref)
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
define_primitive(cereal, string_like, 0)
define_primitive(none,   nil, 0)
define_primitive(AType,  raw, 0)
define_primitive(handle, raw, 0)
define_primitive(Member, raw, 0)
define_primitive(ARef,   ref, 0);

define_enum(OPType)
define_enum(Exists)

define_class(path)
define_class(string)
define_class(item)
//define_proto(collection) -- disabling for now during reduction to base + class + mod
define_class(list)
define_class(array)
define_class(hashmap)
define_class(pair)
define_mod(map, list) // never quite did one like this but it does make sense
define_class(fn)

define_class(AF)

define_meta(array, ATypes, AType)

/*
map map_parse(map a, int argc, cstr *argv, map def) {
    map iargs = new(map);
    for (int ai = 0; ai < argc; ai++) {
        cstr ps = argv[ai];
        ///
        if (ps[0] == '-') {
            bool   is_single = ps[1] != '-';
            mx key {
                cstr(&ps[is_single ? 1 : 2]), typeid(char)
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