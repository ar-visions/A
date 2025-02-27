#include <import>
#include <A-reserve>
#include <ffi.h>
#include <str.h>
#undef bool
#include <sys/stat.h>
#include <dirent.h>
#include <endian-cross.h>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <cpuid.h>
#endif
#include <math.h>

/// block-chain will go here in quanta

__thread array     af_stack;
__thread   AF      af_top;

static global_init_fn* call_after;
static num             call_after_alloc;
static num             call_after_count;
static map             log_fields;

static CPU_Caps        cpu_caps;

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
CPU_Caps detect_x86_caps() {
    u32 eax, ebx, ecx, edx;
    CPU_Caps cpu_caps = 0;
    // Basic CPUID information
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        // Check for SSE and SSE2
        if (edx & (1 << 25)) {
            cpu_caps |= CPU_CAP_SSE;   // SSE
        }
        if (edx & (1 << 26)) {
            cpu_caps |= CPU_CAP_SSE2;  // SSE2
        }
        // Check for AVX
        if (ecx & (1 << 28)) {
            // Check for OS support for AVX using XGETBV
            u32 xcr0;
            __asm__ volatile ("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
            xcr0 = eax;
            if ((xcr0 & 0x6) == 0x6) {
                cpu_caps |= CPU_CAP_AVX;  // AVX
            }
        }
    }

    // Extended CPUID information
    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
        // Check for AVX2
        if (ebx & (1 << 5)) {
            cpu_caps |= CPU_CAP_AVX2;  // AVX2
        }
        // Check for AVX-512
        if (ebx & (1 << 16)) {
            // Check for OS support for AVX-512 using XGETBV
            unsigned int xcr0;
            __asm__ volatile ("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
            xcr0 = eax;
            if ((xcr0 & 0xE0) == 0xE0) {
                cpu_caps |= CPU_CAP_AVX512;  // AVX-512
            }
        }
    }
    return cpu_caps;
}
#elif defined(__arm__) || defined(__aarch64__)
CPU_Caps detect_arm_caps() {
    CPU_Caps cpu_caps = 0;
    #if defined(__ARM_NEON)
        cpu_caps |= CPU_CAP_NEON32;  // NEON for ARM32
    #endif

    #if defined(__aarch64__)
        cpu_caps |= CPU_CAP_NEON64;  // NEON is standard on ARM64
    #endif
    return cpu_caps;
}
#endif

CPU_Caps detect_cpu_caps() {
    #if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
        return detect_x86_caps();
    #elif defined(__arm__) || defined(__aarch64__)
        return detect_arm_caps();
    #else
        return 0;
    #endif
}

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

cstr copy_cstr(cstr input) {
    sz len = strlen(input);
    cstr res = calloc(len + 1, 1);
    memcpy(res, input, len);
    return res;
}

A A_header(object instance) {
    return (((struct _A*)instance) - 1);
}

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

void A_push_type(AType type) {
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
object A_validate_type(object instance, AType type) {
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

AType A_find_type(cstr name) {
    for (int i = 0; i < types_len; i++) {
        AType type = types[i];
        if (strcmp(type->name, name) == 0)
            return type;
    }
    return null;
}

object A_new(AType type) {
    object res = A_alloc(type, 1, true);
    // todo: do not call init in A_alloc
    return res;
}

static void A_validator(object a) {
    AType type = isa(a);
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* m = &type->members[i];
        if (m->required) {
            u8* ptr = (u8*)a + m->offset;
            object*  ref = (object*)ptr;
            verify(*ref, "required arg [%s] not set for class %s", m->name, type->name);
        }
    }
}

i32 A_enum_value(AType type, cstr cs) {
    int cur = 0;
    int default_val = INT_MIN;
    for (num m = 0; m < type->member_count; m++) {
        type_member_t* mem = &type->members[m];
        if (mem->member_type & A_MEMBER_ENUMV) {
            if (!cs || strcmp(cs, mem->name) == 0)
                return cur;
            if (default_val == INT_MIN) default_val = cur;
            cur++;
        }
    }
    if (default_val != INT_MIN)
        return default_val;
    fault("enum not found");
    return -1;
}

string A_enum_string(AType type, i32 value) {
    int cur = 0;
    for (num m = 0; m < type->member_count; m++) {
        type_member_t* mem = &type->members[m];
        if (mem->member_type & A_MEMBER_ENUMV) {
            if (cur == value)
                return string(mem->name); 
            cur++;
        }
    }
    fault ("invalid enum-value of %i for type %s", value, type->name);
    return null;
}

void debug() {
    return;
}

void A_init_recur(object a, AType current, raw last_init) {
    if (current == &A_type) return;
    void(*init)(object) = ((A_f*)current)->init;
    A_init_recur(a, current->parent_type, (raw)init);
    if (init && init != (void*)last_init) init(a); 
}

object A_initialize(object a) {
    A f           = A_header(a);

    #ifndef NDEBUG
    A_validator(a);
    #endif

    A_init_recur(a, f->type, null);
    return a;
}

object A_alloc(AType type, num count, bool af_pool) {
    sz map_sz = sizeof(map);
    sz A_sz = sizeof(struct _A);
    object a      = calloc(1, A_sz + type->size * count);
    a->refs       = af_pool ? 0 : 1;
    a->type       = type;
    a->data       = &a[1];
    a->count      = count;
    a->alloc      = count;
    if (af_pool) {
        a->ar_index = af_top->pool->len;
        push(af_top->pool, a->data);
    } else {
        a->ar_index = 0; // Indicate that this object is not in the auto-free pool
    }
    return a->data; /// return fields (object)
}

object A_alloc_extra(AType type, num extra, bool af_pool) {
    sz map_sz = sizeof(map);
    sz A_sz = sizeof(struct _A);
    object a      = calloc(1, A_sz + type->size + extra);
    a->refs       = af_pool ? 0 : 1;
    a->type       = type;
    a->data       = &a[1];
    a->count      = 1;
    a->alloc      = 1;
    if (af_pool) {
        a->ar_index = af_top->pool->len;
        push(af_top->pool, a->data);
    } else {
        a->ar_index = 0; // Indicate that this object is not in the auto-free pool
    }
    return a->data; /// return fields (object)
}

object A_valloc(AType type, AType scalar, int alloc, int count, bool af_pool) {
    verify(type,   "type not set");
    verify(scalar, "scalar not set");
    i64 A_sz      = sizeof(struct _A);
    object a      = calloc(1, A_sz + type->size);
    a->refs       = af_pool ? 0 : 1;
    a->scalar     = scalar;
    a->type       = type;
    a->data       = &a[1];
    a->count      = count;
    a->alloc      = alloc;
    a->data       = calloc(alloc, scalar->size);
    if (af_pool) {
        a->ar_index = af_top->pool->len;
        push(af_top->pool, a->data);
    } else {
        a->ar_index = 0;
    }
    return a->data;
}

object A_alloc2(AType type, AType scalar, veci64 shape, bool af_pool) {
    i64 A_sz      = sizeof(struct _A);
    i64 count     = veci64_product(shape);
    object a      = calloc(1, A_sz + (scalar ? scalar->size : type->size) * count);
    a->refs       = af_pool ? 0 : 1;
    a->scalar     = scalar;
    a->type       = type;
    a->data       = &a[1];
    a->shape      = A_hold(shape);
    a->count      = count;
    a->alloc      = count;
    if (af_pool) {
        a->ar_index = af_top->pool->len;
        push(af_top->pool, a->data);
    } else {
        a->ar_index = 0; // Indicate that this object is not in the auto-free pool
    }
    return a->data; /// return fields (object)
}

#define iarray(I,M,...) array_ ## M(I, M, __VA_ARGS__)


/// array -------------------------
void array_alloc_sz(array a, sz alloc) {
    object* elements = (object*)calloc(alloc, sizeof(struct _A*));
    memcpy(elements, a->elements, sizeof(struct _A*) * a->len);
    
    free(a->elements);
    a->elements = elements;
    a->alloc = alloc;
}

void array_fill(array a, object f) {
    for (int i = 0; i < a->alloc; i++)
        push(a, f);
}

string array_cast_string(array a) {
    string r = string(alloc, 64);
    for (int i = 0; i < a->len; i++) {
        object e = (object)a->elements[i];
        string s = cast(string, e);
        if (r->len)
            append(r, " ");
        append(r, s ? s->chars : "null");
    }
    return r;
}

array array_reverse(array a) {
    array r = array((int)len(a));
    for (int i = len(a) - 1; i >= 0; i--)
        push(r, a->elements[i]);
    return r;
}

void array_expand(array a) {
    num alloc = 32 + (a->alloc << 1);
    array_alloc_sz(a, alloc);
}

void array_push_weak(array a, object b) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    AType t = isa(a);
    if (is_meta((object)a))
        assert(is_meta_compatible((object)a, (object)b), "not meta compatible");
    a->elements[a->len++] = b;
}

array  array_copy(array a) {
    array  b = new(array, alloc, len(a));
    concat(b, a);
    return b;
}

array array_with_i32(array a, i32 alloc) {
    a->alloc = alloc;
    return a;
}

void array_push(array a, object b) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    AType t = isa(a);
    if (is_meta(a))
        assert(is_meta_compatible(a, b), "not meta compatible");
    a->elements[a->len++] = A_hold(b);
}

void array_clear(array a) {
    for (num i = 0; i < a->len; i++) {
        A_drop(a->elements[i]);
        a->elements[i] = null;
    }
    a->len = 0;
}

none array_concat(array a, array b) {
    each(b, object, e) array_push(a, e);
}

object array_index_num(array a, num i) {
    if (i < 0)
        i += a->len;
    if (i >= a->len)
        return 0;
    return a->elements[i];
}

void array_remove(array a, num b) {
    for (num i = b; i < a->len; i++) {
        object prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
        A_drop(prev);
    }
    a->elements[--a->len] = null;
}

void array_remove_weak(array a, num b) {
    for (num i = b; i < a->len; i++) {
        object prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
    }
    a->elements[--a->len] = null;
}

void array_operator__assign_add(array a, object b) {
    array_push(a, b);
}

void array_operator__assign_sub(array a, num b) {
    array_remove(a, b);
}

object array_first(array a) {
    assert(a->len, "no items");
    return a->elements[a->len - 1];
}

object array_last(array a) {
    assert(a->len, "no items");
    return a->elements[a->len - 1];
}

void array_push_symbols(array a, ...) {
    va_list args;
    va_start(args, a);
    char* value;
    while ((value = va_arg(args, char*)) != null) {
        string s = new(string, chars, value);
        push(a, s);
    }
    va_end(args);
}

void array_push_objects(array a, A f, ...) {
    va_list args;
    va_start(args, f);
    A value;
    while ((value = va_arg(args, A)) != null)
        push(a, value);
    va_end(args);
}

array array_of(object first, ...) {
    array a = new(array);
    va_list args;
    va_start(args, first);
    if (first) {
        push(a, first);
        for (;;) {
            A arg = va_arg(args, object);
            if (!arg)
                break;
            push(a, arg);
        }
    }
    return a;
}

array array_of_cstr(cstr first, ...) {
    array a = allocate(array);
    va_list args;
    va_start(args, first);
    for (cstr arg = first; arg; arg = va_arg(args, A))
        push(a, allocate(string, chars, arg));
    return a;
}

raw push_generic(object a) {
    A aa = A_header(a);
    verify (!aa->shape, "shape undefined");
    return A_realloc(a, aa->count + 1);
}

none vecf64_push(vecf64 a, f64 v) {
    f64* ptr = push_generic(a);
    *ptr = v;
}

none vecf_push(vecf a, f32 v) {
    f32* ptr = push_generic(a);
    *ptr = v;
}

none veci64_push(veci64 a, i64 v) {
    i64* ptr = push_generic(a);
    *ptr = v;
}

none veci8_push(veci8 a, i8 v) {
    i8* ptr = push_generic(a);
    *ptr = v;
}

vecf64 vecf64_typed_copy(vecf64 a) {
    return vecf64(A_data(a));
}

vecf vecf_typed_copy(vecf a) {
    return vecf(A_data(a));
}

veci64 veci64_typed_copy(veci64 a) {
    return veci64(A_data(a));
}

veci8 veci8_typed_copy(veci8 a) {
    return veci8(A_data(a));
}

veci8  new_veci8(object data) {
    A a = A_header(data);
    i8* values = data;
    if (a->count == 2) return vec2i8(values[0], values[1]);
    if (a->count == 3) return vec3i8(values[0], values[1], values[2]);
    if (a->count == 4) return vec4i8(values[0], values[1], values[2], values[3]);
    veci8 def = new(veci8);
    A def_a = A_header(def);
    def_a->data = hold(data); /// drop logic handles data reallocation cases
    return def;
}

veci64 new_veci64(object data) {
    A a = A_header(data);
    i64* values = data;
    if (a->count == 2) return vec2i64(values[0], values[1]);
    if (a->count == 3) return vec3i64(values[0], values[1], values[2]);
    if (a->count == 4) return vec4i64(values[0], values[1], values[2], values[3]);
    veci64 def = new(veci64);
    A def_a = A_header(def);
    def_a->data = hold(data);
    return def;
}

vecf64 new_vecf64(object data) {
    A a = A_header(data);
    f64* values = data;
    if (a->count == 2) return vec2f64(values[0], values[1]);
    if (a->count == 3) return vec3f64(values[0], values[1], values[2]);
    if (a->count == 4) return vec4f64(values[0], values[1], values[2], values[3]);
    vecf64 def = new(vecf64);
    A def_a = A_header(def);
    def_a->data = hold(data);
    return def;
}

vecf   new_vecf(object data) {
    A a = A_header(data);
    f32* values = data;
    if (a->count == 2) return vec2f(values[0], values[1]);
    if (a->count == 3) return vec3f(values[0], values[1], values[2]);
    if (a->count == 4) return vec4f(values[0], values[1], values[2], values[3]);
    vecf def = new(vecf);
    A def_a = A_header(def);
    def_a->data = hold(data);
    return def;
}

