#include <A-intern>
#include <A>
#include <A-init>
#include <A-methods>
#include <A-reserve>
#undef realloc
//#include <ffi.h>
#undef USE_FFI
#undef bool
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <endian-cross.h>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <cpuid.h>
#endif
#include <math.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/inotify.h>




#ifndef line
#define line(...)       new(line,       __VA_ARGS__)
#endif

object parse(cstr s, AType schema, ctx context);

i64 epoch_millis() {
    struct timeval tv;
    gettimeofday(&tv, null);
    return (i64)(tv.tv_sec) * 1000 + (i64)(tv.tv_usec) / 1000;
}

shape shape_with_array(shape a, array dims) {
    num count = len(dims);
    each (dims, object, e) {
        i64* i = (i64*)instanceof(e, i64);
        a->data[a->count++] = *i;
    }
    return a;
}

i64 shape_total(shape a) {
    i64* data = a->data;
    i64 total = 1;
    for (int i = 0; i < a->count; i++)
        total *= data[i];
    return total;
}

i64 shape_compare(shape a, shape b) {
    if (a->count != b->count)
        return a->count - b->count;
    for (int i = 0; i < a->count; i++)
        if (a->data[i] != b->data[i])
            return a->data[i] - b->data[i];
    return 0;
}

shape shape_from(i64 count, i64* values) {
    shape res = shape(count, count);
    memcpy(res->data, values, sizeof(i64) * count);
    return res;
}

shape shape_read(FILE* f) {
    i32 n_dims;
    i64 data[256];
    verify(fread(&n_dims, sizeof(i32), 1, f) == 1, "n_dims");
    verify(n_dims < 256, "invalid");
    verify(fread(data, sizeof(i64), n_dims, f) == n_dims, "shape_read data");
    shape res = shape(count, n_dims);
    memcpy(res->data, data, sizeof(i64) * n_dims);
    return res;
}

shape shape_new(i64 size, ...) {
    va_list args;
    va_start(args, size);
    i64 n_dims = 0;
    for (i64 arg = size; arg; arg = va_arg(args, i64))
        n_dims++;
    
    va_start(args, size);
    shape res = shape(count, n_dims);
    i64  index = 0;
    for (i64 arg = size; arg; arg = va_arg(args, i64))
        res->data[index++] = arg;
    return res;
}

define_class(shape, A)

/// block-chain will go here in quanta

__thread array     af_stack;
__thread   AF      af_top;

static global_init_fn* call_after;
static num             call_after_alloc;
static num             call_after_count;
static map             log_funcs;

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