i8    veci8_get (veci8  a, num i)          { i8*    m = A_data(a); return m[i]; }
i64   veci64_get(veci64 a, num i)          { i64*   m = A_data(a); return m[i]; }
f32   vecf_get  (vecf   a, num i)          { float* m = A_data(a); return m[i]; }
real  vecf64_get(vecf64 a, num i)          { real*  m = A_data(a); return m[i]; }
i8    veci8_set (veci8  a, num i, i8   n)  { i8*    m = A_data(a); i8    r = m[i]; m[i] = n; return r; }
i64   veci64_set(veci64 a, num i, i64  n)  { i64*   m = A_data(a); i64   r = m[i]; m[i] = n; return r; }
f32   vecf_set  (vecf   a, num i, f32  n)  { float* m = A_data(a); float r = m[i]; m[i] = n; return r; }
real  vecf64_set(vecf64 a, num i, real n)  { real*  m = A_data(a); real  r = m[i]; m[i] = n; return r; }

sz    veci8_len (veci8  a) { return A_header(a)->count; }
sz    veci64_len(veci64 a) { return A_header(a)->count; }
sz    vecf_len  (vecf   a) { return A_header(a)->count; }
sz    vecf64_len(vecf64 a) { return A_header(a)->count; }

num veci8_index_of(veci8 a, i8 n) {
    i8* v = A_header(a)->data;
    num c = A_header(a)->count;
    for (num i = 0; i < c; i++) if (v[i] == n) return i; return -1;
}

num veci64_index_of(veci64 a, i64 n) {
    i64* v = A_header(a)->data;
    num c = A_header(a)->count;
    for (num i = 0; i < c; i++) if (v[i] == n) return i; return -1;
}

num vecf64_index_of(vecf64 a, f64 n) {
    f64* v = A_header(a)->data;
    num c = A_header(a)->count;
    for (num i = 0; i < c; i++) if (v[i] == n) return i; return -1;
}

num vecf_index_of(vecf a, f32 n) {
    f32* v = A_header(a)->data;
    num c = A_header(a)->count;
    for (num i = 0; i < c; i++) if (v[i] == n) return i; return -1;
}

/// type system is not running when this is called
veci64 veci64_of(i64 first, ...) {
    i64 count = 0;
    va_list args;
    va_start(args, first);
    for (i64 arg = first; arg != 0; arg = va_arg(args, i64))
        count++;
    
    A a = calloc(1024 + sizeof(A) + sizeof(i64) * (count + 1), 1);
    a->count = count;
    a->type  = typeid(i64);
    a->data  = &a[1];
    a->alloc = count;
    veci64 vec = &a[1];
    va_start(args, first);
    i64 index = 0;
    for (i64 arg = first; arg != 0; arg = va_arg(args, i64))
        ((i64*)vec)[index++] = arg;
    return vec;
}

void  array_weak_push(array a, object obj) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    a->elements[a->len++] = obj;
}

object array_pop(array a) {
    assert(a->len > 0, "no items");
    if (!a->unmanaged) drop(a->elements[a->len - 1]);
    return a->elements[--a->len];
}

num array_compare(array a, array b) {
    num diff = a->len - b->len;
    if (diff != 0)
        return diff;
    for (num i = 0; i < a->len; i++) {
        num cmp = compare(a->elements[i], b->elements[i]);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

object array_get(array a, num i) {
    if (i < 0 || i > a->len)
        fault("out of bounds: %i, len = %i", i, a->len);
    return a->elements[i];
}

num array_count(array a) {
    return a->len;
}

sz array_len(array a) {
    return a->len;
}

/// index of element that compares to 0 diff with item
num array_index_of(array a, object b) {
    for (num i = 0; i < a->len; i++) {
        if (compare(a -> elements[i], b) == 0)
            return i;
    }
    return -1;
}

void br() {
    usleep(0);
}

bool array_cast_bool(array a) { return a && a->len > 0; }

none array_init(array a) {
    if (a->alloc)
        array_alloc_sz(a, a->alloc);
}

method_t* method_with_address(handle address, AType rtype, array atypes, AType method_owner) {
    const num max_args = (sizeof(meta_t) - sizeof(num)) / sizeof(AType);
    method_t* method = calloc(1, sizeof(method_t));
    method->ffi_cif  = calloc(1,        sizeof(ffi_cif));
    method->ffi_args = calloc(max_args, sizeof(ffi_type*));


    //_Generic(("hi"), string_schema(string, GENERICS) const void *: (void)0)("hi")  


    method->atypes   = new(array);
    method->rtype    = rtype;
    method->address  = address;
    assert(atypes->len <= max_args, "adjust arg maxima");
    ffi_type **ffi_args = (ffi_type**)method->ffi_args;
    for (num i = 0; i < atypes->len; i++) {
        A_f* a_type   = (A_f*)atypes->elements[i];
        bool is_prim  = a_type->traits & A_TRAIT_PRIMITIVE;
        ffi_args[i]   = is_prim ? a_type->arb : &ffi_type_pointer;
        push_weak(method->atypes, (object)a_type);
    }
    ffi_status status = ffi_prep_cif(
        (ffi_cif*) method->ffi_cif, FFI_DEFAULT_ABI, atypes->len,
        (ffi_type*)((rtype->traits & A_TRAIT_ABSTRACT) ? method_owner->arb : rtype->arb), ffi_args);
    assert(status == FFI_OK, "status == %i", (i32)status);
    return method;
}

/// should work on statics or member functions the same; its up to the caller to push the self in there
fn A_lambda(object target, Member member, object context) {
    fn f = new(fn,
        method,  member->method,
        target,  target,
        context, context);
    return f;
}

object A_method_call(method_t* a, array args) {
    const num max_args = 8;
    void* arg_values[max_args];
    assert(args->len == a->atypes->len, "arg count mismatch");
    for (num i = 0; i < args->len; i++) {
        A_f* arg_type = (A_f*)a->atypes->elements[i];
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
        return (object) result[0];
}

/// this calls type methods
object A_method(AType type, cstr method_name, array args) {
    type_member_t* mem = A_member(type, A_MEMBER_IMETHOD | A_MEMBER_SMETHOD, method_name);
    assert(mem->method, "method not set");
    method_t* m = mem->method;
    object res = A_method_call(m, args);
    return res;
}

object A_convert(AType type, object input) {
    if (type == isa(input)) return input;
    assert(false, "not implemented");
    return input;
}

object A_method_vargs(object instance, cstr method_name, int n_args, ...) {
    AType type = isa(instance);
    type_member_t* mem = A_member(type, A_MEMBER_IMETHOD | A_MEMBER_SMETHOD, method_name);
    assert(mem->method, "method not set");
    method_t* m = mem->method;
    va_list  vargs;
    va_start(vargs, n_args);
    array args = new(array, alloc, n_args + 1);
    push(args, instance);
    for (int i = 0; i < n_args; i++) {
        object arg = va_arg(vargs, object);
        push(args, arg);
    }
    va_end(vargs);
    object res = A_method_call(m, args);
    return res;
}


int fault_level;

static __attribute__((constructor)) bool Aglobal_AF();

void A_start() {
    fault_level = level_err;
    AF pool    = allocate(AF); /// leave pool open [ AF_type is not being populated; check for creation of AF_init and global
    log_fields = new(map, hsize, 32);

    int remaining = call_after_count;
    while (remaining)
        for (int i = 0; i < call_after_count; i++) {
            global_init_fn fn2 = call_after[i];
            if (fn2 && fn2()) {
                call_after[i] = null;
                remaining--;
            }
        }

    remaining = call_last_count;
    while (remaining)
        for (int i = 0; i < call_last_count; i++) {
            global_init_fn fn2 = call_last[i];
            if (fn2 && fn2()) {
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
            if (mem->member_type & (A_MEMBER_IMETHOD | A_MEMBER_SMETHOD)) {
                void* address = 0;
                memcpy(&address, &((u8*)type)[mem->offset], sizeof(void*));
                assert(address, "no address");
                array args = allocate(array, alloc, mem->args.count);
                for (num i = 0; i < mem->args.count; i++)
                    args->elements[i] = (object)((A_f**)&mem->args.meta_0)[i];
                args->len = mem->args.count;
                mem->method = method_with_address(address, mem->type, args, type);
            }
        }
    }
}

map A_args(int argc, symbol argv[], symbol default_arg, ...) {
    va_list  args;
    va_start(args, default_arg);
    symbol arg = default_arg;
    map    defaults = new(map);
    while (arg) {
        object val = va_arg(args, object);
        set(defaults, str(arg), val);
        arg = va_arg(args, symbol);
    }
    va_end(args);
    return A_arguments(argc, argv, defaults,
        default_arg ? str(default_arg) : null);
}

none A_tap(symbol f, subprocedure sub) {
    string fname = str(f);
    set(log_fields, fname, sub ? (object)sub : (object)A_bool(true)); /// if subprocedure, then it may receive calls for the logging
}

none A_untap(symbol f) {
    string fname = str(f);
    set(log_fields, fname, A_bool(false));
}

type_member_t* A_member(AType type, enum A_MEMBER member_type, char* name) {
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        if (mem->member_type & member_type && strcmp(mem->name, name) == 0)
            return mem;
    }
    return 0;
}

type_member_t* A_hold_members(object instance) {
    AType type = isa(instance);
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        object   *mdata = (object*)((cstr)instance + mem->offset);
        if (mem->member_type & (A_MEMBER_PROP | A_MEMBER_PRIV | A_MEMBER_INTERN))
            if (!(mem->type->traits & A_TRAIT_PRIMITIVE))
                A_hold(*mdata);
    }
    return 0;
}

bool A_is_inlay(type_member_t* m) {
    return m->type->traits & A_TRAIT_PRIMITIVE | m->type->traits & A_TRAIT_ENUM | 
        m->member_type == A_MEMBER_INLAY;
}

object A_set_property(object instance, symbol name, object value) {
    AType type = isa(instance);
    type_member_t* m = A_member(type, (A_MEMBER_PROP | A_MEMBER_PRIV | A_MEMBER_INTERN), (cstr)name);
    verify(m, "%s not found on object %s", name, type->name);
    object   *mdata = (object*)((cstr)instance + m->offset);
    if (A_is_inlay(m)) {
        // copy inlay data
        memcpy(mdata, value, m->type->size);
    } else {
        object prev = *mdata;
        // assign as object, increase ref count
        *mdata = A_hold(value); 
        A_drop(prev);
    }
    return value;
}


object A_get_property(object instance, symbol name) {
    AType type = isa(instance);
    type_member_t* m = A_member(type, (A_MEMBER_PROP | A_MEMBER_PRIV | A_MEMBER_INTERN), (cstr)name);
    verify(m, "%s not found on object %s", name, type->name);
    object *mdata = (object*)((cstr)instance + m->offset);
    object  value = *mdata;
    return A_is_inlay(m) ? A_primitive(m->type, mdata) : value;
}


/// should be adapted to work with schemas 
/// what a weird thing it would be to have map access to properties
/// everything should be A-based, and forget about the argument hacks?
map A_arguments(int argc, symbol argv[], map default_values, object default_key) {
    map result = new(map, hsize, 16);
    for (item ii = default_values->first; ii; ii = ii->next) {
        pair  hm = ii->value;
        object k = hm->key;
        object v = hm->value;
        set(result, k, v);
    }
    int    i = 1;
    bool found_single = false;
    while (i < argc + 1) {
        symbol arg = argv[i];
        if (!arg) {
            i++;
            continue;
        }
        if (arg[0] == '-') {
            // -s or --silver
            bool doub  = arg[1] == '-';
            string s_key = new(string, chars, (cstr)&arg[doub + 1]);
            string s_val = new(string, chars, (cstr)argv[i + 1]);

            for (item f = default_values->first; f; f = f->next) {
                /// import A types from runtime
                pair      mi = (pair) f->value;
                object def_value = mi->value;
                AType   def_type = def_value ? isa(def_value) : typeid(string);
                assert(f->key == mi->key, "keys do not match"); /// make sure we copy it over from refs
                if ((!doub && strncmp(((string)f->key)->chars, s_key->chars, 1) == 0) ||
                    ( doub && compare(f->key, s_key) == 0)) {
                    /// inter-op with object-based A-type sells it.
                    /// its also a guide to use the same schema
                    object value = A_formatter(def_type, null, (object)false, "%o", s_val);
                    assert(isa(value) == def_type, "");
                    set(result, f->key, value);
                }
            }
        } else if (!found_single && default_key) {
            A default_key_obj = A_header(default_key);
            string s_val     = new(string, chars, (cstr)arg);
            object def_value = get(default_values, default_key);
            AType  def_type  = isa(def_value);
            object value     = A_formatter(def_type, null, (object)false, "%o", s_val);
            set(result, default_key, value);
            found_single = true;
        }
        i += 2;
    }
    return result;
}

/// can we have i32_with_i16(i32 a, i16 b)
/// primitives are based on A alone
/// i wonder if we can add more constructors or even methods to the prims

object A_primitive(AType type, void* data) {
    assert(type->traits & A_TRAIT_PRIMITIVE, "must be primitive");
    object copy = A_alloc(type, type->size, true);
    memcpy(copy, data, type->size);
    return copy;
}

object A_enum(AType type, i32 data) {
    assert(type->traits & A_TRAIT_ENUM, "must be enum");
    assert(type->size == sizeof(i32), "enum size invalid");
    object copy = A_alloc(type, type->size, true);
    memcpy(copy, &data, type->size);
    return copy;
}

object A_i8(i8 data)     { return A_primitive(&i8_type,  &data); }
object A_u8(u8 data)     { return A_primitive(&u8_type,  &data); }
object A_i16(i16 data)   { return A_primitive(&i16_type, &data); }
object A_u16(u16 data)   { return A_primitive(&u16_type, &data); }
object A_i32(i32 data)   { return A_primitive(&i32_type, &data); }
object A_u32(u32 data)   { return A_primitive(&u32_type, &data); }
object A_i64(i64 data)   { return A_primitive(&i64_type, &data); }
object     i(i64 data)   { return A_primitive(&i64_type, &data); }
object A_sz (sz  data)   { return A_primitive(&sz_type,  &data); }
object A_u64(u64 data)   { return A_primitive(&u64_type, &data); }
object A_f32(f32 data)   { return A_primitive(&f32_type, &data); }
object  A_f64(f64 data)  { return A_primitive(&f64_type, &data); }
object A_f128(f64 data)  { return A_primitive(&f128_type, &data); }
object     f(f32 data)   { return A_primitive(&f32_type, &data); }
object     r(f64 data)   { return A_primitive(&f64_type, &data); }
object A_cstr(cstr data) { return A_primitive(&cstr_type, &data); }
object A_none()          { return A_primitive(&none_type, NULL); }
object A_bool(bool data) { return A_primitive(&bool_type, &data); }

/// A -------------------------
void A_init(A a) { }
void A_destructor(A a) {
    // go through objects type fields/offsets; 
    // when type is A-based, we release; 
    // this part is 'auto-release', our pool is an AF pool, or auto-free.
    // when new() is made, the reference goes into a pool
    // 
    A        f = A_header(a);
    AType type = f->type;
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* m = &type->members[i];
        if ((m->member_type == A_MEMBER_PROP || m->member_type == A_MEMBER_PRIV) && !(m->type->traits & A_TRAIT_PRIMITIVE)) {
            u8* ptr = (u8*)a + m->offset;
            A*  ref = ptr;
            A_drop(*ref);
            *ref = null;
        }
    }
    if (f->data) {
        A_drop(f->data);
        f->data = null;
    }
}
u64  A_hash      (A a) { return (u64)(size_t)a; }
bool A_cast_bool (A a) { return A_header(a)->count > 0; }

i64 read_integer(object data) {
    AType data_type = isa(data);
    i64 v = 0;
         if (data_type == typeid(i8))   v = *(i8*)  data;
    else if (data_type == typeid(i16))  v = *(i16*) data;
    else if (data_type == typeid(i32))  v = *(i32*) data;
    else if (data_type == typeid(i64))  v = *(i64*) data;
    else if (data_type == typeid(u8))   v = *(u8*)  data;
    else if (data_type == typeid(u16))  v = *(u16*) data;
    else if (data_type == typeid(u32))  v = *(u32*) data;
    else if (data_type == typeid(u64))  v = *(u64*) data;
    else fault("unknown data");
    return v;
}

static cstr ws(cstr p) {
    cstr scan = p;
    while (*scan && isspace(*scan))
        scan++;
    return scan;
}

// lets handle both quoted and non-quoted string serialization
string prep_cereal(cereal cs) {
    cstr   scan = (cstr)cs;
    string res;
    if (*scan == '\"') {
        scan = scan + 1;
        cstr start = scan;
        bool last_s = false;
        while (*scan && *scan != '\\' && !last_s) {
            if (*scan == '\\') last_s = true;
            else last_s = false;
        }
        assert(*scan == '\"', "missing end-quote");
        i64 len = (i64)(scan - start) - 1;
        res = string(alloc, len); // string performs + 1
        memcpy(res->chars, start, len);
    } else
        res = string(scan);
    return res;
}