none A_lazy_init(global_init_fn fn) {
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

none A_module_init(bool(*fn)()) {
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

none A_push_type(AType type) {
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

AType A_find_type(symbol name) {
    for (int i = 0; i < types_len; i++) {
        AType type = types[i];
        //print("type: %s", type->name);
        if (strcmp(type->name, name) == 0)
            return type;
    }
    return null;
}

object A_new(AType type) {
    object res = A_alloc(type, 1);
    // todo: do not call init in A_alloc
    return res;
}

u128 AF_bits(object a) {
    AType type = isa(a);
    u128 fields = *(u128*)((i8*)a + type->size - (sizeof(void*) * 2));
    return fields;
}

void AF_set_id(object a, int id) {
    AType t = isa(a);
    u128* fields = (u128*)((i8*)a + t->size - (sizeof(void*) * 2));
    *fields |= ((u128)1) << id;
}

void AF_set_name(object a, cstr name) {
    AType t = isa(a);
    type_member_t* m = A_member(t, A_MEMBER_PROP|A_MEMBER_VPROP, name, true);
    u128* fields = (u128*)((i8*)a + t->size - (sizeof(void*) * 2));
    *fields |= ((u128)1) << m->id;
}

bool AF_query_name(object a, cstr name) {
    AType          t = isa(a);
    type_member_t* m = A_member(t, A_MEMBER_PROP|A_MEMBER_VPROP, name, true);
    u128           f = AF_bits(a);
    return (f & (((u128)1) << m->id)) != 0;
}

static bool A_validator(object a) {
    AType type = isa(a);
    if (type->magic != 1337) return false;

    // now required args are set if (type->required & *(i64*)obj->f) == type->required
    u128 fields = AF_bits(a);
    if ((type->required & fields) != type->required) {
        for (num i = 0; i < type->member_count; i++) {
            type_member_t* m = &type->members[i];
            if (m->required && ((((u128)1) << m->id) & fields) == 0) {
                u8* ptr = (u8*)a + m->offset;
                object*  ref = (object*)ptr;
                fault("required arg [%s] not set for class %s",
                    m->name, type->name);
            }
        }
        exit(2);
    }
    return true;
}

/// some changes being made to these, as enums can now be any primitive type
/// we'll re-cast from i32 later; ptr is the VAL stored in static memory as the typed data in global constructor
/// we must do this with our usage of static const; thats not always addressable
i32* A_enum_default(AType type) {
    for (num m = 0; m < type->member_count; m++) {
        type_member_t* mem = &type->members[m];
        if (mem->member_type & A_MEMBER_ENUMV)
            return (i32*)mem->ptr;
    }
    return null;
}

static object enum_member_value(AType type, type_member_t* mem) {
    if (type->src == typeid(u8))  return A_u8(*(u8*)mem->ptr);
    if (type->src == typeid(i8))  return A_i8(*(i8*)mem->ptr);
    if (type->src == typeid(u16)) return A_u16(*(u16*)mem->ptr);
    if (type->src == typeid(i16)) return A_i16(*(i16*)mem->ptr);
    if (type->src == typeid(u32)) return A_u32(*(u32*)mem->ptr);
    if (type->src == typeid(i32)) return A_i32(*(i32*)mem->ptr);
    if (type->src == typeid(u64)) return A_u64(*(u64*)mem->ptr);
    if (type->src == typeid(i64)) return A_i64(*(i64*)mem->ptr);
    if (type->src == typeid(f32)) return A_f32(*(f32*)mem->ptr);
    fault("implement enum conversion: %s", type->name);
    return null;
}

i32* A_enum_value(AType type, cstr cs) {
    int cur = 0;
    int default_val = INT_MIN;
    bool single = strlen(cs) == 1;
    for (num m = 0; m < type->member_count; m++) {
        type_member_t* mem = &type->members[m];
        if ((mem->member_type & A_MEMBER_ENUMV) &&
            (strcmp(mem->name, cs) == 0)) {
            return (i32*)enum_member_value(type, mem);
        }
    }
    for (num m = 0; m < type->member_count; m++) {
        type_member_t* mem = &type->members[m];
        if ((mem->member_type & A_MEMBER_ENUMV) &&
            (mem->name[0] == cs[0])) {
            return (i32*)enum_member_value(type, mem);
        }
    }
    fault("enum not found");
    return null;
}

string A_enum_string(AType type, i32* value) {
    for (num m = 0; m < type->member_count; m++) {
        type_member_t* mem = &type->members[m];
        if (mem->member_type & A_MEMBER_ENUMV) {
            if (memcmp(mem->ptr, value, mem->type->size) == 0)
                return mem->sname ? mem->sname : string(mem->name); 
        }
    }
    // better to fault than default
    fault ("invalid enum-value of %i for type %s", value, type->name);
    return null;
}

none debug() {
    return;
}


none A_init_recur(object a, AType current, raw last_init) {
    if (current == &A_type) return;
    none(*init)(object) = ((A_f*)current)->init;
    A_init_recur(a, current->parent_type, (raw)init);
    if (init && init != (none*)last_init) init(a); 
}

none A_hold_members(object);


// we need a bit of type logic in here for these numerics; it should mirror the C compiler
numeric numeric_operator__add(numeric a, numeric b) {
    AType type_a = isa(a);
    AType type_b = isa(b);
    
    if (type_a == type_b) {
        if (type_a == typeid(i8))  return  A_i8(* (i8*)a + * (i8*)b);
        if (type_a == typeid(i16)) return A_i16(*(i16*)a + *(i16*)b);
        if (type_a == typeid(i32)) return A_i32(*(i32*)a + *(i32*)b);
        if (type_a == typeid(i64)) return A_i64(*(i64*)a + *(i64*)b);
        if (type_a == typeid(f32)) return A_f32(*(f32*)a + *(f32*)b);
        if (type_a == typeid(f64)) return A_f64(*(f64*)a + *(f64*)b);
    } else {
        fault("implement dislike add");
    }
    return null;
}

numeric numeric_operator__sub(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__mul(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__div(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__or(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__and(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__xor(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__mod(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__right(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__left(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__compare(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__equal(numeric a, numeric b) {
    return null;
}

numeric numeric_operator__not_equal(numeric a, numeric b) {
    return null;
}

none    numeric_operator__assign(numeric a, numeric b) {
    AType type_a = isa(a);
    AType type_b = isa(b);
    num sz = type_a->size < type_b->size ? type_a->size : type_b->size;
    memcpy(a, b, sz);
}

object A_initialize(object a) {
    A   f = A_header(a);
    if (f->type->traits & A_TRAIT_USER_INIT) return a; // in ux, we call our own method on 'mount'
    // this may be skipped with macro generators, but it would still be broken in new() -- so its wise to keep this

    #ifndef NDEBUG
    A_validator(a);
    #endif

    A_init_recur(a, f->type, null);
    A_hold_members(a);
    return a;
}


pid_t last_pid = 0;

pid_t A_last_pid() {
    return last_pid;
}

int A_exec(string cmd) {
    if (starts_with(cmd, "export ")) {
        string a = mid(cmd, 7, len(cmd) - 7);
        int i = index_of(a, "=");
        assert(i >= 1, "invalid syntax"); 
        string var   = trim(mid(a, 0, i));
        string value = trim(mid(a, i + 1, len(a) - i - 1));
        setenv((cstr)var->chars, (cstr)value->chars, 1);
        print("export: setting %o to %o", var, value);
        return 0;
    }

    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();

    if (pid == 0) {
        // child
        setpgid(0, 0);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]); // close write
        execlp("sh", "sh", "-s", null); // or "bash"
        _exit(0);
    } else if (pid > 0) {
        last_pid = pid;
        // parent
        close(pipefd[0]); // close read
        FILE *out = fdopen(pipefd[1], "w");
        cstr verbose = getenv("VERBOSE");
        if (verbose && strcmp(verbose, "0") != 0) {
            printf("----------------------\n");
            printf("%s\n", cstring(cmd));
        }
        fprintf(out, "%s\n", cstring(cmd));
        fflush(out);
        close(pipefd[1]);

        int status;
        int result;
        do {
            result = waitpid(pid, &status, 0);
        } while (result == -1 && errno == EINTR);
        last_pid = 0;

        if (result == -1) {
            perror("waitpid");
            return -123456;
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "child terminated by signal: %s\n", strsignal(WTERMSIG(status)));
            return -128 - WTERMSIG(status);  // POSIX style
        }
        fprintf(stderr, "unknown termination\n");
        return -9999;
    } else {
        perror("fork");
        return -1234;
    }
}

static int all_type_alloc;

int A_alloc_count() {
    return all_type_alloc;
}

//static cstr alloc_src = null;
//static int  alloc_line = 0;

A A_alloc_instance(AType type, int n_bytes, int recycle_size) {
    A a = null;
    af_recycler* af = type->af;
    bool use_recycler = false; //af && n_bytes == recycle_size;

    if (use_recycler && af->re_count) {
        a = af->re[--af->re_count];
        memset(a, 0, n_bytes);
    } else {
        type->global_count++;
        all_type_alloc++;

        if (type->traits & A_TRAIT_PUBLIC)
            a = mmap(NULL, n_bytes,
                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        else
            a = calloc(1, n_bytes);
        a->recycle = use_recycler;
    }
    a->refs = 1;
    if (use_recycler) {
        if ((af->af_count + 1) >= af->af_alloc) {
            i64 next_size = af->af_alloc << 1;
            if (next_size == 0) next_size = 1024;
            af->af = realloc(af->af, next_size * sizeof(A*));
            af->af[0] = 0x00;
            af->af_alloc = next_size;
        }
        a->af_index = 1 + af->af_count;
        af->af[a->af_index] = a;
        af->af_count++;
    }
    return a;
}

object A_alloc_dbg(AType type, num count, cstr source, int line) {
    //alloc_src  = source;
    //alloc_line = line;
    sz map_sz = sizeof(map);
    sz A_sz   = sizeof(struct _A);
    A a = A_alloc_instance(type,
        A_sz + type->size * count, A_sz + type->size);
    a->type       = type;
    a->data       = &a[1];
    a->count      = count;
    a->alloc      = count;
    a->source     = source;
    a->line       = line;
    return a->data; /// return fields (object)
}

object A_alloc(AType type, num count) {
    sz map_sz = sizeof(map);
    sz A_sz   = sizeof(struct _A);
    object a = A_alloc_instance(type,
        A_sz + type->size * count, A_sz + type->size);
    a->refs       = 0;
    a->type       = type;
    a->data       = &a[1];
    a->count      = count;
    a->alloc      = count;
    return a->data; /// return fields (object)
}

object A_alloc2(AType type, AType scalar, shape s) {
    i64 A_sz      = sizeof(struct _A);
    shape_ft* t2 = &shape_type;
    i64 count     = total(s);
    object a      = A_alloc_instance(type,
        A_sz + scalar->size * count, A_sz + scalar->size);
    a->refs       = 0;
    a->scalar     = scalar;
    a->type       = type;
    a->data       = &a[1];
    a->shape      = A_hold(s);
    a->count      = count;
    a->alloc      = count;
    return a->data; /// return fields (object)
}


/// array -------------------------
none array_alloc_sz(array a, sz alloc) {
    object* elements = (object*)calloc(alloc, sizeof(struct _A*));
    memcpy(elements, a->elements, sizeof(struct _A*) * a->len);
    
    free(a->elements);
    a->elements = elements;
    a->alloc = alloc;
}

none array_fill(array a, object f) {
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

none array_expand(array a) {
    num alloc = 32 + (a->alloc << 1);
    array_alloc_sz(a, alloc);
}

none array_push_weak(array a, object b) {
    if (a->alloc == a->len) array_expand(a);
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

none array_push(array a, object b) {
    if (a->alloc == a->len) {
        array_expand(a);
    }
    AType t = isa(a);
    AType vtype = isa(b);
    A info = head(a);
    verify(!a->last_type || a->last_type == vtype || a->assorted,
        "unassorted array received differing type: %s, previous: %s (%s:%i)",
        vtype->name, a->last_type->name, info->source, info->line);
    a->last_type = vtype;

    if (is_meta(a) && t->meta.meta_0 != typeid(object))
        assert(is_meta_compatible(a, b), "not meta compatible");
    a->elements[a->len++] = a->unmanaged ? b : hold(b);
}

none array_clear(array a) {
    if (!a->unmanaged)
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

none array_remove(array a, num b) {
    for (num i = b; i < a->len; i++) {
        object prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
        A_drop(prev);
    }
    a->elements[--a->len] = null;
}

none array_remove_weak(array a, num b) {
    for (num i = b; i < a->len; i++) {
        object prev = a->elements[b];
        a->elements[b] = a->elements[b + 1];
    }
    a->elements[--a->len] = null;
}

none array_operator__assign_add(array a, object b) {
    array_push(a, b);
}

none array_operator__assign_sub(array a, num b) {
    array_remove(a, b);
}

object array_first(array a) {
    assert(a->len, "no items");
    return a->elements[0];
}

object array_last(array a) {
    assert(a->len, "no items");
    return a->elements[a->len - 1];
}

none array_push_symbols(array a, ...) {
    va_list args;
    va_start(args, a);
    char* value;
    while ((value = va_arg(args, char*)) != null) {
        string s = new(string, chars, value);
        push(a, s);
    }
    va_end(args);
}

none array_push_objects(array a, A f, ...) {
    va_list args;
    va_start(args, f);
    A value;
    while ((value = va_arg(args, A)) != null)
        push(a, value);
    va_end(args);
}

array array_of(object first, ...) {
    array a = new(array, alloc, 32, assorted, true);
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
    array a = alloc(array, alloc, 256);
    va_list args;
    va_start(args, first);
    for (cstr arg = first; arg; arg = va_arg(args, A))
        push(a, string(chars, arg));
    return a;
}

none  array_weak_push(array a, object obj) {
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

object array_peek(array a, num i) {
    if (i < 0 || i >= a->len)
        return null;
    return a->elements[i];
}

array array_mix(array a, array b, f32 f) {
    int ln0 = len(a);
    int ln1 = len(b);
    if (ln0 != ln1) return b;

    type_member_t* fmix = null;
    AType expect = null;
    array res = array(ln0);
    for (int i = 0; i < ln0; i++) {
        object aa = a->elements[i];
        object bb = b->elements[i];

        AType at = isa(aa);
        AType bt = isa(bb);

        if (!expect) expect = at;
        verify(expect == at, "disperate types in array during mix");
        verify(at == bt, "types do not match");

        if (!fmix) fmix = A_member(at, A_MEMBER_IMETHOD, "mix", false);
        verify(fmix, "implement mix method for type %s", at->name);
        object e = ((mix_fn)fmix->ptr)(aa, bb, f);
        push(res, e);
    }
    return res;
}

object array_get(array a, num i) {
    if (i < 0 || i >= a->len)
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
    if (a->unmanaged) {
        for (num i = 0; i < a->len; i++) {
            if (a->elements[i] == b)
                return i;
        }
    } else {
        for (num i = 0; i < a->len; i++) {
            if (compare(a -> elements[i], b) == 0)
                return i;
        }
    }

    return -1;
}

none br() {
    usleep(0);
}

bool array_cast_bool(array a) { return a && a->len > 0; }

none array_init(array a) {
    if (a->alloc)
        array_alloc_sz(a, a->alloc);
}

none array_dealloc(array a) {
    clear(a);
    free(a->elements);
    a->elements = null;
}

#ifdef USE_FFI
method_t* method_with_address(handle address, AType rtype, array atypes, AType method_owner) {
    const num max_args = (sizeof(meta_t) - sizeof(num)) / sizeof(AType);
    method_t* method = calloc(1, sizeof(method_t));
    method->ffi_cif  = calloc(1,        sizeof(ffi_cif));
    method->ffi_args = calloc(max_args, sizeof(ffi_type*));


    //_Generic(("hi"), string_schema(string, GENERICS) const none *: (none)0)("hi")  


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
#endif

/// should work on statics or member functions the same; its up to the caller to push the self in there
fn A_lambda(object target, Member member, object context) {
    fn f = new(fn,
        method,  member->method,
        target,  target,
        context, context);
    return f;
}


#ifdef USE_FFI
object A_method_call(method_t* a, array args) {
    const num max_args = 8;
    none* arg_values[max_args];
    assert(args->len == a->atypes->len, "arg count mismatch");
    for (num i = 0; i < args->len; i++) {
        A_f* arg_type = (A_f*)a->atypes->elements[i];
        arg_values[i] = (arg_type->traits & (A_TRAIT_PRIMITIVE | A_TRAIT_ENUM)) ? 
            (none*)args->elements[i] : (none*)&args->elements[i];
    }
    none* result[8]; /// enough space to handle all primitive data
    ffi_call((ffi_cif*)a->ffi_cif, a->address, result, arg_values);
    if (a->rtype->traits & A_TRAIT_PRIMITIVE)
        return A_primitive(a->rtype, result);
    else if (a->rtype->traits & A_TRAIT_ENUM)
        return A_enum(a->rtype, *(i32*)result);
    else
        return (object) result[0];
}
#endif

/// this calls type methods
object A_method(AType type, cstr method_name, array args) {
#ifdef USE_FFI
    type_member_t* mem = A_member(type, A_MEMBER_IMETHOD | A_MEMBER_SMETHOD, method_name, false);
    assert(mem->method, "method not set");
    method_t* m = mem->method;
    object res = A_method_call(m, args);
    return res;
#else
    return null;
#endif
}

object A_convert(AType type, object input) {
    if (type == isa(input)) return input;
    return construct_with(type, input, null);
}

object A_method_vargs(object instance, cstr method_name, int n_args, ...) {
#ifdef USE_FFI
    AType type = isa(instance);
    type_member_t* mem = A_member(type, A_MEMBER_IMETHOD | A_MEMBER_SMETHOD, method_name, false);
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
#else
    return null;
#endif
}


int fault_level;

static __attribute__((constructor)) bool Aglobal_AF();

static bool started = false;

none A_start(symbol argv[]) {
    AType f32_type = typeid(f32);
    if (started) return;

    int argc    = 0;
    while (argv[argc]) argc++;
    started     = true;
    fault_level = level_err;
    log_funcs   = hold(map(hsize, 32, unmanaged, true));

    /// initialize logging; * == all
    bool explicit_listen = false;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            string s = string(argv[i + 1]);
            array  a = split(s, ",");
            each(a, string, s) {
                set(log_funcs, eq(s, "all") ? string("*") : s, A_bool(true));
                explicit_listen = true;
            }
        }
    }
 
#ifndef NDEBUG
    if (!explicit_listen)
        set(log_funcs, string("*"), A_bool(true));
#endif

    if (len(log_funcs)) {
        string topics = string(alloc, 32);
        pairs(log_funcs, i) {
            if (len(topics) > 0)
                append(topics, ", ");
            concat(topics, (string)i->key);
        }
        printf("listening-to: %s\n", topics->chars);
    }

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
            if (mem->name)
                mem->sname = hold(string(mem->name));
            
            if (mem->required && (mem->member_type & A_MEMBER_PROP)) {
                type->required |= 1 << mem->id;
                // now required args are set if (type->required & *(i64*)obj->f) == type->required
            }
            if (mem->member_type & (A_MEMBER_IMETHOD | A_MEMBER_SMETHOD)) {
                none* address = 0;
                memcpy(&address, &((u8*)type)[mem->offset], sizeof(none*));
                assert(address, "no address");
#ifdef USE_FFI
                array args = alloc(array, alloc, mem->args.count);
                for (num i = 0; i < mem->args.count; i++)
                    args->elements[i] = (object)((A_f**)&mem->args.meta_0)[i];
                args->len = mem->args.count;
                mem->method = method_with_address(address, mem->type, args, type);
#endif
            }
        }
    }
    path_app((cstr)argv[0]);
}


struct mutex_t {
    pthread_mutex_t lock;
    pthread_cond_t  cond;
};

none mutex_init(mutex m) {
    m->mtx = calloc(sizeof(struct mutex_t), 1);
    pthread_mutex_init(&m->mtx->lock, null);
    if (m->cond) pthread_cond_init(&m->mtx->cond, null);
}

none mutex_dealloc(mutex m) {
    pthread_mutex_destroy(&m->mtx->lock);
    if (m->cond) pthread_cond_destroy(&m->mtx->cond);
    free(m->mtx);
}

none mutex_lock(mutex m) {
    pthread_mutex_lock(&m->mtx->lock);
}

none mutex_unlock(mutex m) {
    pthread_mutex_unlock(&m->mtx->lock);
}

none mutex_cond_broadcast(mutex m) {
    pthread_cond_broadcast(&m->mtx->lock);
}

none mutex_cond_signal(mutex m) {
    pthread_cond_signal(&m->mtx->lock);
}

none mutex_cond_wait(mutex m) {
    pthread_cond_wait(&m->mtx->cond, &m->mtx->lock);
}


map A_args(cstrs argv, symbol default_arg, ...) {
    int argc = 0;
    while (argv[argc]) argc++;
    va_list  args;
    va_start(args, default_arg);
    symbol arg = default_arg;
    map    defaults = new(map, assorted, true);
    while (arg) {
        object val = va_arg(args, object);
        set(defaults, str(arg), hold(val));
        arg = va_arg(args, symbol);
    }
    va_end(args);
    return A_arguments(argc, argv, defaults,
        default_arg ? str(default_arg) : null);
}

none A_tap(symbol f, hook sub) {
    string fname = str(f);
    set(log_funcs, fname, sub ? (object)sub : (object)A_bool(true)); /// if subprocedure, then it may receive calls for the logging
}

none A_untap(symbol f) {
    string fname = str(f);
    set(log_funcs, fname, A_bool(false));
}

type_member_t* A_member(AType type, enum A_MEMBER member_type, symbol name, bool poly) {
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        if ((member_type == 0) || (mem->member_type & member_type) && strcmp(mem->name, name) == 0)
            return mem;
    }
    if (poly && type->parent_type && type->parent_type != typeid(A))
        return A_member(type->parent_type, member_type, name, true);
    return 0;
}

bool A_is_inlay(type_member_t* m) {
    return (m->type->traits & A_TRAIT_VECTOR    |
            m->type->traits & A_TRAIT_STRUCT    | 
            m->type->traits & A_TRAIT_PRIMITIVE | 
            m->type->traits & A_TRAIT_ENUM      | 
            m->member_type == A_MEMBER_INLAY) != 0;
}

none A_hold_members(object instance) {
    AType type = isa(instance);
    while (type != typeid(A)) {
        for (num i = 0; i < type->member_count; i++) {
            type_member_t* mem = &type->members[i];
            object   *mdata = (object*)((cstr)instance + mem->offset);
            if (mem->member_type & (A_MEMBER_PROP | A_MEMBER_PRIV))
                if (!A_is_inlay(mem) && *mdata) { // was trying to isolate what class name was responsible for our problems
                    if (mem->args.meta_0 == typeid(weak))
                        continue;

                    object member_value = *mdata;
                    A head = A_header(member_value);
                    //printf("holding member: %s, of type: %s\n", mem->name, head->type->name);// i
                    head->refs++;
                }
        }
        type = type->parent_type;
    }
}

object A_set_property(object instance, symbol name, object value) {
    AType type = isa(instance);
    type_member_t* m = A_member(type, A_MEMBER_PROP, (cstr)name, true);
    A_member_set(instance, m, value);
    /*
    verify(m, "%s not found on object %s", name, type->name);
    object   *mdata = (object*)((cstr)instance + m->offset);
    AType value_type = isa(value);

    if (m->type->traits & A_TRAIT_STRUCT) {
        A header = A_header(value);
        verify(value_type == m->type->vmember_type, "%s: expected vmember_type (%s) to equal isa(value) (%s)",
            name, m->type->vmember_type->name, type->name);
        verify(m->type->size == value_type->size * header->count, "vector size mismatch for %s", name);
        memcpy(mdata, value, m->type->size);
    } else if (A_is_inlay(m)) {
        // copy inlay data
        memcpy(mdata, value, m->type->size);
    } else {

        verify(value_type == m->type || value_type == m->type->parent_type, "%s:%s: must convert data from %s to %s",
            type->name, name, value_type->name, m->type->name);

        object prev = *mdata;
        // assign as object, increase ref count
        *mdata = A_hold(value); 
        A_drop(prev);
    }*/
    return value;
}


object A_get_property(object instance, symbol name) {
    AType type = isa(instance);
    type_member_t* m = A_member(type, (A_MEMBER_PROP | A_MEMBER_PRIV | A_MEMBER_INTERN), (cstr)name, true);
    verify(m, "%s not found on object %s", name, type->name);
    object *mdata = (object*)((cstr)instance + m->offset);
    object  value = *mdata;
    return A_is_inlay(m) ? A_primitive(m->type, mdata) : value;
}


/// should be adapted to work with schemas 
/// what a weird thing it would be to have map access to properties
/// everything should be A-based, and forget about the argument hacks?
map A_arguments(int argc, symbol argv[], map default_values, object default_key) {
    map result = new(map, hsize, 16, assorted, true);
    for (item ii = default_values->fifo->first; ii; ii = ii->next) {
        object k = ii->key;
        object v = ii->value;
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

            for (item f = default_values->fifo->first; f; f = f->next) {
                /// import A types from runtime
                object def_value = f->value;
                AType   def_type = def_value ? isa(def_value) : typeid(string);
                assert(f->key == f->key, "keys do not match"); /// make sure we copy it over from refs
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

object A_primitive(AType type, none* data) {
    assert(type->traits & A_TRAIT_PRIMITIVE, "must be primitive");
    object copy = A_alloc(type, type->size);
    memcpy(copy, data, type->size);
    return copy;
}

object A_enum(AType type, i32 data) {
    assert(type->traits & A_TRAIT_ENUM, "must be enum");
    assert(type->size == sizeof(i32), "enum size invalid");
    object copy = A_alloc(type, type->size);
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
object i(i64 data)       { return A_primitive(&i64_type, &data); }
object A_sz (sz  data)   { return A_primitive(&sz_type,  &data); }
object A_u64(u64 data)   { return A_primitive(&u64_type, &data); }
object A_f32(f32 data)   { return A_primitive(&f32_type, &data); }
object A_f64(f64 data)   { return A_primitive(&f64_type, &data); }
object A_f128(f64 data)  { return A_primitive(&f128_type, &data); }
object float32(f32 data) { return A_primitive(&f32_type, &data); }
object real64(f64 data)  { return A_primitive(&f64_type, &data); }
object A_cstr(cstr data) { return A_primitive(&cstr_type, &data); }
object A_none()          { return A_primitive(&none_type, NULL); }
object A_bool(bool data) { return A_primitive(&bool_type, &data); }

/// A -------------------------
none A_init(A a) { }

none A_drop_members(object a) {
    A        f = A_header((A)a);
    AType type = f->type;
    while (type != typeid(A)) {
        for (num i = 0; i < type->member_count; i++) {
            type_member_t* m = &type->members[i];
            if ((m->member_type & (A_MEMBER_PROP | A_MEMBER_PRIV)) &&
                    !A_is_inlay(m)) {
                if (m->args.meta_0 == typeid(weak))
                    continue;
                //printf("A_dealloc: drop member %s.%s (%s)\n", type->name, m->name, m->type->name);
                A*  ref = (A*)((u8*)a + m->offset);
                A_drop(*ref);
                *ref = null;
            }
        }
        type = type->parent_type;
    }
}

none A_dealloc(A a) { 
    A        f = A_header(a);
    AType type = f->type;
    if (!(type->traits & A_TRAIT_USER_INIT)) // composer does this for an 'unmount' operation, and its non-mount results in no holds at all (which is why we must compensate here)
        A_drop_members(a);
    if ((object)f->data != (object)a) {
        A_drop(f->data);
        f->data = null;
    }
}
u64  A_hash      (A a) { return (u64)(size_t)a; }
bool A_cast_bool (A a) {
    A info = A_header(a);
    bool has_count = info->count > 0;
    if (has_count && info->type == typeid(bool))
        return *(bool*)a;

    return has_count;
}

type_member_t* A_member_type(AType type, enum A_MEMBER mt, AType f, bool poly) {
    for (num i = 0; i < type->member_count; i++) {
        type_member_t* mem = &type->members[i];
        if ((mt == 0) || (mem->member_type & mt) && (mem->type == f))
            return mem;
    }
    if (poly && type->parent_type && type->parent_type != typeid(A))
        return A_member(type->parent_type, mt, f, true);
    return 0;
}

object A_cast(A a, AType type) {
    if (A_instanceof(a, type)) return (object)a;
    AType atype = isa(a);
    Member m = A_member_type(atype, A_MEMBER_CAST, type, true);
    if (m) {
        object(*fcast)(object) = m->ptr;
        return fcast(a);
    }
    return null;
}

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
    bool retry = true;
    while (retry) {
        retry = false;
        while (*scan && isspace(*scan))
            scan++;
        if (*scan == '#') {
            scan++;
            while (*scan && *scan != '\n')
                scan++;
            retry = *scan != 0;
        } else if (strncmp(scan, "/*", 2) == 0) {
            scan += 2;
            scan = strstr(scan, "*/");
            if (scan) {
                scan = scan + 2;
                retry = true;
            }
        }
    }
    return scan;
}

// lets handle both quoted and non-quoted string serialization
string prep_cereal(cereal cs) {
    cstr   scan = (cstr)cs.value;
    string res;

    if (*scan == '\"') {
        scan++; // skip opening quote
        cstr start = scan;
        bool escaped = false;

        while (*scan) {
            if (escaped) {
                escaped = false;
            } else if (*scan == '\\') {
                escaped = true;
            } else if (*scan == '\"') {
                break;
            }
            scan++;
        }

        assert(*scan == '\"', "missing end-quote");

        i64 len = scan - start;
        res = string(alloc, len); // string handles +1 for null-terminator
        memcpy((cstr)res->chars, start, len);
    } else
        res = string((symbol)scan);

    return res;
}

A A_with_cstrs(A a, cstrs argv) {
    A_start(argv);
    int argc = 0;
    while (argv[argc]) { // C standard puts a null char* on end, by law (see: Brannigans law)
        cstr arg = argv[argc];
        if (arg[0] == '-') {
            bool single = arg[1] != '-';
            type_member_t* mem = null;
            AType type = isa(a);
            while (type != typeid(A)) {
                for (int i = 0; i < type->member_count; i++) {
                    type_member_t* m = &type->members[i];
                    if ((m->member_type & A_MEMBER_PROP) && 
                        ( single &&        m->name[0] == arg[1]) ||
                        (!single && strcmp(m->name,     &arg[2]) == 0)) {
                        mem = m;
                        break;
                    }
                }
                type = type->parent_type;
            }
            verify(mem, "member not found: %s", &arg[1 + !single]);
            cstr value = argv[++argc];
            verify(value, "expected value after %s", &arg[1 + !single]);
            
            object conv = A_convert(mem->type, string(value));
            A_set_property(a, mem->name, conv);
        }
        argc++;
    }
    return a;
}

A A_with_cereal(A a, cereal _cs) {
    cstr cs = _cs.value;
    sz len = strlen(cs);
    A        f = A_header(a);
    AType type = f->type;
    if      (type == typeid(f64)) sscanf(cs, "%lf",  (f64*)a);
    else if (type == typeid(f32)) sscanf(cs, "%f",   (f32*)a);
    else if (type == typeid(i32)) sscanf(cs, "%i",   (i32*)a);
    else if (type == typeid(u32)) sscanf(cs, "%u",   (u32*)a);
    else if (type == typeid(i64)) sscanf(cs, "%lli", (i64*)a);
    else if (type == typeid(u64)) sscanf(cs, "%llu", (u64*)a);
    else if (type == typeid(bool)) {
        *(bool*)a = (cs[0] == 't' || cs[0] == 'T' || cs[0] == '1');
    }
    else if (type == typeid(string)) {
        string  res = a;
        sz     a_ln = len > -1 ? len : strlen(cs);
        res->chars  = calloc(a_ln + 1, 1);
        res->len    = a_ln;
        memcpy((cstr)res->chars, cs, a_ln);
        return res;
    }
    else {
        bool can = constructs_with(f->type, typeid(string));
        if (can) {
            return construct_with(f->type, string(cs), null);
        }
        printf("implement ctr cstr for %s\n", f->type->name);
        exit(-1);
    }
    return a;
}

bool constructs_with(AType type, AType with_type) {
    for (int i = 0; i < type->member_count; i++) {
        Member mem = &type->members[i];
        if (mem->member_type == A_MEMBER_CONSTRUCT) {
            if (mem->type == with_type)
                return true;
        }
    }
    return false;
}

/// used by parse (from json) to construct objects from data
object construct_with(AType type, object data, ctx context) {
    if (type == typeid(map)) {
        verify(isa(data) == typeid(map), "expected map");
        return hold(data);
    }

    /// this will lookup ways to construct the type from the available data
    AType data_type = isa(data);
    object result = null;
    map    mdata  = null;

    /// construct with map of fields
    if (!(type->traits & A_TRAIT_PRIMITIVE) && data_type == typeid(map)) {
        map m = data;
        result = A_alloc(type, 1);
        pairs(m, i) {
            verify(isa(i->key) == typeid(string),
                "expected string key when constructing object from map");
            string s_key = instanceof(i->key, string);
            A_set_property(result, s_key->chars, i->value);
        }
        mdata = m;
    }
    /// check for identical constructor
    AType atype = type;
    while (atype != typeid(A)) {
        for (int i = 0; i < atype->member_count; i++) {
            Member mem = &atype->members[i];
            
            if (!result && mem->member_type == A_MEMBER_CONSTRUCT) {
                none* addr = mem->ptr;
                /// no meaningful way to do this generically, we prefer to call these first
                if (mem->type == typeid(path) && data_type == typeid(string)) {
                    result = A_alloc(type, 1);
                    result = ((object(*)(object, path))addr)(result, path(((string)data)));
                    verify(A_validator(result), "invalid object");
                    break;
                }
                if (mem->type == data_type) {
                    result = A_alloc(type, 1);
                    result = ((object(*)(object, object))addr)(result, data);
                    verify(A_validator(result), "invalid object");
                    break;
                }
            } else if (context && result && mdata) {
                // lets set required properties from context
                if (mem->required && (mem->member_type & A_MEMBER_PROP) && 
                    !contains(mdata, mem->sname))
                {
                    object from_ctx = get(context, mem->sname);
                    verify(from_ctx,
                        "context requires property: %s (%s) in class %s",
                            mem->name, mem->type->name, atype->name);
                    A_member_set(result, mem, from_ctx);
                }
            }
        }
        atype = atype->parent_type;
    }

    /// simple enum conversion, with a default handled in A_enum_value and type-based match here
    if (!result)
    if (type->traits & A_TRAIT_ENUM) {
        i64 v = 0;
        if (data_type->traits & A_TRAIT_INTEGRAL)
            v = read_integer(data_type);
        else if (data_type == typeid(symbol) || data_type == typeid(cstr))
            v = *A_enum_value (type, (cstr)data);
        else if (data_type == typeid(string))
            v = *A_enum_value (type, (cstr)((string)data)->chars);
        else
            v = *A_enum_value (type, null);
        result = A_alloc(type, 1);
        *((i32*)result) = (i32)v;
    }

    /// check if we may use generic object from string
    if (!result)
    if ((type->traits & A_TRAIT_PRIMITIVE) && (data_type == typeid(string) ||
                                               data_type == typeid(cstr)   ||
                                               data_type == typeid(symbol))) {
        result = A_alloc(type, 1);
        if (data_type == typeid(string))
            A_with_cereal(result, (cereal) { .value = (cstr)((string)data)->chars } );
        else
            A_with_cereal(result, (cereal) { .value = data });
    }

    /// check for compatible constructor
    if (!result)
    for (int i = 0; i < type->member_count; i++) {
        Member mem = &type->members[i];
        if (!mem->ptr) continue;
        none* addr = mem->ptr;
        /// check for compatible constructors
        if (mem->member_type == A_MEMBER_CONSTRUCT) {
            u64 combine = mem->type->traits & data_type->traits;
            if (combine & A_TRAIT_INTEGRAL) {
                i64 v = read_integer(data);
                result = A_alloc(type, 1);
                     if (mem->type == typeid(i8))   ((none(*)(object, i8))  addr)(result, (i8)  v);
                else if (mem->type == typeid(i16))  ((none(*)(object, i16)) addr)(result, (i16) v);
                else if (mem->type == typeid(i32))  ((none(*)(object, i32)) addr)(result, (i32) v);
                else if (mem->type == typeid(i64))  ((none(*)(object, i64)) addr)(result, (i64) v);
            } else if (combine & A_TRAIT_REALISTIC) {
                result = A_alloc(type, 1);
                if (mem->type == typeid(f64))
                    ((none(*)(object, double))addr)(result, (double)*(float*)data);
                else
                    ((none(*)(object, float)) addr)(result, (float)*(double*)data);
                break;
            } else if ((mem->type == typeid(symbol) || mem->type == typeid(cstr)) && 
                       (data_type == typeid(symbol) || data_type == typeid(cstr))) {
                result = A_alloc(type, 1);
                ((none(*)(object, cstr))addr)(result, data);
                break;
            } else if ((mem->type == typeid(string)) && 
                       (data_type == typeid(symbol) || data_type == typeid(cstr))) {
                result = A_alloc(type, 1);
                ((none(*)(object, string))addr)(result, string((symbol)data));
                break;
            } else if ((mem->type == typeid(symbol) || mem->type == typeid(cstr)) && 
                       (data_type == typeid(string))) {
                result = A_alloc(type, 1);
                ((none(*)(object, cstr))addr)(result, (cstr)((string)data)->chars);
                break;
            }
        }
    }

    // set field bits here (removed case where json parser was doing this)
    if (result && data_type == typeid(map)) {
        map f = (map)data;
        pairs(f, i) {
            string name = i->key;
            AF_set_name(result, (cstr)name->chars);
        }
    }
    
    if (!result && data) {
        // if constructor not found
        verify(data_type == typeid(string) || data_type == typeid(path),
            "failed to construct type %s with %s", type->name, data_type->name);

        // load from presumed .json as fallback
        path f = (data_type == typeid(string)) ? path((string)data) : (path)data;
        return read(f, type, null);
    }
    return result ? A_initialize(result) : null;
}

none A_serialize(AType type, string res, object a) {
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
        else if (type == typeid(symbol)) len = sprintf(buf, "%s",  *(cstr*)a);
        else if (type == typeid(hook)) len = sprintf(buf, "%p",  *(hook*)a);
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

int A_memcmp(A a, A b) {
    AType atype = isa(a);
    AType btype = isa(b);
    if (atype != btype) return ((ssize_t)atype - (ssize_t)btype) < 0 ? -1 : 1;
    return memcmp(a, b, atype->size);
}

bool A_member_set(A a, type_member_t* m, object value) {
    if (!(m->member_type & A_MEMBER_PROP))
        return false;

    AType type         = isa(a);
    bool  is_primitive = (m->type->traits & A_TRAIT_PRIMITIVE) != 0;
    bool  is_enum      = (m->type->traits & A_TRAIT_ENUM)      != 0;
    bool  is_struct    = (m->type->traits & A_TRAIT_STRUCT)    != 0;
    bool  is_inlay     = (m->member_type  & A_MEMBER_INLAY)    != 0;
    ARef  member_ptr   = (cstr)a + m->offset;
    AType vtype        = isa(value);
    A     vinfo        = head(value);

    if (is_struct) {
        //verify(vtype == m->type->vmember_type, "%s: expected vmember_type (%s) to equal isa(value) (%s)",
        //    m->name, m->type->vmember_type->name, type->name);
        verify(m->type->size == vtype->size * vinfo->count, "vector size mismatch for %s", m->name);
        memcpy(member_ptr, value, m->type->size);
    } else if (is_enum || is_inlay || is_primitive) {
        verify(!is_struct || vtype == m->type ||
            vtype == m->type->vmember_type,
            "%s: expected vmember_type (%s) to equal isa(value) (%s)",
            m->name, m->type->vmember_type->name, vtype->name);
        verify(!is_struct || vtype == m->type ||
            m->type->size == vtype->size * vinfo->count,
            "vector size mismatch for %s", m->name);
        int sz = m->type->size < vtype->size ? m->type->size : vtype->size;
        memcpy(member_ptr, value, sz);
    } else if ((object)*member_ptr != value) {
        //verify(A_inherits(vtype, m->type), "type mismatch: setting %s on member %s %s",
        //    vtype->name, m->type->name, m->name);
        drop(*member_ptr);
        *member_ptr = hold(value);
    }
    AF_set_name(a, m->name);
    return true;
}

// try to use this where possible
object A_member_object(A a, type_member_t* m) {
    if (!(m->member_type & A_MEMBER_PROP))
        return null; // we do this so much, that its useful as a filter in for statements

    bool is_primitive = (m->type->traits & A_TRAIT_PRIMITIVE) | 
                        (m->type->traits & A_TRAIT_STRUCT);
    bool is_inlay     = (m->member_type  & A_MEMBER_INLAY);
    object result;
    ARef   member_ptr = (cstr)a + m->offset;
    if (is_inlay || is_primitive) {
        result = A_alloc(m->type, 1);
        memcpy(result, member_ptr, m->type->size);
    } else {
        result = *member_ptr;
    }
    return result;
}

string A_cast_string(A a) {
    AType type = isa(a);
    A a_header = A_header(a);
    bool  once = false;
    if (instanceof(a, string)) return (string)a;
    string res = new(string, alloc, 1024);
    if (type->traits & A_TRAIT_PRIMITIVE)
        A_serialize(type, res, a);
    else {
        //append(res, type->name);
        append(res, "[");
        for (num i = 0; i < type->member_count; i++) {
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
    return (i32)((sz)(none*)a - (sz)(none*)b);
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


object A_formatter(AType type, FILE* f, object opt, symbol template, ...) {
    va_list args;
    va_start(args, template);
    string  res  = new(string, alloc, 1024);
    cstr    scan = (cstr)template;
    bool write_ln = (i64)opt == true;
    bool is_input = (f == stdin);
    string  field = (!is_input && !write_ln && opt) ? instanceof(opt, string) : null;
    
    while (*scan) {
        /// format %o as object's string cast
        char cmd[2] = { *scan, *(scan + 1) };
        int column_size = 0;
        int skip = 0;
        int f = cmd[1];
        /// column size formatting
        if (cmd[0] == '%' && (cmd[1] == '-' || isdigit(cmd[1]))) {
            /// register to fill this space
            for (int n = 1; n; n++) {
                if (!isdigit(cmd[1 + n])) {
                    int column_digits = 1 + n;
                    verify(column_digits < 32, "column size out of range");
                    char val[32];
                    memcpy(val, scan + 1, column_digits);
                    val[column_digits] = 0;
                    column_size = atoi(val);
                    skip = 1 + column_digits + 1;
                    cmd[1] = scan[skip - 1];
                    break;
                }
            }
        }
        if (cmd[0] == '%' && cmd[1] == 'o') {
            object arg = va_arg(args, object);
            string   a = arg ? cast(string, arg) : string((symbol)"null");
            num    len = a->len;
            reserve(res, len);
            if (column_size < 0) {
                for (int i = 0; i < -column_size - len; i++)
                    ((cstr)res->chars)[res->len++] = ' ';
            }
            memcpy((cstr)&res->chars[res->len], a->chars, len);
            res->len += len;
            if (column_size) {
                for (int i = 0; i < column_size - len; i++)
                    ((cstr)res->chars)[res->len++] = ' ';
            }
            scan     += skip ? skip : 2; // Skip over %o
        } else {
            /// format with vsnprintf
            const char* next_percent = strchr(scan, '%');
            num segment_len = next_percent ? (num)(next_percent - scan) : (num)strlen(scan);
            reserve(res, segment_len);
            memcpy((cstr)&res->chars[res->len], scan, segment_len);
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
                    cstr  end = (cstr)&res->chars[res->len];
                    if (strchr("fFgG", formatter[symbol_len - 1]))
                        f_len = snprintf(end, avail, formatter, va_arg(args, double));
                    else if (strchr("diouxX", formatter[symbol_len - 1]))
                        f_len = snprintf(end, avail, formatter, va_arg(args, int));
                    else if (strchr("c", formatter[symbol_len - 1]))
                        f_len = snprintf(end, avail, formatter, va_arg(args, int));
                    else
                        f_len = snprintf(
                            end, avail, formatter, va_arg(args, none*));
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
    bool symbolic_logging = false;
    
    // handle generic logging with type and function name labels, ability to filter based on log_funcs global map
    // map is setup with *:true on debug builds, unless we explicitly listen
    if (f && field) {
        char info[256];
        symbolic_logging = true;
        object fvalue = get(log_funcs, field); // make get be harmless to map; null is absolutely fine identity wise to understand that
        int    l      = 0;
        string tname  = null;
        string fname  = field;
        static string  asterick = null;
        if (!asterick) asterick = hold(string("*"));
        bool   listen = fvalue ? cast(bool, fvalue) : false;
        if ((l = index_of(fname, "_")) > 1) {
            tname = mid(fname, 0, l);
            fname = mid(fname, l + 1, len(fname) - (l + 1));
            if (!listen && (contains(log_funcs, tname) || contains(log_funcs, fname)))
                listen = true;
        }
        if (!listen && !contains(log_funcs, asterick)) return null;
        // write type / function
        if (tname)
            sprintf(info, "%s::%s: ", tname->chars, fname->chars);
        else
            sprintf(info, "%s: ", fname->chars);
        fwrite(info, strlen(info), 1, f);
    }

    if (f == stderr)
        fwrite("\033[1;33m", 7, 1, f);

    if (f) {
        // write message
        write(res, f, false);
        if (symbolic_logging || write_ln) {
            fwrite("\n", 1, 1, f);
            fflush(f);
        }
    }
    
    if (f == stderr) {
        fwrite("\033[0m", 4, 1, f); // ANSI reset
        fflush(f);
    }

    if (type && (type->traits & A_TRAIT_ENUM)) {
        // convert res to instance of this enum
        object enum_v = A_enum_value(type, (cstr)res->chars);
        f32 test_v = *(f32*)enum_v;
        return enum_v;
    }
    return type ? (object)
        ((A_f*)type)->with_cereal(A_alloc(type, 1), (cereal) { .value = (cstr)res->chars }) :
        (object)res;
}

u64 fnv1a_hash(const none* data, size_t length, u64 hash) {
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

none item_init(item a) {
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

none vector_init(vector a);

vector vector_with_i32(vector a, i32 count) {
    A_realloc(a, count);
    return a;
}

sz vector_len(vector a) {
    return A_header(a)->count;
}








none map_init(map m) {
    if (m->hsize <= 0) m->hsize = 8;
    m->fifo = list(unmanaged, m->unmanaged);
    m->fifo->assoc = m;
}

none map_dealloc(map m) {
    A info = head(m);
    if (m->hlist) {
        for (int b = 0; b < m->hsize; b++)
            while (m->hlist[b]) {
                item i = m->hlist[b];
                item n = i->next;
                drop(i->key);
                drop(i->value);
                drop(i);
                //drop(i->ref);
                m->hlist[b] = n;
            }
        free(m->hlist);
        m->hlist = null;
    }
}

item map_lookup(map m, object k) {
    if (!m->hlist) return null;
    u64 h = hash(k);
    i64 b = h % m->hsize;
    for (item i = m->hlist[b]; i; i = i->next) {
        if (i->h == h && compare(i->key, k) == 0)
            return i;
    }
    return null;
}

bool map_contains(map m, object k) {
    return map_lookup(m, k) != null;
}

object map_get(map m, object k) {
    item i = map_lookup(m, k);
    return i ? i->value : null;
}

item map_fetch(map m, object k) {
    item i = map_lookup(m, k);
    if (!i) {
        u64 h = hash(k);
        i64 b = h % m->hsize;
        m->hlist[b] = i = item(next, m->hlist[b], key, hold(k), h, h);
        m->count++;
    }
    return i;
}

none map_set(map m, object k, object v) {
    if (!m->hlist) m->hlist = (item*)calloc(m->hsize, sizeof(item));
    item i = map_fetch(m, k);
    AType vtype = isa(v);
    A info = head(m);
    bool allowed = !m->last_type || m->last_type == vtype || m->assorted;
    if (!allowed) {
        int test2 = 2;
        test2 += 2;
    }
    verify(allowed,
        "unassorted map set to differing type: %s, previous: %s (%s:%i)",
        vtype->name, m->last_type->name, info->source, info->line);
    m->last_type = vtype;
    if (i->value) {
        if (i->value != v) {
            drop(i->value);
            i->value = m->unmanaged ? v : hold(v);
        } else {
            return;
        }
    } else {
        i->value = m->unmanaged ? v : hold(v);
    }
    item ref = push(m->fifo, m->unmanaged ? v : hold(v));
    ref->key = hold(k);
    ref->ref = i; // these reference each other
    i->ref = ref;
}

none map_rm_item(map m, item i) {
    drop(i->key);
    drop(i->value);
    remove_item(m->fifo, i->ref);
    drop(i);
}

none map_rm(map m, object k) {
    u64  h    = hash(k);
    i64  b    = h % m->hsize;
    item prev = null;
    for (item i = m->hlist[b]; i; i = i->next) {
        if (i->h == h && compare(i->key, k) == 0) {
            if (prev) {
                prev->next = i->next;
            } else {
                m->hlist[b] = i->next;
            }
            map_rm_item(m, i);
            return;
        }
        prev = i;
    }
}

none map_clear(map m) {
    for (int b = 0; b < m->hsize; b++) {
        item prev = null;
        item cur  = m->hlist[b];
        item next = null;
        while (cur) {
            next = cur->next;
            map_rm_item(m, cur);
            cur = next;
        }
    }
}

sz map_len(map a) {
    return a->count;
}

object map_index_sz(map a, sz index) {
    assert(index >= 0 && index < a->count, "index out of range");
    item i = list_get(a->fifo, A_sz(index));
    return i ? i->value : null;
}

object map_index_object(map a, object key) {
    return map_get(a, key);
}

map map_with_i32(map a, i32 size) {
    a->hsize = size;
    return a;
}

string map_cast_string(map a) {
    string res  = string(alloc, 1024);
    bool   once = false;
    for (item i = a->fifo->first; i; i = i->next) {
        string key   = cast(string, i->key);
        string value = cast(string, i->value);
        if (once) append(res, " ");
        append(res, key->chars);
        append(res, ":");
        append(res, value->chars);
        once = true;
    }
    return res;
}

none map_concat(map a, map b) {
    pairs(b, e) set(a, e->key, e->value);
}

bool map_cast_bool(map a) {
    return a->count > 0;
}

map map_of(symbol first_key, ...) {
    map a = map(hsize, 16, assorted, true);
    va_list args;
    va_start(args, first_key);
    symbol key = first_key;
    if (!key) return a;
    for (;;) {
        A arg = va_arg(args, A);
        set(a, string(key), arg);
        key = va_arg(args, cstr);
        if (key == null)
            break;
    }
    return a;
}


src src_with_object(src a, object obj) {
    a->obj = obj;
    return a;
}

string src_cast_string(src a) {
    A i = head(a->obj);
    return f(string, "%s:%i", i->source, i->line);
}

define_class(src, A);



bool string_is_numeric(string a) {
    return a->chars[0] == '-' ||
          (a->chars[0] >= '0' && a->chars[0] <= '9');
}

i32 string_first(string a) {
    return a->len ? a->chars[0] : 0;
}

i32 string_last(string a) {
    return a->len ? a->chars[a->len - 1] : 0;
}

f64 string_real_value(string a) {
    double v = 0.0;
    sscanf(a->chars, "%lf", &v);
    return v;
}


string string_ucase(string a) {
    string res = string(a->chars);
    for (cstr s = (cstr)res->chars; *s; ++s) *s = toupper((unsigned char)*s);
    return res;
}

string string_lcase(string a) {
    string res = string(a->chars);
    for (cstr s = (cstr)res->chars; *s; ++s) *s = tolower((unsigned char)*s);
    return res;
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
        {'\r', "\\r"}
    };
    int escape_count = sizeof(escape_map) / sizeof(escape_map[0]);
    int input_len    = len(input);
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
    escaped[pos] = '\0';
    string res = string((symbol)escaped); /// with cstr constructor, it does not 'copy' but takes over life cycle
    free(escaped);
    return res;
}

none  string_dealloc(string a) {
    printf("string_dealloc: %s", a->chars);
    free((cstr)a->chars);
}
num   string_compare(string a, string b)   { return strcmp(a->chars, b->chars); }
num   string_cmp(string a, symbol b)       { return strcmp(a->chars, b); }
bool  string_eq(string a, symbol b)        { return strcmp(a->chars, b) == 0; }

string string_copy(string a) {
    return string((symbol)a->chars);
}

bool A_inherits(AType src, AType check) {
    while (src != typeid(A)) {
        if (src == check) return true;
        src = src->parent_type;
    }
    if ((src   == typeid(A) || src   == typeid(object)) &&
        (check == typeid(A) || check == typeid(object))) {
        return true;
    }
    return src == check; // true for A-type against A-type
}

static inline char just_a_dash(char a) {
    return a == '-' ? '_' : a;
}

string string_interpolate(string a, map f) {
    cstr   s    = (cstr)a->chars;
    cstr   prev = null;
    string res  = string(alloc, 256);

    verify(f && f->hsize, "no hashmap on map, set hsize > 0");

    while (*s) {
        if (*s == '{') {
            if (s[1] == '{') {
                s += 2;
                continue;
            } else {
                if (prev) {
                    append_count(res, prev, (sz)(s - prev));
                    prev = null;
                }
                cstr kstart = ++s;
                u64  hash   = OFFSET_BASIS;
                while (*s != '}') {
                    verify (*s, "unexpected end of string", 1);
                    hash ^= just_a_dash((u8)*(s++));
                    hash *= FNV_PRIME;
                }
                item b = f->hlist[hash % f->hsize]; // todo: change schema of hashmap to mirror map
                item i = null;
                for (i = b; i; i = i->next)
                    if (i->h == hash)
                        break;
                verify(i, "key not found in map");
                string v = A_cast(i->value, typeid(string));

                if (!v)
                    append(res, "null");
                else
                    concat(res, v);
                
                s++; // skip the }
            }
        } else if (*s == '}') {
            verify(s[1] == '}', "missing extra '}'", 2);
            s += 2;
        } else if (!prev) {
            prev = s++;
        } else
            s++;
    }
    if (prev) {
        append_count(res, prev, (sz)(s - prev));
        prev = null;
    }
    return res;
}

i32   string_index_num(string a, num index) {
    if (index < 0)
        index += a->len;
    if (index >= a->len)
        return 0;
    return (i32)a->chars[index];
}

array string_split(string a, symbol sp) {
    cstr next = (cstr)a->chars;
    sz   slen = strlen(sp);
    array result = array(alloc, 32);
    while (next) {
        cstr   n = strstr(&next[1], sp);
        string v = string(chars, next, ref_length, n ? (sz)(n - next) : 0);
        next = n ? n + slen : null;
        push(result, v);
        if (!next || !next[0])
            break;
    }
    return result;
}

none string_alloc_sz(string a, sz alloc) {
    char* chars = calloc(1 + alloc, sizeof(char));
    memcpy(chars, a->chars, sizeof(char) * a->len);
    chars[a->len] = 0;
    //free(a->chars);
    a->chars = chars;
    a->alloc = alloc;
}

string string_mid(string a, num start, num len) {
    if (start < 0)
        start = a->len + start;
    if (start < 0)
        start = 0;
    if (start + len > a->len)
        len = a->len - start;
    return new(string, chars, &a->chars[start], ref_length, len);
}

none  string_reserve(string a, num extra) {
    if (a->alloc - a->len >= extra)
        return;
    string_alloc_sz(a, a->alloc + extra);
}

none  string_append(string a, symbol b) {
    sz blen = strlen(b);
    if (blen + a->len >= a->alloc)
        string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy((cstr)&a->chars[a->len], b, blen);
    a->len += blen;
    a->h = 0; /// mutable operations must clear the hash value
    ((cstr)a->chars)[a->len] = 0;
}

none  string_append_count(string a, symbol b, i32 blen) {
    if (blen + a->len >= a->alloc)
        string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy((cstr)&a->chars[a->len], b, blen);
    a->len += blen;
    a->h = 0; /// mutable operations must clear the hash value
    ((cstr)a->chars)[a->len] = 0;
}

string string_trim(string a) {
    cstr s = cstring(a);
    int count = len(a);
    while (*s == ' ') {
        s++;
        count--;
    }
    while (count && s[count - 1] == ' ')
        count--;
    
    return string(chars, s, ref_length, count);
}

string string_ltrim(string a) {
    cstr s = cstring(a);
    int count = len(a);
    while (*s == ' ') {
        s++;
        count--;
    }
    return string(chars, s, ref_length, count);
}

string string_rtrim(string a) {
    cstr s = cstring(a);
    int count = len(a);
    while (s[count - 1] == ' ')
        count--;
    return string(chars, s, ref_length, count);
}

none  string_push(string a, u32 b) {
    sz blen = 1;
    if (blen + a->len >= a->alloc)
        string_alloc_sz(a, (a->alloc << 1) + blen);
    memcpy((cstr)&a->chars[a->len], &b, 1);
    a->len += blen;
    a->h = 0; /// mutable operations must clear the hash value
    ((cstr)a->chars)[a->len] = 0;
}

none  string_concat(string a, string b) {
    string_append(a, b->chars);
}

sz string_len(string a) { return a->len; }

num   string_index_of(string a, symbol cs) {
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
    return (cstr)a->chars;
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

none message_init(message a) {
    a->role = strdup(a->role);
    a->content = strdup(a->content);
}

none message_dealloc(message a) {
    free(a->role);
    free(a->content);
}

none string_init(string a) {
    cstr value = (cstr)a->chars;
    if (a->alloc)
        a->chars = (char*)calloc(1, 1 + a->alloc);
    if (value) {
        sz len = a->ref_length ? a->ref_length : strlen(value);
        if (!a->alloc)
            a->alloc = len;
        if (a->chars == value)
            a->chars = (char*)calloc(1, len + 1);
        memcpy((cstr)a->chars, value, len);
        ((cstr)a->chars)[len] = 0;
        a->len = len;
    }
}

string string_with_i32(string a, i32 value) {
    // Check if the value is within the BMP (U+0000 - U+FFFF)
    if (value <= 0xFFFF) {
        a->len = 1;
        a->chars = calloc(8, 1);
        ((cstr)a->chars)[0] = (char)value;
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
        memcpy((cstr)a->chars, buf, len);
    }
    return a;
}

string string_with_cstr(string a, cstr value) {
    a->len   = value ? strlen(value) : 0;
    a->chars = calloc(a->len + 1, 1);
    memcpy((cstr)a->chars, value, a->len);
    return a;
}


string string_with_symbol(string a, symbol value) {
    return string_with_cstr(a, (cstr)value);
}


bool string_starts_with(string a, symbol value) {
    sz ln = strlen(value);
    if (!ln || ln > a->len) return false;
    return strncmp(&a->chars[0], value, ln) == 0;
}

bool string_ends_with(string a, symbol value) {
    sz ln = strlen(value);
    if (!ln || ln > a->len) return false;
    return strcmp(&a->chars[a->len - ln], value) == 0;
}

item list_push(list a, object e);

item hashmap_fetch(hashmap a, object key) {
    u64 h = a->unmanaged ? (u64)((none*)key) : hash(key);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    for (item f = bucket->first; f; f = f->next)
        if (compare(f->key, key) == 0)
            return f;
    item n = list_push(bucket, null);
    a->item_header = A_header(n);
    n->key = hold(key);
    a->count++;
    return n;
}

none hashmap_clear(hashmap a) {
    each(a->data, list, bucket) {
        while (bucket->first)
            list_remove_item(bucket, bucket->first);
    }
}

item hashmap_lookup(hashmap a, object key) {
    u64 h = a->unmanaged ? (u64)((none*)key) : hash(key);
    u64 k = h % a->alloc;
    list bucket = &a->data[k];
    if (a->unmanaged) {
        for (item f = bucket->first; f; f = f->next)
            if (f->key == key)
                return f;
    } else
        for (item f = bucket->first; f; f = f->next)
            if (compare(f->key, key) == 0)
                return f;
    return null;
}

none hashmap_set(hashmap a, object key, object value) {
    item i = fetch(a, key);
    object prev = i->value;
    i->value = hold(value);
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

none hashmap_init(hashmap a) {
    if (!a->alloc)
         a->alloc = 16;
    a->data  = (list)calloc(a->alloc, sizeof(struct _list)); /// we can zero-init a vectorized set of objects with A-type
}

none hashmap_dealloc(hashmap a) {
    clear(a);
    free(a->data);
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

none list_quicksort(list a, i32(*sfn)(object, object)) {
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

none list_sort(list a, ARef fn) {
    list_quicksort(a, (i32(*)(object, object))fn);
}

object list_get(list a, object at_index);

object A_copy(A a) {
    A f = A_header(a);
    assert(f->count > 0, "invalid count");
    AType type = isa(a);
    object b = A_alloc(type, f->count);
    memcpy(b, a, f->type->size * f->count);
    A_hold_members(b);
    return b;
}

A A_hold(A a) {
    if (a) {
        A f = A_header(a);
        f->refs++;

        af_recycler* af = f->type->af;
        if (f->af_index > 0) {
            af->af[f->af_index] = null;
            f->af_index = 0;
        }
    }
    return a;
}

//#undef dealloc

static int recycle_stop_at;

none recycle_stop(int at) {
    recycle_stop_at = at;
}

none recycle() {
    A_f** types = A_types(&types_len);

    /// iterate through types
    for (num i = 0; i < types_len; i++) {
        A_f* type = types[i];
        af_recycler* af = type->af;
        if (af && af->af_count) {
            for (int i = 0; i <= af->af_count; i++) {
                A a = af->af[i];
                if (a && --a->refs == 0) {
                    A_free(&a[1]);
                }
            }
            af->af_count = 0;
        }
    }
}

none A_free(object a) {
    A       aa = A_header(a);
    A_f*  type = aa->type;
    none* prev = null;
    A_f*   cur = type;
    while (cur) {
        if (prev != cur->dealloc) {
            cur->dealloc(a);
            prev = cur->dealloc;
        }
        if (cur == &A_type)
            break;
        cur = cur->parent_type;
    }
    
    af_recycler* af = type->af;    
    if (true || !af || af->re_alloc == af->re_count) {
        memset(aa, 0xff, type->size);
        free(aa);
        if (--all_type_alloc < 0) {
            printf("all_type_alloc < 0\n");
        }
        if (--type->global_count < 0) {
            printf("global_count < 0 for type %s\n", type->name);
        }
    } else if (af) {
        aa->af_index = 0;
        af->re[af->re_count++] = aa;
    }
}

none A_drop(A a) {
    if (!a) return;
    A header = A_header(a);

    if (header->refs < 0) {
        printf("A_drop: data freed twice: %s\n", header->type->name);
    } else if (--header->refs <= 0) {
        // ref:0 on new, and then ref:+1 when added to list
        // when removing from list, that object is freed.
        // a = new(object) ... then drop(a) ref:-1 also frees.
        // this is to facilitate push(array, new(obj)) <- ends up with a ref of 1.
        // also set(map, string("key"), string("value")) <- both are 1

        // quirks are only seen at top level use of objects,
        // where it is not required to drop a list and the objects you allocated for it.
        // if you dont have a list, then you must drop the objects.

        if (header->af_index > 0) {
            header->type->af->af[header->af_index] = null;
            header->af_index = 0;
        }
        A_free(a);
    }
}

/// bind works with delegate callback registration efficiently to 
/// avoid namespace collisions, and allow enumeration interfaces without override and base object boilerplate
callback A_bind(A a, A target, bool required, AType rtype, AType arg_type, symbol id, symbol name) {
    AType self_type   = isa(a);
    AType target_type = isa(target);
    bool inherits     = A_instanceof(target, self_type) != null;
    string method     = f(string, "%s%s%s", id ? id : 
        (!inherits ? self_type->name : ""), (id || !inherits) ? "_" : "", name);
    type_member_t* m  = A_member(target_type, A_MEMBER_IMETHOD, method->chars, true);
    verify(!required || m, "bind: required method not found: %o", method);
    if (!m) return null;
    callback f       = m->ptr;
    verify(f, "expected method address");
    verify(m->args.count  == 2, "%s: expected method address with instance, and arg*", name);
    verify(!arg_type || m->args.meta_1 == arg_type, "%s: expected arg type: %s", name, arg_type->name);
    verify(!rtype    || m->type        == rtype,    "%s: expected return type: %s", name, rtype->name);
    return f;
}

object A_data(A instance) {
    A obj = A_header(instance);
    return obj->data;
}

i64 A_data_stride(A a) {
    AType t = A_data_type(a);
    return t->size - (t->traits & A_TRAIT_PRIMITIVE ? 0 : sizeof(none*));
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
    item n = item();
    n->value = a->unmanaged ? e : hold(e); /// held already by caller
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


none list_dealloc(list a) {
    while (pop(a)) { }
}

item list_insert_after(list a, object e, i32 after) {
    num index = 0;
    item found = null;
    if (after >= 0)
        for (item ai = a->first; ai; ai = ai->next) {
            if (after <= index) {
                found = ai;
                break;
            }
            index++;
        }
    item n = hold(item(value, e));
    if (!found) {
        n->next = a->first;
        if (a->first)
            a->first->prev = n;
        a->first = n;
        if (!a->last)
             a->last = n;
    } else {
        n->next = null;
        n->prev = found;
        found->next = n;
        if (found->prev)
            found->prev->next = n;
        if (a->last == found)
            a->last = n;
    }
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

none list_remove(list a, num index) {
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

none list_remove_item(list a, item ai) {
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
        type_member_t* m = A_member(ai_t, (A_MEMBER_IMETHOD), "compare", true);
        for (item ai = a->first, bi = b->first; ai; ai = ai->next, bi = bi->next) {
            num   v  = ((num(*)(object,A))((method_t*)m->method)->address)(ai, bi);
            if (v != 0) return v;
        }
    }
    return 0;
}

object list_pop(list a) {
    item l = a->last;
    if (!l)
        return null;
    A info = head(a);
    a->last = a->last->prev;
    if (!a->last)
        a->first = null;
    l->prev = null;
    if (!a->unmanaged)
        drop(l->value);
    drop(l->key);
    a->count--;
    drop(l);
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
            if (A_inherits(bt, mt))
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

none vector_init(vector a) {
    A f = A_header(a);
    f->count = 0;
    f->scalar = f->type->meta.meta_0 ? f->type->meta.meta_0 : a->type ? a->type : typeid(i8);
    verify(f->scalar, "scalar not set");
    if (a->vshape)
        a->alloc = total(a->vshape);
    A_realloc(a, a->alloc);
}

vector vector_with_path(vector a, path file_path) {
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
    return a;
}

ARef vector_get(vector a, num index) {
    num location = index * a->type->size;
    i8* arb = data(a);
    return &arb[location];
}

none vector_set(vector a, num index, ARef element) {
    num location = index * a->type->size;
    i8* arb = data(a);
    memcpy(&arb[location], element, a->type->size); 
}

none vector_realloc(vector a, sz size) {
    A_realloc(a, size);
}

none vector_resize(vector a, sz size) {
    A_resize(a, size);
}

none vector_concat(vector a, ARef any, num count) {
    if (count <= 0) return;
    AType type = data_type(a);
    A f = A_header(a);
    if (f->alloc < f->count + count)
        vector_realloc(a, (a->alloc << 1) + 32 + count);
    
    u8* ptr  = data(a);
    i64 size = data_stride(a);
    memcpy(&ptr[f->count * size], any, size * count);
    f->count += count;
    if (a->vshape)
        a->vshape->data[a->vshape->count - 1] = f->count;
}

none vector_push(vector a, A any) {
    vector_concat(a, any, 1);
}

num abso(num i) { 
    return (i < 0) ? -i : i;
}

vector vector_slice(vector a, num from, num to) {
    A      f   = A_header(a);
    /// allocate the data type (no i8 bytes)
    num count = (1 + abso(from - to)); // + 31 & ~31;
    object res = A_alloc(f->type, 1);
    A      res_f = A_header(res);

    /// allocate 
    u8* src   = f->data;
    u8* dst   = null; //A_valloc(f->type, f->scalar, count, count);
    fault("implement vector_slice allocator");
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

define_class(vector, A);


object subprocedure_invoke(subprocedure a, object arg) {
    object(*addr)(object, object, object) = a->addr;
    return addr(a->target, arg, a->ctx);
}

bool isname(char n) {
    return (n >= 'a' && n <= 'z') || (n >= 'A' && n <= 'Z') || (n == '_');
}

u64 fn_hash(fn f) {
    return (u64)f->method->address;
}

object fn_call(fn f, array args) {
#ifdef USE_FFI
    return A_method_call(f->method, args);
#else
    return null;
#endif
}

bool create_symlink(path target, path link) {
    bool is_err = symlink(target, link) == -1;
    return !is_err;
}

/*
none file_init(file f) {
    verify(!(f->read && f->write), "cannot open for both read and write");
    cstr src = (cstr)(f->src ? f->src->chars : null);
    if (!f->id && (f->read || f->write)) {
        verify (src || f->write, "can only create temporary files for write");

        if (!src) {
            i64    h      = 0;
            bool   exists = false;
            string r      = null;
            path   p      = null;
            do {
                h   = (i64)rand() << 32 | (i64)rand();
                r   = formatter("/tmp/f%p", (none*)h);
                src = (cstr)r->chars;
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

bool file_file_write(file f, object o) {
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



object file_file_read(file f, AType type) {
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
    object o = A_alloc(type, 1);
    sz size = isa(o)->size;
    f->location += size;
    verify(type->traits & A_TRAIT_PRIMITIVE, "not a primitive type");
    bool success = fread(o, size, 1, f->id) == 1;
    return success ? o : null;
}

none file_file_close(file f) {
    if (f->id) {
        fclose(f->id);
        f->id = null;
    }
}

none file_dealloc(file f) {
    file_file_close(f);
}
*/

none path_init(path a) {
    cstr arg = (cstr)a->chars;
    num  len = arg ? strlen(arg) : 0;
    a->chars = calloc(len + 1, 1);
    if (arg) {
        memcpy((cstr)a->chars, arg, len + 1);
        a->len = len;
    }
}

none path_cd(path a) {
    chdir(a->chars);
}

path path_temp(symbol tmpl) {
    path p = null;
    do {
        i64    h = (i64)rand() << 32 | (i64)rand();
        string r = formatter("/tmp/%p.%s", (none*)h, tmpl);
        p        = path(chars, r->chars);
    } while (exists(p));
    return p;
}

path path_with_string(path a, string s) {
    a->chars = copy_cstr((cstr)s->chars);
    a->len   = strlen(a->chars);
    return a;
}

num path_len(path a) {
    return strlen(cstring(a));
}

bool path_is_ext(path a, symbol e) {
    string ex = ext(a);
    if (ex && cmp(ex, e) == 0)
        return true;
    return false;
}

bool path_cast_bool(path a) {
    return a->chars && strlen(a->chars) > 0;
}

sz path_cast_sz(path a) {
    return strlen(a->chars);
}

cstr path_cast_cstr(path a) {
    return (cstr)a->chars;
}

string path_cast_string(path a) {
    return new(string, chars, a->chars);
}

path path_with_cstr(path a, cstr cs) {
    a->chars = copy_cstr((cstr)cs);
    a->len   = strlen(a->chars);
    return a;
}

path path_with_symbol(path a, symbol cs) {
    a->chars = copy_cstr((cstr)cs);
    a->len   = strlen(a->chars);
    return a;
}

bool path_move(path a, path b) {
    return rename(a->chars, b->chars) == 0;
}

bool path_make_dir(path a) {
    cstr cs  = (cstr)a->chars; /// this can be a bunch of folders we need to make in a row
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

i64 get_stat_millis(struct stat* st) {
#if defined(__APPLE__)
    return (i64)(st->st_mtimespec.tv_sec) * 1000 + st->st_mtimespec.tv_nsec / 1000000;
#else
    return (i64)(st->st_mtim.tv_sec) * 1000 + st->st_mtim.tv_nsec / 1000000;
#endif
}

static path _path_latest_modified(path a, ARef mvalue, map visit) {
    cstr base_dir = (cstr)a->chars;
    if (!is_dir(a))
        return 0;

    cstr canonical = realpath(base_dir, null);
    if (!canonical) return null;
    string k = string(canonical);
    if (contains(visit, k)) return null;;
    set(visit, k, A_bool(true));
    
    DIR *dir = opendir(base_dir);
    char abs[4096];
    struct dirent *entry;
    struct stat statbuf;
    i64  latest   = 0;
    path latest_f = null;

    verify(dir, "opendir");
    while ((entry = readdir(dir)) != null) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(abs, sizeof(abs), "%s/%s", base_dir, entry->d_name);

        if (stat(abs, &statbuf) == 0) {
            if (S_ISREG(statbuf.st_mode)) {
                i64 stat_millis = get_stat_millis(&statbuf);
                if (stat_millis > latest) {
                    latest = stat_millis;
                    *((i64*)mvalue) = latest;
                    latest_f = path(abs);
                }
            } else if (S_ISDIR(statbuf.st_mode)) {
                path subdir = new(path, chars, abs);
                i64 sub_latest = 0;
                path lf = _path_latest_modified(subdir, &sub_latest, visit);
                if (lf && sub_latest > latest) {
                    latest = sub_latest;
                    *((i64*)mvalue) = latest;
                    latest_f = lf;
                }
            }
        }
    }
    closedir(dir);
    return latest_f;
}

path path_latest_modified(path a, ARef mvalue) {
    return _path_latest_modified(a, mvalue, map(hsize, 64));
}
 
i64 path_modified_time(path a) {
    struct stat st;
    if (stat((cstr)a->chars, &st) != 0) return 0;

    if (is_dir(a)) {
        i64  mtime  = 0;
        path latest = latest_modified(a, &mtime);
        return mtime;
    } else {
#if defined(__APPLE__)
        return (i64)(st.st_mtimespec.tv_sec) * 1000 + st.st_mtimespec.tv_nsec / 1000000;
#else
        return (i64)(st.st_mtim.tv_sec) * 1000 + st.st_mtim.tv_nsec / 1000000;
#endif
    }
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
    cstr cs  = (cstr)a->chars; /// this can be a bunch of folders we need to make in a row
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
            memcpy((cstr)res->chars, start, n_bytes);
            res->len = n_bytes;
            break;
        }
    }
    return res;
}

string path_filename(path a) {
    cstr cs  = (cstr)a->chars; /// this can be a bunch of folders we need to make in a row
    sz   len = strlen(cs);
    string res = new(string, alloc, 256);
    for (num i = len - 1; i >= 0; i--) {
        if (cs[i] == '/' || i == 0) {
            cstr start = &cs[i + (cs[i] == '/')];
            int n_bytes = len - i - 1;
            memcpy((cstr)res->chars, start, n_bytes);
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
    result->len    = strlen(result->chars);
    return result;
}


path path_directory(path a) {
    path  result  = new(path);
    char* cp      = strdup(a->chars);
    char* temp    = dirname(cp);
    result->chars = strdup(temp);
    result->len   = strlen(result->chars);
    free(cp);
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
    free(cp);
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
        memcpy( (cstr)res->chars, a->chars, ext_pos + 1);
        if (e_len)
            memcpy((cstr)&res->chars[ext_pos + 1], ext, e_len);
        else
            ((cstr)res->chars)[ext_pos] = 0;
    } else {
        memcpy( (cstr)res->chars, a->chars, len);
        if (e_len) {
            memcpy((cstr)&res->chars[len], ".", 1);
            memcpy((cstr)&res->chars[len + 1], ext, e_len);
        }
    }
    return res;
}

path path_app(cstr name) {
    char exe[PATH_MAX];

#if defined(__APPLE__)
    uint32_t size = sizeof(exe);
    if (_NSGetExecutablePath(exe, &size) != 0) return path(""); // fail safe
#else
    ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (len == -1) return path(""); // fail safe
    exe[len] = '\0';
#endif

    cstr last = strrchr(exe, '/');
    if (last) *last = '\0';
    path res = form(path, "%s/../share/%s/", exe, name);
    if (dir_exists("%o", res)) {
        cd(res);
        return res;
    }
    return null;
}

bool path_is_symlink(path p) {
    struct stat st;
    return lstat(p->chars, &st) == 0 && S_ISLNK(st.st_mode);
}

path path_resolve(path p) {
    char buf[PATH_MAX];
    ssize_t len = readlink(p->chars, buf, sizeof(buf) - 1);
    if (len == -1) return hold(p);
    buf[len] = '\0';
    return path(buf);
}
 
bool path_eq(path a, path b) {
    struct stat sa, sb;
    int ia = stat(a->chars, &sa);
    int ib = stat(b->chars, &sb);
    return ia == 0 &&
           ib == 0 &&
           sa.st_ino == sb.st_ino &&
           sa.st_dev == sb.st_dev;
}

#define MAX_PATH_LEN 4096

/// public statics are not 'static'
path path_cwd(sz size) {
    if (size == 0) size = MAX_PATH_LEN;
    path a = new(path);
    a->chars = calloc(size, 1);
    char* res = getcwd((cstr)a->chars, size);
    a->len   = strlen(a->chars);
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

string serialize_environment(map environment, bool b_export) {
    string env = string(alloc, 32);
    pairs(environment, i) { // for tapestry use-case: TARGET=%s BUILD=%s PROJECT=%s UPROJECT=%s
        string name  = i->key;
        string value = escape((string)i->value); /// this is great to do
        string f     = form(string, "%o=\"%o\"", name, value);
        if (b_export && !len(env)) append(env, "export ");
        concat(env, f);
        append(env, " ");
    }
    if (len(env)) append(env, " && ");
    return env;
}

/// tapestry parsing, can be used in silver as well
string evaluate(string w, map environment) {
    if (!strstr(w->chars, "$"))
        return w;

    if (!environment) environment = map();
    string env = serialize_environment(environment, true);
    string esc = escape(w);
    print("escaped: %o", esc);

    int in_pipe[2];  // parent writes to child
    int out_pipe[2]; // child writes to parent

    if (pipe(in_pipe) != 0 || pipe(out_pipe) != 0) {
        print("pipe creation failed\n");
        return string("");
    }

    pid_t pid = fork();
    if (pid < 0) {
        print("fork failed\n");
        return string("");
    }

    if (pid == 0) {
        // child
        dup2(in_pipe[0], STDIN_FILENO);  // read from parent's input
        dup2(out_pipe[1], STDOUT_FILENO); // write to parent's output

        close(in_pipe[1]); // close write side
        close(out_pipe[0]); // close read side

        const char* argv[] = { "bash", "-s", NULL };
        execvp("bash", (char* const*)argv);

        _exit(127); // if exec fails
    }

    // parent
    close(in_pipe[0]);  // close unused read side
    close(out_pipe[1]); // close unused write side

    // send command to bash
    FILE* in = fdopen(in_pipe[1], "w");
    fprintf(in, "export %s\n", cstring(env));
    fprintf(in, "echo \"%s\"\n", esc->chars);
    fclose(in); // very important: tell bash EOF (no more input)

    // read result
    FILE* out = fdopen(out_pipe[0], "r");
    char buf[1024];
    string result = string(alloc, 64);

    while (fgets(buf, sizeof(buf), out)) {
        int len = strlen(buf);
        if (len && buf[len - 1] == '\n') buf[len - 1] = 0;
        append(result, buf);
    }
    fclose(out);

    int status;
    waitpid(pid, &status, 0); // wait for bash to finish

    return trim(result);
}

static cstr ws_inline(cstr p) {
    cstr scan = p;
    while (*scan && isspace(*scan) && *scan != '\n')
        scan++;
    return scan;
}

/// 'word' can contain shell script; and there can be spaces inside
/// so its not a traditional token parser, however word is a decent name for this compare to token (used in silver)
static cstr read_word(cstr i, string* result) {
    i = ws_inline(i);
    *result = null;

    if (*i == '\n' || !*i)
        return i;

    int depth = 0;

    while (*i) {
        // stop only if: outside nesting AND hit whitespace
        if (depth == 0 && isspace(*i)) break;

        // detect start of $(...) anywhere
        if (*i == '$' && *(i + 1) == '(') {
            if (!*result) *result = string(alloc, 64);
            append_count(*result, i, 2);
            i += 2;
            depth++;
            continue;
        }

        // track end of $(...)
        if (*i == ')' && depth > 0) {
            append_count(*result, i, 1);
            i++;
            depth--;
            continue;
        }

        // quoted strings inside word (optional but safe)
        if (*i == '"' || *i == '\'') {
            char quote = *i;
            if (!*result) *result = string(alloc, 64);
            append_count(*result, i, 1);
            i++;
            while (*i && *i != quote) {
                if (*i == '\\' && *(i + 1)) {
                    append_count(*result, i, 1);
                    i++;
                }
                append_count(*result, i, 1);
                i++;
            }
            if (*i) {
                append_count(*result, i, 1);
                i++;
            }
            continue;
        }

        if (!*result) *result = string(alloc, 64);
        append_count(*result, i, 1);
        i++;
    }

    return i;
}

static cstr read_indent(cstr i, i32* result) {
    int t = 0;
    int s = 0;
    while (*i == ' ' || *i == '\t') {
        if (*i == ' ')
            s++;
        else if (*i == '\t')
            t++;
        i++;
    }
    *result = t + s / 4;
    return i;
}

static array read_lines(path f) {
    array  lines   = array(256);
    string content = read(f, typeid(string), null);
    cstr   scan    = cstring(content);

    while (*scan) {
        i32 indent = 0;
        scan = read_indent(scan, &indent);
        array words = array(32);
        for (;;) {
            string w = null;
            scan = read_word(scan, &w);
            if (!w) break;
            push(words, w);
        }
        if (len(words)) {
            line l = line(indent, indent, text, words);
            push(lines, l);
        }
        if (*scan == '\n') scan++;
    }

    return lines;
}

object path_read(path a, AType type, ctx context) {
    if (is_dir(a)) return null;
    if (type == typeid(array))
        return read_lines(a);
    FILE* f = fopen(a->chars, "rb");
    if (!f) return null;
    bool is_obj = type && !(type->traits & A_TRAIT_PRIMITIVE);
    fseek(f, 0, SEEK_END);
    sz flen = ftell(f);
    fseek(f, 0, SEEK_SET);
    string str = new(string, alloc, flen);
    size_t n = fread((cstr)str->chars, 1, flen, f);
    fclose(f);
    assert(n == flen, "could not read enough bytes");
    str->len   = flen;
    if (type == typeid(string))
        return str;
    if (is_obj) {
        object obj = parse((cstr)str->chars, type, context);
        return obj;
    }
    assert(false, "not implemented");
    return null;
}

none* primitive_ffi_arb(AType ptype) {
#ifdef USE_FFI
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
#else
    return null;
#endif
}


static none copy_file(path from, path to) {
    path f_to = is_dir(to) ? form(path, "%o/%o", to, filename(from)) : hold(to);
    FILE *src = fopen(cstring(from), "rb");
    FILE *dst = fopen(cstring(f_to), "wb");
    drop(f_to);
    verify(src && dst, "copy_file: cannot open file");

    char buffer[8192];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0)
        fwrite(buffer, 1, n, dst);

    fclose(src);
    fclose(dst);
}

none path_cp(path from, path to, bool recur, bool if_newer) {
    if (dir_exists("%o", from) && file_exists("%o", to))
        fault("attempting to copy from directory to a file");
    
    bool same_kind = is_dir(from) == is_dir(to);
    if (file_exists("%o", from)) {
        if (!if_newer || (!same_kind || modified_time(from) > modified_time(to)))
            copy_file(from, to);
    } else {
        verify(is_dir(from), "source must be directory");
        make_dir(to);
    
        DIR *dir = opendir(cstring(from));
        struct dirent *entry;
        verify(dir, "opendir");
    
        while ((entry = readdir(dir)) != null) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            path src = form(path, "%o/%s", from, entry->d_name);
            path dst = form(path, "%o/%s", to,   entry->d_name);
            cp(src, dst, recur, if_newer);
        }
        closedir(dir);
    }
}

array path_ls(path a, string pattern, bool recur) {
    cstr base_dir = (cstr)a->chars;
    array list = new(array, alloc, 32); // Initialize array for storing paths
    if (!is_dir(a))
        return list;
    DIR *dir = opendir(base_dir);
    char abs[MAX_PATH_LEN];
    struct dirent *entry;
    struct stat statbuf;

    assert (dir, "opendir");
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(abs, sizeof(abs), "%s/%s", base_dir, entry->d_name);
        string s_abs = string(abs);
        if (stat(abs, &statbuf) == 0) {
            if (S_ISREG(statbuf.st_mode)) {
                if (!pattern || !pattern->len || ends_with(s_abs, pattern->chars))
                    push(list, new(path, chars, abs));
                
            } else if (S_ISDIR(statbuf.st_mode)) {
                if (recur) {
                    path subdir = new(path, chars, abs);
                    array sublist = ls(subdir, pattern, recur);
                    concat(list, sublist);
                } else if (!pattern)
                    push(list, new(path, chars, abs));
            }
        }
    }
    closedir(dir);
    return list;
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
            concat(res, json(mem->sname));
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

string parse_symbol(cstr input, cstr* remainder, ctx context) {
    cstr start = null;
    bool second = false;
    while ((!second && isalpha(*input)) || 
           ( second && isalnum(*input)) ||
           ((*input == '-' || *input == '_')  && second))
    {
        if (!start) start = input;
        input++;
        second = true;
    }
    *remainder = ws(input);

    if (start) {
        string r = string(chars, start, ref_length, (size_t)input - (size_t)start);
        for (int i = 0; i < r->len; i++)
            if (r->chars[i] == '-') ((cstr)r->chars)[i] = '_';
        return r;
    }
    
    return null;
}

string parse_json_string(cstr origin, cstr* remainder, ctx context) {
    char delim = *origin;
    if (delim != '\"' && delim != '\'')
        return null;
    
    string res = string(alloc, 64);
    cstr scan;
    for (scan = &origin[1]; *scan;) {
        if (*scan == delim) {
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
    return (context && delim == '\'') ? interpolate(res, context) : res;
}

object parse_array(cstr s, AType schema, AType meta, cstr* remainder, ctx context);

type_member_t* A_member_first(AType type, AType find, bool poly) {
    if (poly && type->parent_type != typeid(A)) {
        type_member_t* m = A_member_first(type->parent_type, find, poly);
        if (m) return m;
    }
    for (int i = 0; i < type->member_count; i++) {
        type_member_t* m = &type->members[i];
        if (!(m->member_type & A_MEMBER_PROP)) continue;
        if (m->type == type) return m;
    }
    return null;
}

object parse_object(cstr input, AType schema, AType meta_type, cstr* remainder, ctx context) {
    cstr   scan   = ws(input);
    cstr   origin = null;
    object res    = null;
    char  *endptr;

    if (remainder)
       *remainder = null;

    string sym     = parse_symbol(scan, &scan, context);
    bool   set_ctx = sym && context && !remainder && context->establishing && eq(sym, "ctx");
    bool   is_true = false;

    if (sym && ((is_true = eq(sym, "true")) || eq(sym, "false"))) {
        verify(!schema || schema == typeid(bool), "type mismatch");
        res = A_bool(is_true); 
    }
    else if (*scan == '[') {
        if (sym) {
            // can be an enum, can be a struct, or class
            // for these special syntax, we cannot take schema & meta_type into account
            // this is effectively 'ason' syntax
            scan = ws(scan + 1);
            AType type = A_find_type(sym->chars);
            verify(type, "type not found: %o", sym);

            if (type->traits & A_TRAIT_ENUM) {
                // its possible we could reference a context variable within this enum [ value-area ]
                // in which case, we could effectively look it up
                //
                object evalue = A_alloc(type, 1);
                string enum_symbol = parse_symbol(scan, &scan, context);
                verify(enum_symbol, "enum symbol expected");
                verify(*scan == ']', "expected ']' after enum symbol");
                scan++;
                type_member_t* e = A_member(
                    type, A_MEMBER_ENUMV, enum_symbol->chars, false);
                verify(e, "enum symbol %o not found in type %s", enum_symbol, type->name);
                memcpy(evalue, e->ptr, e->type->size);

                res = evalue;
            } else if (type->traits & A_TRAIT_STRUCT) {
                object svalue = A_alloc(type, 1);
                for (int i = 0; i < type->member_count; i++) {
                    type_member_t* m = &type->members[i];
                    if (!(m->member_type & A_MEMBER_PROP)) continue;
                    object f = (object)((cstr)svalue + m->offset);
                    object r = parse_object(scan, null, null, &scan, context);
                    verify(r && isa(r) == m->type, "type mismatch while parsing struct %s:%s (read: %s, expect: %s)",
                        type->name, m->name, !r ? "null" : isa(r)->name, m->type->name);
                    if (*scan == ',') scan = ws(scan + 1); // these dumb things are optional
                }
                scan++;
                res = svalue;
            } else {
                // this is parsing a 'constructor call', so we must effectively call this constructor with the arg given
                // only a singular arg is allowed for these
                object r = parse_object(scan, null, null, &scan, context);
                verify(r, "expected value to give to type %s", type->name);
                verify(*scan == ']', "expected ']' after construction of %s", type->name);
                scan++;

                res = construct_with(type, r, null);
            }
        } else {
            res = parse_array (scan, schema, meta_type, remainder, context);
            scan = *remainder;
        }
    }
    else if ((*scan >= '0' && *scan <= '9') || *scan == '-') {
        verify(!sym, "unexpected numeric after symbol %o", sym);
        origin = scan;
        int has_dot = 0;
        while (*++scan) {
            has_dot += *scan == '.';
            if (*scan == 'f')
                break;
            if (*scan != '.' && !(*scan >= '0' && *scan <= '9'))
                break;
        }
        if (has_dot || (schema && schema->traits & A_TRAIT_REALISTIC)) {
            bool force_f32 = false;
            if (has_dot) {
                force_f32 = *scan == 'f';
                if (force_f32) scan++;
            }
            if (schema == typeid(i64)) {
                double v = strtod(origin, &scan);
                res = A_i64((i64)floor(v));
            }
            else if (force_f32 || schema == typeid(f32)) {
                res = A_f32(strtof(origin, &scan));
                if (force_f32)
                    scan++; // f++
            }
            else
                res = A_f64(strtod(origin, &scan));
        } else
            res = A_i64(strtoll(origin, &scan, 10));
    }
    else if (*scan == '"' || *scan == '\'') {
        verify(!sym, "unexpected string after symbol %o", sym);
        origin = scan;
        string js = parse_json_string(origin, &scan, context); // todo: use context for string interpolation
        res = construct_with((schema && schema != typeid(object)) 
            ? schema : typeid(string), js, context);
    }
    else if (*scan == '{') { /// Type will make us convert to the object, from map, and set is_map back to false; what could possibly get crossed up with this one
        if (sym) {
            verify(!schema || schema == typeid(object) || eq(sym, schema->name),
                "expected type: %s, found %o", schema->name, sym);

            if (!schema) {
                schema = A_find_type(sym->chars);
                verify(schema, "type not found: %o", sym);
            }

            if (schema == typeid(object)) {
                schema = A_find_type(sym->chars);
                verify(schema, "%s not found", sym->chars);
            }
        }
        AType use_schema = schema ? schema : typeid(map);
        bool  is_map     = use_schema == typeid(map);
        scan = ws(&scan[1]);
        map props = map(hsize, 16);

        for (;;) {
            scan = ws(&scan[0]);
            if (*scan == '}') {
                scan++;
                break;
            }

            if (!context && *scan != '\"')
                return null;

            origin        = scan;
            string name   = null;
            Member member = null;
            bool   quick_map = false;
            bool   json_type = false;

            // if object, then we find the first object
            if (*scan == '{' && use_schema) {
                /// this goes to the bottom class above A first, then proceeds to look
                /// if you throw a map onto a subclass of element, its going to look through element first
                member = A_member_first(use_schema, typeid(map), true);
                verify(member, "map not found in object %s (shorthand {syntax})",
                    use_schema->name);
                name = member->sname;
                quick_map = true;
            } else
                name = (*scan != '\"' && *scan != '\'') ?
                    (string)parse_symbol(origin, &scan, context) : 
                    (string)parse_json_string(origin, &scan, context);
            
            if (!member) {
                json_type = cmp(name, "Type") == 0;
                member  = (is_map) ? null : 
                    A_member(use_schema, A_MEMBER_PROP, name->chars, true);
            }

            if (eq(name, "nodes")) {
                int test2 = 2;
                test2 += 2;
            }
            
            if (!json_type && !member && !is_map && !context) {
                print("property '%o' not found in type: %s", name, use_schema->name);
                return null;
            }
            
            if (!quick_map) {
                bool short_hand_key = false;
                if (context && *scan == '{') {
                    scan = ws(origin); // nice trick here for this mode, effectively reads symbol again in parse_object
                    short_hand_key = true;
                } else if (*scan != ':')
                    return null;
                else
                    scan = ws(&scan[1]);
            }
            if (*scan == '}') {
                int test2 = 2;
                test2 += 2;
            }
            object value = parse_object(scan, (member ? member->type : null),
                (member ? member->args.meta_0 : null), &scan, context);
            
            if (!value)
                return null;

            if (set_ctx && value)
                set(context, name, value);

            if (json_type) {
                string type_name = value;
                use_schema = A_find_type(type_name->chars);
                verify(use_schema, "type not found: %o", type_name);
            } else
                set(props, name, value);

            if (*scan == ',') {
                scan++;
                continue;
            } else if (!context && *scan != '}')
                return null;
        }

        if (set_ctx && use_schema == typeid(ctx)) // nothing complex; we have a ctx already and will merge these props in
            use_schema = typeid(map);
        
        res = construct_with(use_schema, props, context); // makes a bit more sense to implement required here
        if (use_schema != typeid(map))
            drop(props);
        else
            hold(props);
    } else {
        res = (context && sym) ? get(context, sym) : null;
        if (res) {
            verify(!schema || A_inherits(isa(res), schema),
                "variable type mismatch: %s does not match expected %s",
                isa(res)->name, schema->name);
        } else
            verify(!sym, "cannot resolve symbol: %o", sym);
    }
    if (remainder) *remainder = ws(scan);
    return res;
}

array parse_array_objects(cstr* s, AType element_type, ctx context) {
    cstr scan = *s;
    array res = array(alloc, 64);
    static int seq = 0;
    seq++;

    for (;;) {
        if (scan[0] == ']') {
            scan = ws(&scan[1]);
            break;
        }
        object a = parse_object(scan, element_type, null, &scan, context);
        push(res, a);
        scan = ws(scan);
        if (scan && scan[0] == ',') {
            scan = ws(&scan[1]);
            continue;
        }
    }
    *s = scan;
    return res;
}

object parse_array(cstr s, AType schema, AType meta_type, cstr* remainder, ctx context) {
    cstr scan = ws(s);
    verify(*scan == '[', "expected array '['");
    scan = ws(&scan[1]);
    object res = null;
    if (!schema || (schema == typeid(array) || schema->src == typeid(array))) {
        AType element_type = meta_type ? meta_type : (schema ? schema->meta.meta_0 : typeid(object));
        res = parse_array_objects(&scan, element_type, context);
    } else if (schema->vmember_type == typeid(i64)) { // should support all vector types of i64 (needs type bounds check with vmember_count)
        array arb = parse_array_objects(&scan, typeid(i64), context);
        int vcount = len(arb);
        res = A_alloc2(schema, typeid(i64), shape_new(vcount, 0));
        int n = 0;
        each(arb, object, a) {
            verify(isa(a) == typeid(i64), "expected i64");
            ((i64*)res)[n++] = *(i64*)a;
        }
    } else if (schema->vmember_type == typeid(f32)) { // should support all vector types of f32 (needs type bounds check with vmember_count)
        array arb = parse_array_objects(&scan, typeid(f32), context);
        int vcount = len(arb);
        res = A_alloc(typeid(f32), vcount);
        int n = 0;
        each(arb, object, a) {
            AType a_type = isa(a);
            if (a_type == typeid(i64))      ((f32*)res)[n++] =  (float)*(i64*)a;
            else if (a_type == typeid(f32)) ((f32*)res)[n++] = *(float*)a;
            else if (a_type == typeid(f64)) ((f32*)res)[n++] =  (float)*(double*)a;
            else fault("unexpected type");
        }
    } else if (constructs_with(schema, typeid(array))) {
        // i forget where we use this!
        array arb = parse_array_objects(&scan, typeid(i64), context);
        res = construct_with(schema, arb, null);
    } else if (schema->src == typeid(vector)) {
        AType scalar_type = schema->meta.meta_0;
        verify(scalar_type, "scalar type required when using vector (define a meta-type of vector with type)");
        
        array prelim = parse_array_objects(&scan, null, context);
        int count = len(prelim);
        // this should contain multiple arrays of scalar values; we want to convert each array to our 'scalar_type'
        // for instance, we may parse [[1,2,3,4,5...16],...] mat4x4's; we merely need to validate vmember_count and vmember_type and convert
        // if we have a vmember_count of 0 then we are dealing with a single primitive type
        vector vres = A_alloc(schema, 1);
        vres->vshape = shape_new(count, 0);
        A_initialize(vres);
        i8* vdata = data(vres);
        int index = 0;
        each (prelim, object, o) {
            AType itype = isa(o);
            if (itype->traits & A_TRAIT_PRIMITIVE) {
                /// parse object here (which may require additional
                memcpy(&vdata[index], o, scalar_type->size);
                index += scalar_type->size;
            } else {
                fault("implement struct parsing");
            }
        }
        res = vres;
    } else {
        fault("unhandled vector type: %s", schema ? schema->name : null);
    }
    if (remainder) *remainder = scan;
    return res;
}

static map ctx_checksums; // where context goes to live, lol.

string extract_context(cstr src, cstr *endptr) {
    src = ws(src);
    const char *start = strstr(src, "ctx");
    if (!start) return NULL;

    const char *scan = start;
    while (*scan && *scan != '{') scan++;  // find first {
    if (*scan != '{') return NULL;
    scan++;  // past initial {

    int depth = 1;
    bool in_string = false;
    char string_delim = 0;

    while (*scan && depth > 0) {
        if (in_string) {
            if (*scan == '\\' && scan[1]) {
                scan += 2;
                continue;
            } else if (*scan == string_delim) {
                in_string = false;
                scan++;
                continue;
            }
        } else {
            if (*scan == '\'' || *scan == '"') {
                in_string = true;
                string_delim = *scan++;
                continue;
            } else if (*scan == '#' || strncmp(scan, "/*", 2) == 0) {
                scan = ws((cstr)scan);
                continue;
            } else if (*scan == '{') {
                depth++;
            } else if (*scan == '}') {
                depth--;
            }
        }
        scan++;
    }

    size_t len = scan - start;
    if (endptr) *endptr = (cstr)scan;
    return string(chars, start, ref_length, len);
}

object parse(cstr s, AType schema, ctx context) {
    if (context) {
        if (!ctx_checksums) ctx_checksums = hold(map(hsize, 32));
        string key = f(string, "%p", context);
        u64*   chk = get(ctx_checksums, key);
        string ctx = extract_context(s, &s);
        u64 h = hash(ctx);
        if (!chk || *chk != h) {
            set(ctx_checksums, key, A_u64(h));
            context->establishing = true;
            map ctx_update = parse_object((cstr)ctx->chars, null, null, null, context);
            context->establishing = false;
        }
    }
    return parse_object(s, schema, null, null, context);
}

typedef struct thread_t {
    pthread_t       obj;
    mutex           lock;
    i32             index;
    bool            done;
    object          w;
    object          next;
    async           t;
    i32             jobs;
} thread_t;

static none async_runner(thread_t* thread) {
    async t = thread->t;

    for (; thread->next; unlock(thread->lock)) {
        t->work_fn(thread->w);

        lock(thread->lock);
        thread->done = true;
        cond_signal(thread->lock);
        cond_broadcast(t->global);

        while (thread->next == thread->w)
            cond_wait(thread->lock);
        
        if (!thread->next)
            break;
        
        thread->done = false;
        drop(thread->w);
        thread->w    = thread->next;
        thread->jobs++; // set something so sync can know
    }
    unlock(thread->lock);
}

none async_init(async t) {
    i32    n = len(t->work);
    verify(n > 0, "no work given, no threads needed");
    // we can then have a worker modulo restriction
    // (1 by default to grab the next available work; 
    //  or say modulo of work length to use a pool)
    t->threads = (thread_t*)calloc(sizeof(thread_t), n);
    t->global = mutex(cond, true);
    for (int i = 0; i < n; i++) {
        thread_t* thread = &t->threads[i];
        thread->index = i;
        thread->w     = get(t->work, i);
        thread->next  = thread->w;
        thread->t     = t;
        thread->lock  = mutex(cond, true);
        lock(thread->lock);
        pthread_create(&thread->obj, null, async_runner, thread);
    }
    for (int i = 0; i < n; i++) {
        thread_t* thread = &t->threads[i];
        unlock(thread->lock);
    }
}

none async_dealloc(async t) {
    sync(t, null);
    for (int i = 0, n = len(t->work); i < n; i++) {
        thread_t* thread = &t->threads[i];
        drop(thread->lock);
    }
}


object async_sync(async t, object w) {
    int n = len(t->work);
    object result = null;

    if (w) {
        /// in this mode, wait for one to finish
        /// can we set a condition here or purely wait real fast?
        for (;;) {
            bool found = false;
            lock(t->global);
            for (int i = 0; i < n; i++) {
                thread_t* thread = &t->threads[i];
                lock(thread->lock);
                if (thread->done) {
                    result = copy(thread->w);
                    thread->next = w;
                    found = true;
                    cond_signal(thread->lock);
                    unlock(thread->lock);
                    break;
                }
                unlock(thread->lock);
            }
            if (found) {
                unlock(t->global);
                if (found)
                    break;
            }
            cond_wait(t->global);
        }
    } else {
        for (int i = 0; i < n; i++) {
            thread_t* thread = &t->threads[i];
            lock(thread->lock);
            thread->next = null;
            unlock(thread->lock);
            pthread_join(thread->obj, null);
        }
        // user can get thread->work for simple cases of sync
        // why return either array or individual work based on argument?
    }
    return result;
}

/*
struct inotify_event {
    int wd;           // THIS is the ID of the watch
    uint32_t mask;
    uint32_t cookie;
    uint32_t len;
    char name[];      // Optional file name (if watching a dir)
};
*/

none watch_init(watch a) {
    /// todo: a-perfectly-good-watch ... dont-throw-away

    if (!a->res) return;

    int fd = inotify_init1(IN_NONBLOCK);
    if (fd < 0) {
        perror("inotify_init1");
        exit(1);
    }

    int wd = inotify_add_watch(fd, a->res->chars, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(1);
    }

    char buf[4096]
        __attribute__((aligned(__alignof__(struct inotify_event))));
    
    while (1) {
        #undef read
        int len = read(fd, buf, sizeof(buf));
        if (len <= 0) continue;

        for (char *ptr = buf; ptr < buf + len; ) {
            struct inotify_event *event = (struct inotify_event *) ptr;
            printf("Event on %s: ", event->len ? event->name : "file");
            if (event->mask & IN_CREATE) puts("Created");
            if (event->mask & IN_DELETE) puts("Deleted");
            if (event->mask & IN_MODIFY) puts("Modified");
            ptr += sizeof(struct inotify_event) + event->len;
        }
        usleep(100000); // throttle
    }

    close(fd);
}

none watch_dealloc(watch a) {
    pause(a);
}

none watch_pause(watch a) {
}

none watch_start(watch a) {
}

bool is_alphabetic(char ch) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        return true;
    return false;
}

// would be nice if region could use these for the 4 coords
// if unit could store percent we handle the use-case
// t10%
// type = xalign ()
unit unit_with_string(unit a, string s) {
       s       = trim(s);
    sz ln      = len (s);
    a->percent = last(s) == '%';

    if (a->percent)
        s = mid(s, 0, ln - 1);

    a->meta_type = isa(a)->meta.meta_0;
    
    verify(a->meta_type, "not a meta-type -- not turtle enough for turtle club");
    a->enum_v = *A_enum_default(a->meta_type);
    if (ln == 0)
        return a;
    
    bool al = is_alphabetic(last(s));
    if (!al) {
        verify(!is_alphabetic(first(s)), "expected single value when end is not alpha numeric");
        verify (sscanf(s->chars, "%f", &a->scale_v) == 1,
            "unit parsing for %s failed: value is %s",
            a->meta_type->name, s->chars);
    } else {
        string u = null;
        for (int i = ln - 1; i >= 0; i--) {
            bool al = is_alphabetic(s->chars[i]);
            if (!al) {
                u = mid(s, i + 1, ln - (i + 1));
                a->enum_v = *A_enum_value(a->meta_type, (cstr)u->chars);
                string r = trim(mid(s, 0, clamp(i + 1, 0, len(s) - 1)));
                verify (sscanf(r->chars, "%f", &a->scale_v) == 1,
                    "unit parsing for %s failed: value is %s",
                    a->meta_type->name, r->chars);

                break;
            }
        }
        if (!u) {
            a->enum_v  = *A_enum_value(a->meta_type, (cstr)s->chars);
            a->scale_v = 0;
        }
    }
    return a;
}


object subs_invoke(subs a, object arg) {
    each (a->entries, subscriber, sub) {
        sub->method(sub->target, arg);
    }
    return null;
}

none subs_add(subs a, object target, callback fn) {
    subscriber sub = subscriber(target, target, method, fn);
    push(a->entries, sub);
}

define_class(subscriber, A)
define_class(subs, A)

// todo: simplify objects definition -- make it a simple typedef
define_any(A,      A, sizeof(struct _A), A_TRAIT_CLASS | A_TRAIT_BASE);
define_any(object, A, sizeof(struct _A), A_TRAIT_CLASS | A_TRAIT_BASE, A);

//define_class(A, A)
//define_class(object,  A, A)

define_class(watch,   A)
define_class(message, A)
define_class(mutex,   A)
define_class(async,   A)

define_class(numeric, A)
define_abstract(string_like,    0)
define_abstract(nil,            0)
define_abstract(raw,            0)
define_abstract(ref,            0)
define_abstract(imported,       0)
define_abstract(weak,           0)
define_abstract(functional,     0)
 

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
define_primitive(cereal, raw, 0)
define_primitive(none,   nil, 0)
define_primitive(AType,  raw, 0)
define_primitive(handle, raw, 0)
define_primitive(Member, raw, 0)
define_primitive(ARef,   ref, 0)
define_primitive(floats, raw, 0)
define_primitive(hook,   raw, 0)
define_primitive(callback, raw, 0)
define_primitive(cstrs, raw, 0)

define_class(line, A)

define_enum(OPType)
define_enum(Exists)
define_enum(level)

define_class(path, string)
//define_class(file)
define_class(string, A)

define_class(unit, A)

define_class(item, A)
//define_proto(collection) -- disabling for now during reduction to base + class + mod
define_class(list,          A, object)
define_class(array,         A, object)
define_class(hashmap,       A)
define_class(map,           A)
define_class(ctx,           map)
define_class(fn,            A)
define_class(subprocedure,  A)

define_class(ATypes,           array, AType)
define_class(array_map,        array, map)
define_class(array_string,     array, string)