A A_with_cereal(object a, cereal cs) {
    sz len = strlen(cs);
    A        f = A_header(a);
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

/// used by parse (from json) to construct objects from data
object construct_with(AType type, object data) {
    if (type == typeid(map)) {
        verify(isa(data) == typeid(map), "expected map");
        return hold(data);
    }

    /// this will lookup ways to construct the type from the available data
    AType data_type = isa(data);
    object result = null;

    if (!(type->traits & A_TRAIT_PRIMITIVE) && data_type == typeid(map)) {
        map m = data;
        result = A_alloc(type, 1, true);
        pairs(m, i) {
            verify(isa(i->key) == typeid(string), "expected string key when constructing object from map");
            string s_key = instanceof(i->key, string);
            A_set_property(result, s_key->chars, i->value);
        }
    }

    /// check for identical constructor
    for (int i = 0; i < type->member_count; i++) {
        Member mem = &type->members[i];
        if (!mem->ptr) continue;
        void* addr = mem->ptr;
        if (mem->member_type == A_MEMBER_CONSTRUCT) {
            /// no meaningful way to do this generically, we prefer to call these first
            if (mem->type == typeid(path) && data_type == typeid(string)) {
                result = A_alloc(type, 1, true);
                ((void(*)(object, path))addr)(result, path(((string)data)->chars));
                break;
            }
            if (mem->type == data_type) {
                result = A_alloc(type, 1, true);
                ((void(*)(object, object))addr)(result, data);
                break;
            }
        }
    }

    /// simple enum conversion, with a default handled in A_enum_value and type-based match here
    if (!result)
    if (type->traits & A_TRAIT_ENUM) {
        i64 v = 0;
        if (data_type->traits & A_TRAIT_INTEGRAL)
            v = read_integer(data_type);
        else if (data_type == typeid(symbol) || data_type == typeid(cstr))
            v = A_enum_value (type, (cstr)data);
        else if (data_type == typeid(string))
            v = A_enum_value (type, (cstr)((string)data)->chars);
        else
            v = A_enum_value (type, null);
        result = A_alloc(type, 1, true);
        *((i32*)result) = (i32)v;
    }

    /// check if we may use generic object from string
    if (!result)
    if ((type->traits & A_TRAIT_PRIMITIVE) && (data_type == typeid(string) ||
                                               data_type == typeid(cstr)   ||
                                               data_type == typeid(symbol))) {
        result = A_alloc(type, 1, true);
        if (data_type == typeid(string))
            A_with_cereal(result, (cereal)((string)data)->chars);
        else
            A_with_cereal(result, (cereal)data);
    }

    /// check for compatible constructor
    if (!result)
    for (int i = 0; i < type->member_count; i++) {
        Member mem = &type->members[i];
        if (!mem->ptr) continue;
        void* addr = mem->ptr;
        /// check for compatible constructors
        if (mem->member_type == A_MEMBER_CONSTRUCT) {
            u64 combine = mem->type->traits & data_type->traits;
            if (combine & A_TRAIT_INTEGRAL) {
                i64 v = read_integer(data);
                result = A_alloc(type, 1, true);
                     if (mem->type == typeid(i8))   ((void(*)(object, i8))  addr)(result, (i8)  v);
                else if (mem->type == typeid(i16))  ((void(*)(object, i16)) addr)(result, (i16) v);
                else if (mem->type == typeid(i32))  ((void(*)(object, i32)) addr)(result, (i32) v);
                else if (mem->type == typeid(i64))  ((void(*)(object, i64)) addr)(result, (i64) v);
            } else if (combine & A_TRAIT_REALISTIC) {
                result = A_alloc(type, 1, true);
                if (mem->type == typeid(f64))
                    ((void(*)(object, double))addr)(result, (double)*(float*)data);
                else
                    ((void(*)(object, float)) addr)(result, (float)*(double*)data);
                break;
            } else if ((mem->type == typeid(symbol) || mem->type == typeid(cstr)) && 
                       (data_type == typeid(symbol) || data_type == typeid(cstr))) {
                result = A_alloc(type, 1, true);
                ((void(*)(object, cstr))addr)(result, data);
                break;
            } else if ((mem->type == typeid(string)) && 
                       (data_type == typeid(symbol) || data_type == typeid(cstr))) {
                result = A_alloc(type, 1, true);
                ((void(*)(object, string))addr)(result, string((cstr)data));
                break;
            } else if ((mem->type == typeid(symbol) || mem->type == typeid(cstr)) && 
                       (data_type == typeid(string))) {
                result = A_alloc(type, 1, true);
                ((void(*)(object, cstr))addr)(result, ((string)data)->chars);
                break;
            }
        }
    }
    return result ? A_initialize(result) : null;
}

void A_serialize(AType type, string res, object a) {
    if (type->traits & A_TRAIT_PRIMITIVE) {
        char buf[128];
        int len = 0;
        if      (type == typeid(bool)) len = sprintf(buf, "%s", *(bool*)a ? "true" : "false");
        else if (type == typeid(i64)) len = sprintf(buf, "%lld", *(i64*)a);
        else if (type == typeid(num)) len = sprintf(buf, "%lld", *(i64*)a);
        else if (type == typeid(i32)) len = sprintf(buf, "%d",   *(i32*)a);
        else if (type == typeid(i16)) len = sprintf(buf, "%hd",  *(i16*)a);
        else if (type == typeid(i8))  len = sprintf(buf, "%hhd", *(i8*) a);
        else if (type == typeid(u64)) len = sprintf(buf, "%llu", *(u64*)a);
        else if (type == typeid(u32)) len = sprintf(buf, "%u",   *(u32*)a);
        else if (type == typeid(u16)) len = sprintf(buf, "%hu",  *(u16*)a);
        else if (type == typeid(u8))  len = sprintf(buf, "%hhu", *(u8*) a);
        else if (type == typeid(f128)) len = sprintf(buf, "%f",  (f64)*(f128*)a);
        else if (type == typeid(f64)) len = sprintf(buf, "%f",   *(f64*)a);
        else if (type == typeid(f32)) len = sprintf(buf, "%f",   *(f32*)a);
        else if (type == typeid(cstr)) len = sprintf(buf, "%s",  *(cstr*)a);
        else if (type == typeid(vec2i8))   len = sprintf(buf, "%i %i",               (int)(((i8*) a)[0]), (int)(((i8*) a)[1]));
        else if (type == typeid(vec2f))    len = sprintf(buf, "%.4f %.4f",           ((f32*)a)[0], ((f32*)a)[1]);
        else if (type == typeid(vec2f64))  len = sprintf(buf, "%.4f %.4f",           ((f64*)a)[0], ((f64*)a)[1]);
        else if (type == typeid(vec3i8))   len = sprintf(buf, "%i %i %i",            (int)(((i8*) a)[0]), (int)(((i8*) a)[1]), (int)((((i8*) a)[2])));
        else if (type == typeid(vec3f))    len = sprintf(buf, "%.4f %.4f %.4f",      ((f32*)a)[0], ((f32*)a)[1], ((f32*)a)[2]);
        else if (type == typeid(vec3f64))  len = sprintf(buf, "%.4f %.4f %.4f",      ((f64*)a)[0], ((f64*)a)[1], ((f64*)a)[2]);
        else if (type == typeid(vec4i8))   len = sprintf(buf, "%i %i %i %i",         (int)(((i8*) a)[0]), (int)(((i8*) a)[1]), (int)(((i8*) a)[2]), (int)(((i8*) a)[3]));
        else if (type == typeid(vec4f))    len = sprintf(buf, "%.4f %.4f %.4f %.4f", ((f32*)a)[0], ((f32*)a)[1], ((f32*)a)[2], ((f32*)a)[3]);
        else if (type == typeid(vec4f64))  len = sprintf(buf, "%.4f %.4f %.4f %.4f", ((f64*)a)[0], ((f64*)a)[1], ((f64*)a)[2], ((f32*)a)[3]);
        else if (type == typeid(mat4f)) {
            len = 0;
            fault("implement m4x4");
        }
        else if (type == typeid(mat3f)) {
            len = 0;
            fault("implement m3x3");
        }
        else if (type == typeid(mat2f)) {
            len = 0;
            fault("implement m2x2");
        }
        else {
            fault("implement primitive cast to str: %s", type->name);
        }
        append(res, buf); // should allow for a -1 or len
    } else {
        string s = cast(string, a);
        if (s) {
            append(res, "\"");
            /// encode the characters
            concat(res, escape(s));
            append(res, "\"");
        } else
            append(res, "null");
    }
}

string A_cast_string(A a) {
    AType type = isa(a);
    A a_header = A_header(a);
    if (instanceof(a, string)) return a;
    bool  once = false;
    string res = new(string, alloc, 1024);
    if (type->traits & A_TRAIT_PRIMITIVE)
        A_serialize(type, res, a);
    else {
        //append(res, type->name);
        append(res, "[");
        if (type->src == typeid(matf)) {
            if (type == typeid(mat4f)) {
                for (int r = 0; r < 4 * 4; r++) {
                    if (r) append(res, ", ");
                    A_serialize(typeid(f32), res, A_f32(((mat4f)a)->m[r]));
                }
            } else if (type == typeid(mat3f)) {
                for (int r = 0; r < 3 * 3; r++) {
                    if (r) append(res, ", ");
                    A_serialize(typeid(f32), res, A_f32(((mat3f)a)->m[r]));
                }
            } else if (type == typeid(mat2f)) {
                for (int r = 0; r < 2 * 2; r++) {
                    if (r) append(res, ", ");
                    A_serialize(typeid(f32), res, A_f32(((mat2f)a)->m[r]));
                }
            } else {
                fault("implement generic");
            }
        } else if (type == typeid(veci64) || type->src == typeid(veci64)) {
            for (int r = 0; r < a_header->count; r++) {
                if (r) append(res, ", ");
                A_serialize(typeid(i64), res, A_i64((&((veci64)a)->x)[r]));
            }
        } else if (type == typeid(vecf) || type->src == typeid(vecf)) {
            for (int r = 0; r < a_header->count; r++) {
                if (r) append(res, ", ");
                A_serialize(typeid(f32), res, A_f32((&((vecf)a)->x)[r]));
            }
        } else if (type == typeid(vecf64) || type->src == typeid(vecf64)) {
            for (int r = 0; r < a_header->count; r++) {
                if (r) append(res, ", ");
                A_serialize(typeid(f64), res, A_f64((&((vecf64)a)->x)[r]));
            }
        } else for (num i = 0; i < type->member_count; i++) {
            type_member_t* m = &type->members[i];
            // todo: intern members wont be registered
            if (m->member_type & (A_MEMBER_PROP | A_MEMBER_PRIV | A_MEMBER_INTERN)) {
                if (once)
                    append(res, ", ");
                u8*    ptr = (u8*)a + m->offset;
                object inst = null;
                bool is_primitive = m->type->traits & A_TRAIT_PRIMITIVE;
                if (is_primitive)
                    inst = (object)ptr;
                else
                    inst = *(object*)ptr;
                A inst_h = A_header(inst);
                append(res, m->name);
                append(res, ":");
                if (is_primitive)
                    A_serialize(m->type, res, inst);
                else {
                    string s_value = inst ? A_cast_string(inst) : null; /// isa may be called on this, but not primitive data
                    if (!s_value)
                        append(res, "null");
                    else {
                        /// we should have a bit more information on what sort 
                        /// of string casts will result here; sometimes it 
                        /// returns a json object which is not a string, of course.
                        /// however there is no knowledge of this in the return
                        if (instanceof(inst, string)) {
                            s_value = escape(s_value);
                            append(res, "\"");
                            concat(res, s_value);
                            append(res, "\"");
                        } else {
                            concat(res, s_value);
                        }
                    }
                }
                once = true;
            }
        }
        append(res, "]");
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

object numeric_with_i8 (object a, i8   v) { set_v(); }
object numeric_with_i16(object a, i16  v) { set_v(); }
object numeric_with_i32(object a, i32  v) { set_v(); }
object numeric_with_i64(object a, i64  v) {
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
object numeric_with_u8 (object a, u8   v) { set_v(); }
object numeric_with_u16(object a, u16  v) { set_v(); }
object numeric_with_u32(object a, u32  v) { set_v(); }
object numeric_with_u64(object a, u64  v) { set_v(); }
object numeric_with_f32(object a, f32  v) { set_v(); }
object numeric_with_f64(object a, f64  v) { set_v(); }
object numeric_with_bool(object a, bool v) { set_v(); }
object numeric_with_num(object a, num  v) { set_v(); }

object A_method(AType type, cstr method_name, array args);

sz A_len(object a) {
    if (!a) return 0;
    AType t = isa(a);
    if (t == typeid(string)) return ((string)a)->len;
    if (t == typeid(array))  return ((array) a)->len;
    if (t == typeid(map))    return ((map)a)->count;
    if (t == typeid(cstr) || t == typeid(symbol) || t == typeid(cereal))
        return strlen(a);
    A aa = A_header(a);
    return aa->count;
}

num index_of_cstr(object a, cstr f) {
    AType t = isa(a);
    if (t == typeid(string)) return index_of((string)a, f);
    if (t == typeid(array))  return index_of((array)a, str(f));
    if (t == typeid(cstr) || t == typeid(symbol) || t == typeid(cereal)) {
        cstr v = strstr(a, f);
        return v ? (num)(v - f) : (num)-1;
    }
    fault("len not handled for type %s", t->name);
    return 0;
}

Exists A_exists(object o) {
    AType type = isa(o);
    path  f = null;
    if (type == typeid(string))
        f = cast(path, (string)o);
    else if (type == typeid(path))
        f = o;
    assert(f, "type not supported");
    bool is_dir = is_dir(f);
    bool r = exists(f);
    if (is_dir)
        return r ? Exists_dir  : Exists_no;
    else
        return r ? Exists_file : Exists_no;
}

/// these pointers are invalid for A since they are in who-knows land, but the differences would be the same
i32 A_compare(A a, A b) {
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


object A_formatter(AType type, FILE* f, object opt, cstr template, ...) {
    va_list args;
    va_start(args, template);
    string  res  = new(string, alloc, 1024);
    cstr    scan = template;
    bool write_ln = (i64)opt == true;
    bool is_input = (f == stdin);
    string  field = (!is_input && !write_ln && opt) ? instanceof(opt, string) : null;
    
    while (*scan) {
        /// format %o as object's string cast
        if (*scan == '%' && *(scan + 1) == 'o') {
            object      arg = va_arg(args, object);
            string   a = arg ? cast(string, arg) : string("null");
            num    len = a->len;
            reserve(res, len);
            memcpy(&res->chars[res->len], a->chars, len);
            res->len += len;
            scan     += 2; // Skip over %o
        } else {
            /// format with vsnprintf
            const char* next_percent = strchr(scan, '%');
            num segment_len = next_percent ? (num)(next_percent - scan) : (num)strlen(scan);
            reserve(res, segment_len);
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
                        reserve(res, res->alloc << 1);
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
    if (f && field) {
        object fvalue = get(log_fields, field); // make get be harmless to map; null is absolutely fine identity wise to understand that
        if (!fvalue) return null;
        verify(fvalue, "map integrity failure");
        bool b = fvalue ? cast(bool, fvalue) : false;
        if (!b) return null; /// need to guard for this in string opt callers
        string ff = instanceof(fvalue, string);
        
        /// if logging is set to a blank string then you simply get all log messages
        /// if logging is set to a bool of true you get a breakpoint
        /// if logging is set to a non-blank string, it breaks at your filter
        if (!ff || (ff->chars[0] && index_of(res, ff->chars) >= 0))
            raise(SIGTRAP);
    }
    if (f == stderr) {
        fwrite("\033[1;33m", 7, 1, f);
    }
    if (f) {
        write(res, f, false);
        if (write_ln) {
            fwrite("\n", 1, 1, f);
            fflush(f);
        }
    }
    /// cereal is pretty available, and we only need this on string and path
    return type ? (object)((A_f*)type)->with_cereal(A_alloc(type, 1, true), res->chars) : (object)res;
}

u64 fnv1a_hash(const void* data, size_t length, u64 hash) {
    const u8* bytes = (const u8*)data;
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];  // xor bottom with current byte
        hash *= FNV_PRIME; // multiply by FNV prime
    }
    return hash;
}

list   list_copy(list a) {
    list  b = new(list);
    for (item i = a->first; i; i = i->next)
        push(b, i->value);
    return b;
}

u64 item_hash(item f) {
    return hash(f->key ? f->key : f->value);
}

num clamp(num i, num mn, num mx) {
    if (i < mn) return mn;
    if (i > mx) return mx;
    return i;
}

real clampf(real i, real mn, real mx) {
    if (i < mn) return mn;
    if (i > mx) return mx;
    return i;
}

void vector_init(vector a);

sz vector_len(vector a) {
    return A_header(a)->count;
}

string string_escape(string input) {
    struct {
        char   ascii;
        symbol escape;
    } escape_map[] = {
        {'\n', "\\n"},
        {'\t', "\\t"},
        {'\"', "\\\""},
        {'\\', "\\\\"},
        {'\r', "\\r"},
    };
    int escape_count = sizeof(escape_map) / sizeof(escape_map[0]);
    int input_len    = strlen(input);
    int extra_space  = 0;
    for (int i = 0; i < input_len; i++)
        for (int j = 0; j < escape_count; j++)
            if (input->chars[i] == escape_map[j].ascii) {
                extra_space += strlen(escape_map[j].escape) - 1;
                break;
            }

    // allocate memory for escaped string with applied space
    cstr escaped = calloc(input_len + extra_space + 1, 1);
    if (!escaped) return NULL;

    // fill escaped string
    int pos = 0;
    for (int i = 0; i < input_len; i++) {
        bool found = false;
        for (int j = 0; j < escape_count; j++) {
            if (input->chars[i] == escape_map[j].ascii) {
                const char *escape_seq = escape_map[j].escape;
                int len = strlen(escape_seq);
                strncpy(escaped + pos, escape_seq, len);
                pos += len;
                found = true;
                break;
            }
        }
        if (!found) escaped[pos++] = input->chars[i];
    }
    escaped[pos] = '\0';    /// null-terminate result
    return string(escaped); /// with cstr constructor, it does not 'copy' but takes over life cycle
}

void  string_destructor(string a)        { free(a->chars); }
num   string_compare(string a, string b) { return strcmp(a->chars, b->chars); }
num   string_cmp(string a, cstr b)       { return strcmp(a->chars, b); }
bool  string_eq(string a, cstr b)        { return strcmp(a->chars, b) == 0; }

string string_copy(string a) {
    return string(a->chars);
}

i32   string_index_num(string a, num index) {
    if (index < 0)
        index += a->len;
    if (index >= a->len)
        return 0;
    return (i32)a->chars[index];
}

array string_split(string a, cstr sp) {
    cstr next = a->chars;
    sz   slen = strlen(sp);
    while (next) {
        cstr   n = strstr(&next[1], sp);
        string v = new(string, chars, next, ref_length, n ? (sz)(1 + next - n) : 0);
        next = n ? n + slen : null;
    }
    return null;
}

void string_alloc_sz(string a, sz alloc) {
    char* chars = calloc(1 + alloc, sizeof(char));
    memcpy(chars, a->chars, sizeof(char) * a->len);
    chars[a->len] = 0;
    //free(a->chars);
    a->chars = chars;
    a->alloc = alloc;
}

string string_mid(string a, num start, num len) {
    return new(string, chars, &a->chars[start], ref_length, len);
}

none  string_reserve(string a, num extra) {
    if (a->alloc - a->len >= extra)
        return;
    string_alloc_sz(a, a->alloc + extra);
}

none  string_append(string a, cstr b) {
    sz blen = strlen(b);
    if (blen + a->len >= a->alloc)
        string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy(&a->chars[a->len], b, blen);
    a->len += blen;
    a->h = 0; /// mutable operations must clear the hash value
    a->chars[a->len] = 0;
}

none  string_append_count(string a, cstr b, i32 blen) {
    if (blen + a->len >= a->alloc)
        string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy(&a->chars[a->len], b, blen);
    a->len += blen;
    a->h = 0; /// mutable operations must clear the hash value
    a->chars[a->len] = 0;
}

none  string_push(string a, u32 b) {
    sz blen = 1;
    if (blen + a->len >= a->alloc)
        string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy(&a->chars[a->len], &b, 1);
    a->len += blen;
    a->h = 0; /// mutable operations must clear the hash value
    a->chars[a->len] = 0;
}

none  string_concat(string a, string b) {
    string_append(a, b->chars);
}

sz string_len(string a) { return a->len; }

num   string_index_of(string a, cstr cs) {
    char* f = strstr(a->chars, cs);
    return f ? (num)(f - a->chars) : (num)-1;
}

bool string_cast_bool(string a) {
    return a->len > 0;
}

sz string_cast_sz(string a) {
    return a->len;
}

cstr string_cast_cstr(string a) {
    return a->chars;
}

none string_write(string a, handle f, bool new_line) {
    FILE* output = f ? f : stdout;
    fwrite(a->chars, a->len, 1, output);
    if (new_line) fwrite("\n", 1, 1, output);
    fflush(output);
}

path string_cast_path(string a) {
    return new(path, chars, a->chars);
}



u64 string_hash(string a) {
    if (a->h) return a->h;
    a->h = fnv1a_hash(a->chars, a->len, OFFSET_BASIS);
    return a->h;
}

void message_init(message a) {
    a->role = strdup(a->role);
    a->content = strdup(a->content);
}

void message_destructor(message a) {
    free(a->role);
    free(a->content);
}

void string_init(string a) {
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

string string_with_i32(string a, uint32_t value) {
    // Check if the value is within the BMP (U+0000 - U+FFFF)
    if (value <= 0xFFFF) {
        a->len = 1;
        a->chars = calloc(8, 1);
        a->chars[0] = (char)value;
    } else {
        // Encode the Unicode code point as UTF-8
        a->len = 0;
        char buf[4];
        int len = 0;
        if (value <= 0x7F) {
            buf[len++] = (char)value;
        } else if (value <= 0x7FF) {
            buf[len++] = 0xC0 | ((value >> 6) & 0x1F);
            buf[len++] = 0x80 | (value & 0x3F);
        } else if (value <= 0xFFFF) {
            buf[len++] = 0xE0 | ((value >> 12) & 0x0F);
            buf[len++] = 0x80 | ((value >> 6) & 0x3F);
            buf[len++] = 0x80 | (value & 0x3F);
        } else if (value <= 0x10FFFF) {
            buf[len++] = 0xF0 | ((value >> 18) & 0x07);
            buf[len++] = 0x80 | ((value >> 12) & 0x3F);
            buf[len++] = 0x80 | ((value >> 6) & 0x3F);
            buf[len++] = 0x80 | (value & 0x3F);
        } else {
            // Invalid Unicode code point
            a->len = 0;
            a->chars = NULL;
            return a;
        }
        a->len = len;
        a->chars = calloc(len + 1, 1);
        memcpy(a->chars, buf, len);
    }
    return a;
}

string string_with_cstr(string a, cstr value) {
    a->len   = value ? strlen(value) : 0;
    a->chars = calloc(a->len + 1, 1);
    memcpy(a->chars, value, a->len);
    return a;
}


string string_with_symbol(string a, symbol value) {
    return string_with_cstr(a, (cstr)value);
}


bool string_starts_with(string a, cstr value) {
    sz ln = strlen(value);
    if (!ln || ln > a->len) return false;
    return strncmp(&a->chars[0], value, ln) == 0;
}

bool string_ends_with(string a, cstr value) {
    sz ln = strlen(value);
    if (!ln || ln > a->len) return false;
    return strcmp(&a->chars[a->len - ln], value) == 0;
}

item list_push(list a, object e);

item hashmap_fetch(hashmap a, object key) {
    u64 h = a->unmanaged ? (u64)((void*)key) : hash(key);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (compare(f->key, key) == 0)
            return f;
    item n = list_push(bucket, null);
    n->key = a->unmanaged ? key : A_hold(key);
    a->count++;
    return n;
}

item hashmap_lookup(hashmap a, object key) {
    u64 h = a->unmanaged ? (u64)((void*)key) : hash(key);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (compare(f->key, key) == 0)
            return f;
    return null;
}

none hashmap_set(hashmap a, object key, object value) {
    item i = fetch(a, key);
    object prev = i->value;
    i->value = a->unmanaged ? value : A_hold(value);
    if (!a->unmanaged) A_drop(prev);
}

object hashmap_get(hashmap a, object key) {
    item i = lookup(a, key);
    return i ? i->value : null;
}

bool hashmap_contains(hashmap a, object key) {
    item i = lookup(a, key);
    return i != null;
}

none hashmap_remove(hashmap a, object key) {
    u64 h = hash(key);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (compare(f->key, key) == 0) {
            list_remove_item(bucket, f);
            a->count--;
            break;
        }
}

bool hashmap_cast_bool(hashmap a) {
    return a->count > 0;
}

object hashmap_index_object(hashmap a, object key) {
    return get(a, key);
}

hashmap hashmap_init(hashmap a) {
    if (!a->alloc)
         a->alloc = 16;
    a->data  = (list)calloc(a->alloc, sizeof(struct _list)); /// we can zero-init a vectorized set of objects with A-type
    return a;
}

string hashmap_cast_string(hashmap a) {
    string res  = new(string, alloc, 1024);
    bool   once = false;
    for (int i = 0; i < a->alloc; i++) {
        list bucket = &a->data[i];
        for (item f = bucket->first; f; f = f->next) {
            string key = cast(string, f->key);
            string val = cast(string, f->value);
            if (once) append(res, " ");
            append(res, key->chars);
            append(res, ":");
            append(res, val->chars);
            once = true;
        }
    }
    return res;
}

void map_concat(map a, map b) {
    pairs(b, e) set(a, e->key, e->value);
}

bool map_rm(map a, object key) {
    item i = lookup(a->hmap, key);
    if (i) {
        list_remove_item(a, i);
        return true;
    }
    return false;
}

item map_fetch(map a, object key) {
    num prev = a->hmap->count;
    item i = fetch(a->hmap, key);
    if (prev != a->hmap->count) {
        pair mi = i->value = new(pair, key, key); // todo: make pair originate in hash; remove the key from item
        mi->ref = push(a, i);
        mi->ref->key = a->unmanaged ? key : A_hold(key);
        mi->ref->value = mi; //
    }
    return i;
}

none map_set(map a, object key, object value) {
    item i  = fetch(a->hmap, key);
    pair mi = i->value;
    if (mi) {
        object before     = mi->value;
        mi->value    = a->unmanaged ? value : A_hold(value);
        if (!a->unmanaged) A_drop(before);
    } else {
        mi = i->value = new(pair, key, key, value, value); // todo: make pair originate in hash; remove the key from item
        mi->ref      = push(a, i);
        mi->ref->key = a->unmanaged ? key : A_hold(key);
        mi->ref->value = mi;
    }
}

object map_get(map a, object key) {
    item i = lookup(a->hmap, key);
    return (i && i->value) ? ((pair)i->value)->value : null;
}

bool map_contains(map a, object key) {
    item i = lookup(a->hmap, key);
    return i != null;
}

/// must store a ref to the ref item in hashmap item's value; we may call this an object map_value
none map_remove(map a, object key) {
    item i = lookup(a->hmap, key);
    if (i) {
        item ref = i->value;
        remove_item(a, ref);
    }
}

bool map_cast_bool(map a) {
    return a->count > 0;
}

void list_quicksort(list a, i32(*sfn)(object, object)) {
    item f = a->first;
    int  n = a->count;
    for (int i = 0; i < n - 1; i++) {
        int jc = 0;
        for (item j = f; jc < n - i - 1; j = j->next, jc++) {
            item j1 = j->next;
            if (sfn(j->value, j1->value) > 0) {
                object t = j->value;
                j ->value = j1->value;
                j1->value = t;
            }
        }
    }
}

void list_sort(list a, ARef fn) {
    list_quicksort(a, (i32(*)(object, object))fn);
}

object list_get(list a, object at_index);

sz map_len(map a) {
    return a->count;
}

object map_index_sz(map a, sz index) {
    assert(index >= 0 && index < a->count, "index out of range");
    item i = list_get(a, A_sz(index));
    return i ? i->value : null;
}

object map_index_object(map a, object key) {
    item i = get(a->hmap, key);
    return i ? i->value : null;
}

map map_with_sz(map a, sz size) {
    a->hsize = size;
    return a;
}

void map_init(map a) {
    if (!a->hsize) a->hsize = 1024;
    a->hmap  = new(hashmap, alloc, a->hsize, unmanaged, a->unmanaged);
}

string map_cast_string(map a) {
    string res  = new(string, alloc, 1024);
    bool   once = false;
    for (item i = a->first; i; i = i->next) {
        pair   kv    = i->value;
        string key   = cast(string, kv->key);
        string value = cast(string, kv->value);
        if (once) append(res, " ");
        append(res, key->chars);
        append(res, ":");
        append(res, value->chars);
        once = true;
    }
    return res;
}

object A_copy(A a) {
    A f = A_header(a);
    assert(f->count > 0, "invalid count");
    AType type = isa(a);
    object b = A_alloc(type, f->count, true);
    memcpy(b, a, f->type->size * f->count);
    A_hold_members(b);
    return b;
}

object A_hold(object a) {
    if (a) {
        A f = A_header(a);
        if (f->refs++ == 1 && f->ar_index > 0)
            af_top->pool->elements[f->ar_index] = null; // index of 0 is occupied by the pool itself; just a sentinel 
    }
    return a;
}

//#undef destructor

void A_free(object a) {
    A       aa = A_header(a);
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

void A_drop(object a) {
    if (a && --A_header(a)->refs == -1)
        A_free(a);
}

object A_data(A instance) {
    A obj = A_header(instance);
    return obj->data;
}

i64 A_data_stride(A a) {
    AType t = A_data_type(a);
    return t->size - (t->traits & A_TRAIT_PRIMITIVE ? 0 : sizeof(void*));
}

AType A_data_type(A a) {
    A f = A_header(a);
    return f->scalar ? f->scalar : f->type;
}

object A_instanceof(object inst, AType type) {
    if (!inst) return null;

    verify(inst, "instanceof given a null value");
    AType t  = type;
    AType it = isa(inst); 
    AType it_copy = it;
    while (it) {
        if (it == t)
            return inst;
        else if (it == typeid(A))
            break;
        it = it->parent_type; 
    }
    return null;
}

/// list -------------------------
item list_push(list a, object e) {
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
num list_index_of(list a, object value) {
    num index = 0;
    for (item ai = a->first; ai; ai = ai->next) {
        if (ai->value == value)
            return index;
        index++;
    }
    return -1;
}

item list_item_of(list a, object value) {
    num index = 0;
    for (item ai = a->first; ai; ai = ai->next) {
        if (ai->value == value) {
            ai->key = A_i64(index);
            return ai;
        }
        index++;
    }
    return null;
}

void list_remove(list a, num index) {
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
}

void list_remove_item(list a, item ai) {
    num i = 0;
    if (ai) {
        if (ai == a->first) a->first = ai->next;
        if (ai == a->last)  a->last  = ai->prev;
        if (ai->prev)       ai->prev->next = ai->next;
        if (ai->next)       ai->next->prev = ai->prev;
        a->count--;
    }
}

num list_compare(list a, list b) {
    num diff  = a->count - b->count;
    if (diff != 0)
        return diff;
    A_f* ai_t = a->first ? isa(a->first->value) : null;
    if (ai_t) {
        type_member_t* m = A_member(ai_t, (A_MEMBER_IMETHOD), "compare");
        for (item ai = a->first, bi = b->first; ai; ai = ai->next, bi = bi->next) {
            num   v  = ((num(*)(object,A))((method_t*)m->method)->address)(ai, bi);
            if (v != 0) return v;
        }
    }
    return 0;
}

object list_pop(list a) {
    item l = a->last;
    a->last = a->last->prev;
    if (!a->last)
        a->first = null;
    l->prev = null;
    a->count--;
    return l;
}

object list_get(list a, object at_index) {
    sz index = 0;
    AType itype = isa(at_index);
    sz at = 0;
    if (itype == typeid(sz)) {
        at = *(sz*)at_index;
    } else {
        assert(itype == typeid(i32), "invalid indexing type");
        at = (sz)*(i32*)at_index;
    }
    for (item i = a->first; i; i = i->next) {
        if (at == index)
            return i->value;
        index++;
    }
    assert(false, "could not fetch item at index %i", at);
    return null;
}

num list_count(list a) {
    return a->count;
}


bool is_meta(object a) {
    AType t = isa(a);
    return t->meta.count > 0;
}

bool is_meta_compatible(object a, object b) {
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

object A_realloc(object a, sz alloc) {
    A   i = A_header(a);
    if (alloc > i->alloc) {
        AType type = data_type(a);
        sz  size  = data_stride(a);
        u8* data  = calloc(alloc, size);
        u8* prev  = i->data;
        memcpy(data, prev, i->count * size);
        i->data  = data;
        i->alloc = alloc;
        if ((object)prev != a) free(prev);
    }
    return i->data;
}

object A_resize(object a, sz count) {
    A_realloc(a, count);
    A f = A_header(a);
    f->count = count;
    return f->data;
}

void vector_init(vector a) {
    A f = A_header(a);
    f->count = 0;
    f->scalar = f->type->meta.meta_0 ? f->type->meta.meta_0 : a->type ? a->type : typeid(i8);
    verify(f->scalar, "scalar not set");
    A_realloc(a, a->alloc);
}

none vector_with_path(vector a, path file_path) {
    A f = A_header(a);
    f->scalar = typeid(i8);
    
    verify(exists(file_path), "file %o does not exist", file_path);
    FILE* ff = fopen(cstring(file_path), "rb");
    fseek(ff, 0, SEEK_END);
    sz flen = ftell(ff);
    fseek(ff, 0, SEEK_SET);

    A_realloc(a, flen);
    f->count = flen;
    size_t n = fread(f->data, 1, flen, ff);
    verify(n == flen, "could not read file: %o", f);
    fclose(ff);
}

void vector_realloc(vector a, sz size) {
    A_realloc(a, size);
}

void vector_resize(vector a, sz size) {
    A_resize(a, size);
}

void vector_concat(vector a, ARef any, num count) {
    if (count <= 0) return;
    AType type = data_type(a);
    A f = A_header(a);
    if (f->alloc < f->count + count)
        realloc(a, (a->alloc << 1) + 32 + count);
    
    u8* ptr  = data(a);
    i64 size = data_stride(a);
    memcpy(&ptr[f->count * size], any, size * count);
    f->count += count;
}

void vector_push(vector a, A any) {
    vector_concat(a, any, 1);
}

num abso(num i) { 
    return (i < 0) ? -i : i;
}

vector vector_slice(vector a, num from, num to) {
    A      f   = A_header(a);
    /// allocate the data type (no i8 bytes)
    num count = (1 + abso(from - to)); // + 31 & ~31;
    object res = A_alloc(f->type, 1, true);
    A      res_f = A_header(res);

    /// allocate 
    u8* src   = f->data;
    u8* dst   = A_valloc(f->type, f->scalar, count, count, true);
    i64 stride = data_stride(a);
    if (from <  to)
        memcpy(dst, &src[from * stride], count * stride);
    else
        for (int i = from; i > to; i--, dst++)
            memcpy(dst, &src[i * stride], count * stride);
    res_f->data = dst;
    A_initialize(res);
    return res;
}

sz vector_count(vector a) {
    A f = A_header(a);
    return f->count;
}

define_class(vector);

map map_of(cstr first_key, ...) {
    map a = new(map, hsize, 16);
    va_list args;
    va_start(args, first_key);
    cstr key = first_key;
    for (;;) {
        A arg = va_arg(args, A);
        set(a, string(key), arg);
        key = va_arg(args, cstr);
        if (key == null)
            break;
    }
    return a;
}




object subprocedure_invoke(subprocedure a, object arg) {
    object(*addr)(object, object, object) = a->addr;
    return addr(a->target, arg, a->ctx);
}

void AF_init(AF a) {
    af_top = a;
    a->pool = allocate(array, alloc, a->start_size ? a->start_size : 1024);
    push_weak(a->pool, a); // push self to pool, now all indices are > 0; obviously we dont want to free this though

    if (!af_stack) af_stack = allocate(array, alloc, 16);
    push_weak(af_stack, a);
}

AF AF_create(sz start_size) {
    AF a = allocate(AF, start_size, start_size);
    //AF_init(a);
    return a;
}

AF A_pool(sz start_size) {
    return AF_create(start_size);
}

void AF_destructor(AF a) {
    int f = index_of(af_stack, a);
    assert(f >= 0, "invalid af-stack index");
    remove_weak(af_stack, f);
    if (af_top == a)
        af_top = af_stack->len ? af_stack->elements[af_stack->len - 1] : null;
    for (int i = 1; i < a->pool->len; i++) {
        A ref = a->pool->elements[i];
        if (ref) {
            A_free(ref);
        }
    }
    A_free(a->pool);
    a->pool = null;
}

AF AF_fetch(num index) {
    if (af_stack && abso((int)index) < af_stack->len)
        return index < 0 ? af_stack->elements[af_stack->len + index] :
                           af_stack->elements[index];
    else
        return null;
}

bool isname(char n) {
    return (n >= 'a' && n <= 'z') || (n >= 'A' && n <= 'Z') || (n == '_');
}


u64 fn_hash(fn f) {
    return (u64)f->method->address;
}

object fn_call(fn f, array args) {
    return A_method_call(f->method, args);
}

bool create_symlink(path target, path link) {
    bool is_err = symlink(target, link) == -1;
    return !is_err;
}

void file_init(file f) {
    verify(!(f->read && f->write), "cannot open for both read and write");
    cstr src = f->src ? f->src->chars : null;
    if (!f->id && (f->read || f->write)) {
        verify (src || f->write, "can only create temporary files for write");

        if (!src) {
            i64    h      = 0;
            bool   exists = false;
            string r      = null;
            path   p      = null;
            do {
                h   = (i64)rand() << 32 | (i64)rand();
                r   = formatter("/tmp/f%p", (void*)h);
                src = r->chars;
                p   = new(path, chars, r);
            } while (exists(p));
        }
        f->id = fopen(src, f->read ? "rb" : "wb");
        if (!f->src)
             f->src = new(path, chars, src);
    }
}

bool file_cast_bool(file f) {
    return f->id != null;
}

string file_gets(file f) {
    char buf[2048];
    if (fgets(buf, 2048, f->id) > 0)
        return string(buf);
    return null;
}

bool file_write(file f, object o) {
    AType type = isa(o);
    if (type == typeid(string)) {
        u16 nbytes    = ((string)o)->len;
        u16 le_nbytes = htole16(nbytes);
        fwrite(&le_nbytes, 2, 1, f->id);
        f->size += (num)nbytes;
        return fwrite(((string)o)->chars, 1, nbytes, f->id) == nbytes;
    }
    sz size = isa(o)->size;
    f->size += (num)size;
    verify(type->traits & A_TRAIT_PRIMITIVE, "not a primitive type");
    return fwrite(o, size, 1, f->id) == 1;
}



object file_read(file f, AType type) {
    if (type == typeid(string)) {
        char bytes[65536];
        u16  nbytes;
        if (f->text_mode) {
            verify(fgets(bytes, sizeof(bytes), f->id), "could not read text");
            return string(bytes); 
        }
        verify(fread(&nbytes, 2, 1, f->id) == 1, "failed to read byte count");
        nbytes = le16toh(nbytes);
        f->location += nbytes;
        verify(nbytes < 1024, "overflow");
        verify(fread(bytes, 1, nbytes, f->id) == nbytes, "read fail");
        bytes[nbytes] = 0;
        return string(bytes); 
    }
    object o = A_alloc(type, 1, true);
    sz size = isa(o)->size;
    f->location += size;
    verify(type->traits & A_TRAIT_PRIMITIVE, "not a primitive type");
    bool success = fread(o, size, 1, f->id) == 1;
    return success ? o : null;
}

void file_close(file f) {
    if (f->id) {
        fclose(f->id);
        f->id = null;
    }
}

void file_destructor(file f) {
    file_close(f);
}

none path_init(path a) {
    cstr arg = a->chars;
    num  len = arg ? strlen(arg) : 0;
    a->chars = calloc(len + 1, 1);
    if (arg)
        memcpy(a->chars, arg, len + 1);
}

path path_temp(symbol tmpl) {
    path p = null;
    do {
        i64    h = (i64)rand() << 32 | (i64)rand();
        string r = formatter("/tmp/%p.%s", (void*)h, tmpl);
        p        = path(chars, r->chars);
    } while (exists(p));
    return p;
}

path path_with_string(path a, string s) {
    a->chars = copy_cstr(s->chars);
    return a;
}

bool path_cast_bool(path a) {
    return a->chars && strlen(a->chars) > 0;
}

sz path_cast_sz(path a) {
    return strlen(a->chars);
}

cstr path_cast_cstr(path a) {
    return a->chars;
}

string path_cast_string(path a) {
    return new(string, chars, a->chars);
}

path path_with_cereal(path a, cereal cs) {
    a->chars = copy_cstr((cstr)cs);
    return a;
}

path path_with_cstr(path a, cereal cs) {
    a->chars = copy_cstr((cstr)cs);
    return a;
}

bool path_move(path a, path b) {
    return rename(a->chars, b->chars) == 0;
}

bool path_make_dir(path a) {
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

bool path_is_dir(path a) {
    DIR   *dir = opendir(a->chars);
    if (dir == NULL)
        return false;
    closedir(dir);
    return true;
}


bool path_is_empty(path a) {
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

string path_ext(path a) {
    for (int i = strlen(a->chars) - 1; i >= 0; i--)
        if (a->chars[i] == '.')
            return str(&a->chars[i + 1]);
    return str(null);
}

string path_stem(path a) {
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

string path_filename(path a) {
    cstr cs  = a->chars; /// this can be a bunch of folders we need to make in a row
    sz   len = strlen(cs);
    string res = new(string, alloc, 256);
    for (num i = len - 1; i >= 0; i--) {
        if (cs[i] == '/' || i == 0) {
            cstr start = &cs[i + (cs[i] == '/')];
            int n_bytes = len - i;
            memcpy(res->chars, start, n_bytes);
            res->len = n_bytes;
            break;
        }
    }
    return res;
}

path path_absolute(path a) {
    path  result   = new(path);
    cstr  rpath    = realpath(a->chars, null);
    result->chars  = rpath ? strdup(rpath) : copy_cstr("");
    return result;
}


path path_directory(path a) {
    path result = new(path);
    result->chars = dirname(strdup(a->chars));
    return result;
}

path path_parent(path a) {
    int len = strlen(a->chars);
    for (int i = len - 2; i >= 0; i--) { /// -2 because we dont mind the first /
        char ch = a->chars[i];
        if  (ch == '/') {
            string trim = new(string, chars, a->chars, ref_length, i);
            return new(path, chars, trim->chars);
        }
    }
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

path path_change_ext(path a, cstr ext) {
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

/// public statics are not 'static'
path path_cwd(sz size) {
    path a = new(path);
    a->chars = calloc(size, 1);
    char* res = getcwd(a->chars, size);
    assert(res != null, "getcwd failure");
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

object path_read(path a, AType type) {
    FILE* f = fopen(a->chars, "rb");
    if (!f) return null;
    bool is_obj = type && !(type->traits & A_TRAIT_PRIMITIVE);
    if (type == typeid(string) || is_obj) {
        fseek(f, 0, SEEK_END);
        sz flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        string a = new(string, alloc, flen);
        size_t n = fread(a->chars, 1, flen, f);
        fclose(f);
        assert(n == flen, "could not read enough bytes");
        a->len   = flen;
        if (!is_obj)
            return a;
        object obj = parse(a->chars, type);
        return obj;
    }
    assert(false, "not implemented");
    return null;
}

void* primitive_ffi_arb(AType ptype) {
    if (ptype == typeid(u8))        return &ffi_type_uint8;
    if (ptype == typeid(i8))        return &ffi_type_sint8;
    if (ptype == typeid(u16))       return &ffi_type_uint16;
    if (ptype == typeid(i16))       return &ffi_type_sint16;
    if (ptype == typeid(u32))       return &ffi_type_uint32;
    if (ptype == typeid(i32))       return &ffi_type_sint32;
    if (ptype == typeid(u64))       return &ffi_type_uint64;
    if (ptype == typeid(i64))       return &ffi_type_sint64;
    if (ptype == typeid(f32))       return &ffi_type_float;
    if (ptype == typeid(f64))       return &ffi_type_double;
    if (ptype == typeid(f128))      return &ffi_type_longdouble;
    if (ptype == typeid(AMember))   return &ffi_type_sint32;
    if (ptype == typeid(bool))      return &ffi_type_uint32;
    if (ptype == typeid(num))       return &ffi_type_sint64;
    if (ptype == typeid(sz))        return &ffi_type_sint64;
    if (ptype == typeid(none))      return &ffi_type_void;
    return &ffi_type_pointer;
}


#define MAX_PATH_LEN 4096

array path_ls(path a, string pattern, bool recur) {
    cstr base_dir = a->chars;
    assert(path_is_dir(a), "ls: must be called on directory");
    array list = new(array, alloc, 32); // Initialize array for storing paths
    DIR *dir = opendir(base_dir);
    char abs[MAX_PATH_LEN];
    struct dirent *entry;
    struct stat statbuf;

    assert (dir, "opendir");

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(abs, sizeof(abs), "%s/%s", base_dir, entry->d_name);

        if (stat(abs, &statbuf) == 0) {
            if (S_ISREG(statbuf.st_mode)) {
                if (!pattern->len || strstr(abs, pattern->chars))
                    push(list, new(path, chars, abs));
                
            } else if (S_ISDIR(statbuf.st_mode)) {
                if (recur) {
                    path subdir = new(path, chars, abs);
                    array sublist = ls(subdir, pattern, recur);
                    concat(list, sublist);
                }
            }
        }
    }

    closedir(dir);
    return list;
}

// -----------------------------------------------------------
// vec generics (f, f64, and i8)
// -----------------------------------------------------------
mat4f new_mat4f(f32* x0) {
    mat4f r = A_alloc(typeid(mat4f), 1, true);
    AType  type = typeid(mat4f);
    if (x0)
        memcpy(r->m, x0, sizeof(f32) * type->vmember_count);
    else {
        r->m[4 * 0 + 0] = 1.0f;
        r->m[4 * 1 + 1] = 1.0f;
        r->m[4 * 2 + 2] = 1.0f;
        r->m[4 * 3 + 3] = 1.0f;
    }
    return r;
}

mat3f new_mat3f(f32* x0) {
    mat3f r = A_alloc(typeid(mat3f), 1, true);
    AType  type = typeid(mat3f);
    if (x0)
        memcpy(r->m, x0, sizeof(f32) * type->vmember_count);
    else {
        r->m[3 * 0 + 0] = 1.0f;
        r->m[3 * 1 + 1] = 1.0f;
        r->m[3 * 2 + 2] = 1.0f;
    }
    return r;
}

mat2f new_mat2f(f32* x0) {
    mat2f r = A_alloc(typeid(mat2f), 1, true);
    AType  type = typeid(mat2f);
    if (x0)
        memcpy(r->m, x0, sizeof(f32) * type->vmember_count);
    else {
        r->m[2 * 0 + 0] = 1.0f;
        r->m[2 * 1 + 1] = 1.0f;
    }
    return r;
}

bool    matf_cast_bool   (matf a) {
    AType  type = isa(a);
    for (int i = 0; i < type->vmember_count; i++)
        if (a->m[i] != 0.0f) return true;
    return false;
}

quatf new_quatf(f32 x, f32 y, f32 z, f32 w) {
    quatf r = A_alloc(typeid(quatf), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}

vec2i8 new_vec2i8(i8 x, i8 y) {
    vec2i8 r = A_alloc(typeid(vec2i8), 1, true);
    r->x = x;
    r->y = y;
    return r;
}

vec3i8 new_vec3i8(i8 x, i8 y, i8 z) {
    vec3i8 r = A_alloc(typeid(vec3i8), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    return r;
}

vec4i8 new_vec4i8(i8 x, i8 y, i8 z, i8 w) {
    vec4i8 r = A_alloc(typeid(vec4i8), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}

vec2i64 new_vec2i64(i64 x, i64 y) {
    vec2i64 r = A_alloc(typeid(vec2i64), 1, true);
    r->x = x;
    r->y = y;
    return r;
}

vec3i64 new_vec3fi64(i64 x, i64 y, i64 z) {
    vec3i64 r = A_alloc(typeid(vec3i64), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    return r;
}

vec4i64 new_vec4i64(i64 x, i64 y, i64 z, i64 w) {
    vec4i8 r = A_alloc(typeid(vec4i8), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}

vec2f new_vec2f(float x, float y) {
    vec2f r = A_alloc(typeid(vec2f), 1, true);
    r->x = x;
    r->y = y;
    return r;
}

vec3f new_vec3f(float x, float y, float z) {
    vec3f r = A_alloc(typeid(vec3f), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    return r;
}

vec4f new_vec4f(float x, float y, float z, float w) {
    vec4f r = A_alloc(typeid(vec4f), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}




vec2f64 new_vec2f64(f64 x, f64 y) {
    vec2f64 r = A_alloc(typeid(vec2f64), 1, true);
    r->x = x;
    r->y = y;
    return r;
}


vec3i64 new_vec3i64(i64 x, i64 y, i64 z) {
    vec3i64 r = A_alloc(typeid(vec3i64), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    return r;
}

vec3f64 new_vec3f64(f64 x, f64 y, f64 z) {
    vec3f64 r = A_alloc(typeid(vec3f64), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    return r;
}


vec4f64 new_vec4f64(f64 x, f64 y, f64 z, f64 w) {
    vec4f64 r = A_alloc(typeid(vec4f64), 1, true);
    r->x = x;
    r->y = y;
    r->z = z;
    r->w = w;
    return r;
}

vecf vecf_cross(vecf a, vecf b) {
    vec3f result = vec3f(
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x);
    return result;
}

vecf vecf_normalize(vecf a) {
    f32   len_sq = 0.0f;
    AType type   = isa(a);
    for (int i = 0; i < type->vmember_count; i++)
        len_sq += (&a->x)[i] * (&a->x)[i];
    f32   len    = sqrtf(len_sq);
    
    if (len > 0) {
        vecf res = A_copy(a);
        for (int i = 0; i < type->vmember_count; i++)
            (&res->x)[i] /= len;
        return res;
    }
    return a;
}

vecf vecf_mix(vecf a, vecf b, f64 f) {
    vecf   res  = A_copy(a);
    f32*   fres = &res->x;
    AType  type = isa(a);
    verify(type == isa(b) || instanceof(a, vecf) || instanceof(b, vecf),
        "vector mismatch"); // allow for generics
    for (int i = 0; i < type->vmember_count; i++) {
        f32 fa  = (&a->x)[i];
        f32 fb  = (&b->x)[i];
        fres[i] = fa * (1.0f - f) + fb * f;
    }
    return res;
}

f64 vecf_dot(vecf a, vecf b) {
    f64  r = 0.0;
    AType  type = isa(a);
    for (int i = 0; i < type->vmember_count; i++)
        r += (&a->x)[i] * (&b->x)[i];
    return r;
}

f64 vecf_length(vecf a) {
    f64 r = 0.0;
    AType  type = isa(a);
    for (int i = 0; i < type->vmember_count; i++)
        r += (&a->x)[i] * (&a->x)[i];
    return sqrt(r);
}

vec2f   vecf_cast_vec2f  (vecf a) { return vec2f  (a->x, a->y); }
vec3f   vecf_cast_vec3f  (vecf a) { return vec3f  (a->x, a->y, a->z); }
vec4f   vecf_cast_vec4f  (vecf a) { return vec4f  (a->x, a->y, a->z, a->w); }
vec2f64 vecf_cast_vec2f64(vecf a) { return vec2f64(a->x, a->y); }
vec3f64 vecf_cast_vec3f64(vecf a) { return vec3f64(a->x, a->y, a->z); }
vec4f64 vecf_cast_vec4f64(vecf a) { return vec4f64(a->x, a->y, a->z, a->w); }
vec2i8  vecf_cast_vec2i8 (vecf a) { return vec2i8 (a->x, a->y); }
vec3i8  vecf_cast_vec3i8 (vecf a) { return vec3i8 (a->x, a->y, a->z); }
vec4i8  vecf_cast_vec4i8 (vecf a) { return vec4i8 (a->x, a->y, a->z, a->w); }

bool    vecf_cast_bool   (vecf a) {
    AType  type = isa(a);
    for (int i = 0; i < type->vmember_count; i++)
        if ((&a->x)[i] != 0.0f) return true;
    return false;
}



vecf64 vecf64_cross(vecf64 a, vecf64 b) {
    vec3f64 result = vec3f64(
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x);
    return result;
}

vecf64 vecf64_normalize(vecf64 a) {
    f32   len_sq = 0.0f;
    AType type   = isa(a);
    for (int i = 0; i < type->vmember_count; i++)
        len_sq += (&a->x)[i] * (&a->x)[i];
    f32   len    = sqrtf(len_sq);
    
    if (len > 0) {
        vecf64 res = A_copy(a);
        for (int i = 0; i < type->vmember_count; i++)
            (&res->x)[i] /= len;
        return res;
    }
    return a;
}

vecf64 vecf64_mix(vecf64 a, vecf64 b, f64 f) {
    vecf64   res  = A_copy(a);
    f32*   fres = &res->x;
    AType  type = isa(a);
    verify(type == isa(b) || instanceof(a, vecf64) || instanceof(b, vecf64),
        "vector mismatch"); // allow for generics
    for (int i = 0; i < type->vmember_count; i++) {
        f32 fa  = (&a->x)[i];
        f32 fb  = (&b->x)[i];
        fres[i] = fa * (1.0f - f) + fb * f;
    }
    return res;
}

f64 vecf64_dot(vecf64 a, vecf64 b) {
    AType type = isa(a);
    f64  r = 0.0f;
    for (int i = 0; i < type->vmember_count; i++) r += (&a->x)[i] * (&b->x)[i];
    return r;
}

f64 vecf64_length(vecf64 a) {
    AType type = isa(a);
    float  r = 0.0f;
    for (int i = 0; i < type->vmember_count; i++) r += (&a->x)[i] * (&a->x)[i];
    return sqrtf(r);
}

vec2f64 vecf64_cast_vec2f  (vecf64 a) { return vec2f64(a->x, a->y); }
vec3f64 vecf64_cast_vec3f64(vecf64 a) { return vec3f64(a->x, a->y, a->z); }
vec4f64 vecf64_cast_vec4f64(vecf64 a) { return vec4f64(a->x, a->y, a->z, a->w); }
bool  vecf64_cast_bool (vecf64 a) {
    AType type = isa(a);
    for (int i = 0; i < type->vmember_count; i++)
        if ((&a->x)[i] != 0.0f) return true;
    return false;
}

veci8 veci8_cross(veci8 a, veci8 b) {
    vec3i8 result = vec3i8(
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x
    );
    return result;
}

veci8 veci8_normalize(veci8 a) {
    i32 len_sq = 0;
    AType type = isa(a);

    for (int i = 0; i < type->vmember_count; i++)
        len_sq += (&a->x)[i] * (&a->x)[i];

    if (len_sq > 0) {
        veci8 res = A_copy(a);
        for (int i = 0; i < type->vmember_count; i++)
            (&res->x)[i] = (&a->x)[i] * 255 / len_sq;
        return res;
    }
    return a;
}

veci8 veci8_mix(veci8 a, veci8 b, f64 ff) {
    i32 f = ff * 255;
    veci8 res = A_copy(a);
    i8* fres = &res->x;
    AType type = isa(a);

    verify(
        type == isa(b) || instanceof(a, veci8) || instanceof(b, veci8),
        "vector mismatch"
    );

    for (int i = 0; i < type->vmember_count; i++) {
        i8 fa = (&a->x)[i];
        i8 fb = (&b->x)[i];
        fres[i] = (fa * (255 - f) + fb * f) / 255;
    }
    return res;
}

f64 veci8_dot(veci8 a, veci8 b) {
    AType type = isa(a);
    i32 r = 0;
    for (int i = 0; i < type->vmember_count; i++)
        r += (&a->x)[i] * (&b->x)[i];
    return r;
}

f64 veci8_length(veci8 a) {
    AType type = isa(a);
    i32 len_sq = 0;
    for (int i = 0; i < type->vmember_count; i++)
        len_sq += (&a->x)[i] * (&a->x)[i];
    return sqrt((f32)len_sq);
}

vec2i8 veci8_cast_vec2i8(veci8 a) { return vec2i8(a->x, a->y); }
vec3i8 veci8_cast_vec3i8(veci8 a) { return vec3i8(a->x, a->y, a->z); }
vec4i8 veci8_cast_vec4i8(veci8 a) { return vec4i8(a->x, a->y, a->z, a->w); }

bool veci8_cast_bool(veci8 a) {
    AType type = isa(a);
    for (int i = 0; i < type->vmember_count; i++) {
        if ((&a->x)[i] != 0) return true;
    }
    return false;
}







veci64 veci64_cross(veci64 a, veci64 b) {
    vec3i64 result = vec3i64(
        a->y * b->z - a->z * b->y,
        a->z * b->x - a->x * b->z,
        a->x * b->y - a->y * b->x
    );
    return result;
}

veci64 veci64_normalize(veci64 a) {
    i32 len_sq = 0;
    AType type = isa(a);

    for (int i = 0; i < type->vmember_count; i++)
        len_sq += (&a->x)[i] * (&a->x)[i];

    if (len_sq > 0) {
        veci64 res = A_copy(a);
        for (int i = 0; i < type->vmember_count; i++)
            (&res->x)[i] = (&a->x)[i] * 255 / len_sq;
        return res;
    }
    return a;
}

veci64 veci64_mix(veci64 a, veci64 b, f64 ff) {
    i32 f = ff * 255;
    veci64 res = A_copy(a);
    i64* fres = &res->x;
    AType type = isa(a);

    verify(
        type == isa(b) || instanceof(a, veci64) || instanceof(b, veci64),
        "vector mismatch"
    );

    for (int i = 0; i < type->vmember_count; i++) {
        i64 fa = (&a->x)[i];
        i64 fb = (&b->x)[i];
        fres[i] = (fa * (255 - f) + fb * f) / 255;
    }
    return res;
}

f64 veci64_dot(veci64 a, veci64 b) {
    AType type = isa(a);
    i32 r = 0;
    for (int i = 0; i < type->vmember_count; i++)
        r += (&a->x)[i] * (&b->x)[i];
    return r;
}

f64 veci64_length(veci64 a) {
    AType type = isa(a);
    i32 len_sq = 0;
    for (int i = 0; i < type->vmember_count; i++)
        len_sq += (&a->x)[i] * (&a->x)[i];
    return sqrt((f32)len_sq);
}

vec2i64 veci64_cast_vec2i64(veci64 a) { return vec2i64(a->x, a->y); }
vec3i64 veci64_cast_vec3i64(veci64 a) { return vec3i64(a->x, a->y, a->z); }
vec4i64 veci64_cast_vec4i64(veci64 a) { return vec4i64(a->x, a->y, a->z, a->w); }

bool veci64_cast_bool(veci64 a) {
    AType type = isa(a);
    for (int i = 0; i < type->vmember_count; i++) {
        if ((&a->x)[i] != 0) return true;
    }
    return false;
}


/// total vector length, shape and all (or its count otherwise if its 1D)
i64 vlength(object a) {
    A     header = A_header(a);
    AType type = header->type;
    A     header_shape = header->shape ? A_header(header->shape) : null;
    if (!header_shape)
        return header->count;
    i64  product = 1;
    i64* shape_data = header_shape->data;
    for (int i = 0; i < header_shape->count; i++) product *= shape_data[i];
    return product;
}

/// shape takes precedence here
i64 veci64_product(veci64 a) {
    A aa = A_header(a);
    i64* data  = aa->data;
    i64  count = vlength(a);
    i64  product = 1;
    for (int i = 0; i < count; i++)
        product *= data[i];
    return product;
}


shape new_shape(i64 size, ...) {
    va_list args;
    va_start(args, size);
    i64 dim_count = 0;
    for (i64 arg = size; arg; arg = va_arg(args, i64))
        dim_count++;
    
    va_start(args, size);
    i64* res = calloc(sizeof(i64), dim_count);
    i64  index = 0;
    for (i64 arg = size; arg; arg = va_arg(args, i64))
        res[index++] = arg;

    return A_vec(typeid(i64), dim_count, res);
}


object A_mat(AType type, i32 rows, i32 cols, object vdata) {
    object res = A_alloc2(type, typeid(f32), veci64_of(rows, cols, 0), true);
    // we must set the rows and columns on a field in object
    A f = A_header(res);

    if (vdata)
        memcpy(res, vdata, rows * cols * type->size);
    return res;
}

object A_vec(AType type, i32 count, object vdata) {
    //AType type = typeid(veci64);
    veci64 res = A_alloc(type, count, true);
    if (vdata)
        memcpy(res, vdata, count * type->size);
    return res;
}

none shape_set(object obj, i64* dim, i64 dim_count) {
    A a = A_header(obj);
    if (dim_count > 1) {
        veci64 shape = A_vec(typeid(i64), dim_count, dim);
        a->alloc = 1;
        for (int i = 0; i < dim_count; i++)
            a->alloc *= dim[i];
    } else {
        a->alloc = dim ? dim[0] : 1; // unit or default is 1
    }
}

matf matf_operator__mul(matf mat, vec4f v) {
    return vec4f(1.0f, 1.0f, 1.0f, 1.0f);
}

/// vec4f treated as axis x/y/z + theta (w) args
none quatf_with_vec4f(quatf q, vec4f v) {
    f32   theta        = v->w;
    f32   half_theta   = theta * 0.5f;
    f32   s_half_theta = sinf(half_theta);
    q->x = v->x * s_half_theta;
    q->y = v->y * s_half_theta;
    q->z = v->z * s_half_theta;
    q->w = cosf(half_theta);
}

void mat2f_with_floats(matf mat, floats f) {
    memcpy(mat->m, f, sizeof(float) * 2 * 2);
}

void mat3f_with_floats(matf mat, floats f) {
    memcpy(mat->m, f, sizeof(float) * 3 * 3);
}

void mat4f_with_floats(matf mat, floats f) {
    memcpy(mat->m, f, sizeof(float) * 4 * 4);
}

void mat4f_with_quatf(matf mat, quatf q) {
    /// values are at mat->values[0...15] [ row-major ]
    f32 x = q->x, y = q->y, z = q->z, w = q->w;
    f32 xx = x * x;
    f32 yy = y * y;
    f32 zz = z * z;
    f32 xy = x * y;
    f32 xz = x * z;
    f32 yz = y * z;
    f32 wx = w * x;
    f32 wy = w * y;
    f32 wz = w * z;

    // Fill matrix values in row-major order
    mat->m[0]  = 1.0f - 2.0f * (yy + zz); // Row 1, Col 1
    mat->m[1]  = 2.0f * (xy - wz);        // Row 1, Col 2
    mat->m[2]  = 2.0f * (xz + wy);        // Row 1, Col 3
    mat->m[3]  = 0.0f;                    // Row 1, Col 4

    mat->m[4]  = 2.0f * (xy + wz);        // Row 2, Col 1
    mat->m[5]  = 1.0f - 2.0f * (xx + zz); // Row 2, Col 2
    mat->m[6]  = 2.0f * (yz - wx);        // Row 2, Col 3
    mat->m[7]  = 0.0f;                    // Row 2, Col 4

    mat->m[8]  = 2.0f * (xz - wy);        // Row 3, Col 1
    mat->m[9]  = 2.0f * (yz + wx);        // Row 3, Col 2
    mat->m[10] = 1.0f - 2.0f * (xx + yy); // Row 3, Col 3
    mat->m[11] = 0.0f;                    // Row 3, Col 4

    mat->m[12] = 0.0f;                    // Row 4, Col 1
    mat->m[13] = 0.0f;                    // Row 4, Col 2
    mat->m[14] = 0.0f;                    // Row 4, Col 3
    mat->m[15] = 1.0f;                    // Row 4, Col 4
}

i64 matf_rows(matf a) {
    AType f = isa(a);
    return (&f->vmember_shape->x)[0];
}

i64 matf_cols(matf a) {
    AType f = isa(a);
    return (&f->vmember_shape->x)[1];
}

matf matf_mul(matf a, matf b) {
    // lets multiply mat (rows(mat), cols(mat)) * b (b.rows, b.cols)
    i64 rows_a = matf_rows(a);
    i64 cols_a = matf_cols(a);
    i64 rows_b = matf_rows(b);
    i64 cols_b = matf_cols(b);

    // Validate matrix dimensions
    if (cols_a != rows_b) {
        fault("a-columns and b-rows do not match for multiplication: (%ix%i) * (%ix%i)\n",
            (int)rows_a, (int)cols_a, (int)rows_b, (int)cols_b);
    }

    // Create the result matrix
    AType mtype = typeid(matf);
    if (rows_a == 4 && cols_b == 4) mtype = typeid(mat4f);
    matf result = A_mat(mtype, rows_a, cols_b, null);

    // Perform multiplication
    for (i64 i = 0; i < rows_a; ++i) {
        for (i64 j = 0; j < cols_b; ++j) {
            result->m[i * cols_b + j] = 0; // Initialize element
            for (i64 k = 0; k < cols_a; ++k) {
                result->m[i * cols_b + j] += a->m[i * cols_a + k] * b->m[k * cols_b + j];
            }
        }
    }

    return result;
}

vec4f matf_mul_v4(matf a, vec4f b) {
    i64 rows_a = matf_rows(a);
    i64 cols_a = matf_cols(a);

    // Validate vector size
    if (cols_a != 4)
        fault("columns (%i) do not match vector size (4)\n", (int)cols_a);

    vec4f result = vec4f(0, 0, 0, 0);

    // Perform multiplication
    for (i64 i = 0; i < rows_a; ++i) {
        for (i64 j = 0; j < 4; ++j) {
            (&result->x)[i] += a->m[i * cols_a + j] * (&b->x)[j];
        }
    }

    return result;
}

matf matf_scale(matf a, vecf factors) {
    matf res = A_copy(a);
    u32 size = isa(a)->vmember_count;
    for (u32 i = 0; i < size; ++i)
        res->m[i * (size + 1)] = (&factors->x)[i];
    return res;
}

// any 'shape' in A-type model applies on top of vmember_count
matf matf_translate(matf a, vecf offsets) {
    matf res = A_copy(a);
    u32 size = isa(a)->vmember_count;
    i64 rows = matf_rows(a);
    i64 cols = matf_cols(a);
    for (u32 i = 0; i < rows - 1; ++i)
        res->m[i * cols + (cols - 1)] = (&offsets->x)[i];
    return res;
}

matf matf_rotate(matf mat, quatf q) {
    matf res = A_copy(mat);
    u32 size = isa(mat)->vmember_count;
    verify(size == 16, "rotation currently supports only 4x4 matrices");

    // quaternion rotation
    f32 x = q->x, y = q->y, z = q->z, w = q->w;
    f32 xx = x * x, yy = y * y, zz = z * z;
    f32 xy = x * y, xz = x * z, yz = y * z;
    f32 wx = w * x, wy = w * y, wz = w * z;

    res->m[0]  = 1.0f - 2.0f * (yy + zz);
    res->m[1]  = 2.0f * (xy - wz);
    res->m[2]  = 2.0f * (xz + wy);
    res->m[3]  = 0.0f;

    res->m[4]  = 2.0f * (xy + wz);
    res->m[5]  = 1.0f - 2.0f * (xx + zz);
    res->m[6]  = 2.0f * (yz - wx);
    res->m[7]  = 0.0f;

    res->m[8]  = 2.0f * (xz - wy);
    res->m[9]  = 2.0f * (yz + wx);
    res->m[10] = 1.0f - 2.0f * (xx + yy);
    res->m[11] = 0.0f;

    res->m[12] = 0.0f;
    res->m[13] = 0.0f;
    res->m[14] = 0.0f;
    res->m[15] = 1.0f;
    return res;
}





// idea:
// silver should: swap = and : ... so : is const, and = is mutable-assign
// we serialize our data with : and we do not think of this as a changeable form, its our data and we want it intact, lol
// serialize object into json
string json(object a) {
    AType  type  = isa(a);
    string res   = string(alloc, 1024);
    /// start at 1024 pre-alloc
    if (!a) {
        append(res, "null");
    } else if (instanceof(a, array)) {
        // array with items
        push(res, '[');
        each (a, object, i) concat(res, json(i));
        push(res, ']');
    } else if (!(type->traits & A_TRAIT_PRIMITIVE)) {
        // object with fields
        push(res, '{');
        bool one = false;
        for (num m = 0; m < type->member_count; m++) {
            if (one) push(res, ',');
            type_member_t* mem = &type->members[m];
            if (!(mem->member_type & (A_MEMBER_PROP | A_MEMBER_INLAY))) continue;
            string name = string(mem->name);
            concat(res, json(name));
            push  (res, ':');
            object value = A_get_property(a, mem->name);
            concat(res, json(value));
            one = true;
        }
        push(res, '}');
    } else {
        A_serialize(type, res, a);
    }
    return res;
}

string parse_json_string(cstr origin, cstr* remainder) {
    if (*origin != '\"') return null;
    string res = string(alloc, 64);
    cstr scan;
    for (scan = &origin[1]; *scan;) {
        if (*scan == '\"') {
            scan++;
            break;
        }
        if (*scan == '\\') {
            scan++;
            if (*scan == 'n') push(res, 10);
            else if (*scan == 'r') push(res, 13);
            else if (*scan == 't') push(res,  9);
            else if (*scan == 'b') push(res,  8);
            else if (*scan == '/') push(res, '/');
            else if (*scan == 'u') {
                // Read the next 4 hexadecimal digits and compute the Unicode codepoint
                uint32_t code = 0;
                for (int i = 0; i < 4; i++) {
                    scan++;
                    char c = *scan;
                    if      (c >= '0' && c <= '9') code = (code << 4) | (c - '0');
                    else if (c >= 'a' && c <= 'f') code = (code << 4) | (c - 'a' + 10);
                    else if (c >= 'A' && c <= 'F') code = (code << 4) | (c - 'A' + 10);
                    else
                        fault("Invalid Unicode escape sequence");
                }
                // Convert the codepoint to UTF-8 encoded bytes
                if (code <= 0x7F) {
                    push(res, (i8)code);
                } else if (code <= 0x7FF) {
                    push(res, (i8)(0xC0 | (code >> 6)));
                    push(res, (i8)(0x80 | (code & 0x3F)));
                } else if (code <= 0xFFFF) {
                    push(res, (i8)(0xE0 | ( code >> 12)));
                    push(res, (i8)(0x80 | ((code >> 6) & 0x3F)));
                    push(res, (i8)(0x80 | ( code       & 0x3F)));
                } else if (code <= 0x10FFFF) {
                    push(res, (i8)(0xF0 | ( code >> 18)));
                    push(res, (i8)(0x80 | ((code >> 12) & 0x3F)));
                    push(res, (i8)(0x80 | ((code >> 6)  & 0x3F)));
                    push(res, (i8)(0x80 | ( code        & 0x3F)));
                } else {
                    fault("Unicode code point out of range");
                }
            }
        } else
            push(res, *scan);
        scan++;
    }
    *remainder = scan;
    return res;
}

object parse_array(cstr s, AType schema, cstr* remainder);

object parse_object(cstr input, AType schema, cstr* remainder) {
    cstr   scan   = ws(input);
    cstr   origin = null;
    object res    = null;
    char  *endptr;

    if (remainder)
       *remainder = null;

    if (strncmp(scan, "true", 4) == 0 || strncmp(scan, "false", 5) == 0) {
        verify(!schema || schema == typeid(bool), "type mismatch");
        bool is_true = strncmp(scan, "true", 4) == 0;
        if (is_true)
            *remainder = scan + 4;
        else
            *remainder = scan + 5;
        return A_bool(is_true); 
    }
    else if (*scan == '[')
        return parse_array (scan, schema, remainder);
    
    else if ((*scan >= '0' && *scan <= '9') || *scan == '-') {
        origin = scan;
        int has_dot = 0;
        while (*++scan) {
            has_dot += *scan == '.';
            if (*scan != '.' && !(*scan >= '0' && *scan <= '9'))
                break;
        }
        if (has_dot || (schema && schema->traits & A_TRAIT_REALISTIC)) {
            if (schema == typeid(i64)) {
                double v = strtod(origin, &scan);
                res = A_i64((i64)floor(v));
            }
            else if (schema == typeid(f32))
                res = A_f32(strtof(origin, &scan));
            else
                res = A_f64(strtod(origin, &scan));
        } else
            res = A_i64(strtoll(origin, &scan, 10));
    }
    else if (*scan == '"' || *scan == '\'') {
        origin = scan;
        res = construct_with(schema ? schema : typeid(string), parse_json_string(origin, &scan));
    }
    else if (*scan == '{') {
        AType use_schema = schema ? schema : typeid(map);
        bool is_map = use_schema == typeid(map);
        map required = null;
        if (!is_map) {
            /// required fields
            required = map();
            for (int i = 0; i < use_schema->member_count; i++) {
                type_member_t* mem = use_schema->members;
                if (mem->required) {
                    set(required, string(mem->name), A_bool(true)); // field-name
                }
            }
        }
        scan = ws(&scan[1]);
        map props = map(hsize, 16);
        for (;;) {
            if (*scan == '}') {
                scan++;
                break;
            }
            scan = ws(&scan[0]);
            if (*scan != '\"') return null;
            origin        = scan;
            string name   = parse_json_string(origin, &scan);
            Member member = is_map ? null : A_member(use_schema, A_MEMBER_PROP, name->chars);
            if (!member && !is_map) {
                print("property '%o' not found in type: %s", name, use_schema->name);
                return null;
            }
            scan = ws(&scan[0]);
            if (*scan != ':') return null;
            scan = ws(&scan[1]);
            if (required && contains(required, name)) {
                rm(required, name);
            }
            object value = parse_object(scan, member ? member->type : null, &scan); /// issue with parsing a map type (attributes)
            AType type = isa(value);
            scan = ws(&scan[0]);
            if   (!value)
                return null;
            string s_value = value;
            set(props, name, value);
            if (*scan == ',') {
                scan++;
                continue;
            } else if (*scan != '}')
                return null;
        }
        /// check for remaining required
        if (required && len(required)) {
            string r = string();
            pairs(required, i) {
                r = form(string, "%o%s%o", r, len(r) ? " " : "", i->key);
            }
            fault("required fields not set: %o", r); // in public case, no required fields
        }
        if (remainder) *remainder = ws(scan);
        res = construct_with(use_schema, props);
    }
    if (remainder) *remainder = scan;
    return res;
}

array parse_array_objects(cstr* s, AType element_type) {
    cstr scan = *s;
    array res = array(alloc, 64);
    for (;;) {
        if (scan[0] == ']') {
            scan = ws(&scan[1]);
            break;
        }
        object a = parse_object(scan, element_type, &scan);
        push(res, a);
        scan = ws(scan);
        if (scan[0] == ',') {
            scan = ws(&scan[1]);
            continue;
        }
    }
    *s = scan;
    return res;
}

object parse_array(cstr s, AType schema, cstr* remainder) {
    cstr scan = ws(s);
    verify(*scan == '[', "expected array '['");
    scan = ws(&scan[1]);
    object res = null;
    if (!schema || schema->src == typeid(array)) {
        AType element_type = schema ? schema->meta.meta_0 : typeid(object);
        res = parse_array_objects(&scan, element_type);
    } else if (schema == typeid(veci64) || schema->src == typeid(veci64)) {
        array arb = parse_array_objects(&scan, typeid(i64));
        int vcount = len(arb);
        res = A_alloc2(schema, typeid(i64), veci64_of(vcount, 0), true);
        int n = 0;
        each(arb, object, a) {
            verify(isa(a) == typeid(i64), "expected i64");
            ((i64*)res)[n] = *(i64*)a;
        }
    } else if (schema      == typeid(mat4f) || schema      == typeid(quatf) || 
               schema->src == typeid(matf)  || schema->src == typeid(vecf)) {
        array arb = parse_array_objects(&scan, typeid(f32));
        int vcount = len(arb);
        res = A_alloc2(schema, typeid(f32), veci64_of(vcount, 0), true);
        int n = 0;
        each(arb, object, a) {
            AType a_type = isa(a);
            if (a_type == typeid(i64))      ((f32*)res)[n++] =  (float)*(i64*)a;
            else if (a_type == typeid(f32)) ((f32*)res)[n++] = *(float*)a;
            else if (a_type == typeid(f64)) ((f32*)res)[n++] =  (float)*(double*)a;
            else fault("unexpected type");
        }
        
    } else {
        fault("unhandled vector type: %s", schema ? schema->name : null);
    }
    if (remainder) *remainder = scan;
    return res;
}

// root will apply to objects contained within an array, or a top level object
object parse(cstr s, AType schema) {
    return parse_object(s, schema, null);
}

define_class(quatf)

define_class(message)

define_vector_base(mat, f32,  f)

define_vector_base(vec, i8,  i8)
define_vector_base(vec, i64, i64)
define_vector_base(vec, f32, f)
define_vector_base(vec, f64, f64)

define_vector(mat, 4, f32, f,   4, 4)
define_vector(mat, 3, f32, f,   3, 3)
define_vector(mat, 2, f32, f,   2, 2)

define_vector(vec, 2, i8,  i8,  2)
define_vector(vec, 2, i64, i64, 2)
define_vector(vec, 2, f32, f,   2)
define_vector(vec, 2, f64, f64, 2)

define_vector(vec, 3, i8,  i8,  2)
define_vector(vec, 3, i64, i64, 2)
define_vector(vec, 3, f32, f,   2)
define_vector(vec, 3, f64, f64, 2)

define_vector(vec, 4, i8,  i8,  4, 4)
define_vector(vec, 4, i64, i64, 4, 4)
define_vector(vec, 4, f32, f,   4, 4)
define_vector(vec, 4, f64, f64, 4, 4)

define_class(A)
define_meta(object, A, A)

define_abstract(numeric)
define_abstract(string_like)
define_abstract(nil)
define_abstract(raw)
define_abstract(ref)
define_abstract(imported)

define_primitive( u8,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_UNSIGNED)
define_primitive(u16,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_UNSIGNED)
define_primitive(u32,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_UNSIGNED)
define_primitive(u64,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_UNSIGNED)
define_primitive( i8,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_SIGNED)
define_primitive(i16,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_SIGNED)
define_primitive(i32,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_SIGNED)
define_primitive(i64,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_SIGNED)
define_primitive(bool,   numeric, A_TRAIT_INTEGRAL | A_TRAIT_UNSIGNED)
define_primitive(num,    numeric, A_TRAIT_INTEGRAL | A_TRAIT_SIGNED)
define_primitive(sz,     numeric, A_TRAIT_INTEGRAL | A_TRAIT_SIGNED)
define_primitive(f32,    numeric, A_TRAIT_REALISTIC)
define_primitive(f64,    numeric, A_TRAIT_REALISTIC)
define_primitive(f128,   numeric, A_TRAIT_REALISTIC)
define_primitive(AMember, numeric, A_TRAIT_INTEGRAL | A_TRAIT_UNSIGNED)
define_primitive(cstr,   string_like, 0)
define_primitive(symbol, string_like, 0)
define_primitive(cereal, string_like, 0)
define_primitive(none,   nil, 0)
define_primitive(AType,  raw, 0)
define_primitive(handle, raw, 0)
define_primitive(Member, raw, 0)
define_primitive(ARef,   ref, 0)
define_primitive(floats, raw, 0)

define_enum(OPType)
define_enum(Exists)
define_enum(level)

define_class(path)
define_class(file)
define_class(string)

define_class(item)
//define_proto(collection) -- disabling for now during reduction to base + class + mod
define_class(list, object)
define_class(array, object)
define_class(hashmap)
define_class(pair)
define_mod(map, list) // never quite did one like this but it does make sense
define_class(fn)
define_class(subprocedure)

define_class(AF)

define_meta(ATypes,           array, AType)
define_meta(array_map,        array, map)
define_meta(array_mat4f,      array, mat4f)
define_meta(array_mat3f,      array, mat3f)
define_meta(array_mat2f,      array, mat2f)
define_meta(array_string,     array, string)