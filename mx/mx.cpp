#include <mx/mx.hpp>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__linux__) || defined(__APPLE__)
#include <sys/time.h>
#endif

namespace ion {

A_impl(str, string)
A_impl(Field, field)
A_impl(List, list)
A_impl(Item, item)
A_impl(Hash, hashmap)

u64 fnv1a_hash(const void* data, size_t length, u64 hash) {
    const u8* bytes = (const u8*)data;
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];  // xor bottom with current byte
        hash *= FNV_PRIME; // multiply by FNV prime
    }
    return hash;
}

static cstr parse_fn(const std::string &cn);

static void describe_type(memory *mem, cstr name, sz_t sz, u64 traits);

static void push_type(id *type);

void register_type2(id *type, const std::string &sig, sz_t sz, u64 traits);

static id *primitive_type(symbol name, sz_t sz);

template <>
id *id::for_type<null_t>(void*, size_t) {
    static id* type; if (type) return type;
    type = primitive_type("std::nullptr_t", sizeof(null_t));
    return type;
}

template <>
id *id::for_type<char>(void*, sz_t) {
    static id* type; if (type) return type;
    type = primitive_type("char", sizeof(char));
    type->traits |= traits::integral;
    return type;
}

template <>
id *id::for_type<i64>(void*, sz_t) {
    static id* type; if (type) return type;
    type = primitive_type("long long", sizeof(i64));
    type->traits |= traits::integral;
    return type;
}

template <>
id *id::for_type<u64>(void*, sz_t) {
    static id* type; if (type) return type;
    type = primitive_type("unsigned long long", sizeof(u64));
    type->traits |= traits::integral;
    return type;
}


cstr parse_fn(const std::string &cn) {
    std::string      st = "with TT = ";
    std::string      en = ";";
    num		         p  = cn.find(st) + st.length();
    num              ln = cn.find(en, p) - p;
    std::string      nm = cn.substr(p, ln);
    auto             sp = nm.find(' ');
    std::string  s_name = (sp != std::string::npos) ? nm.substr(sp + 1) : nm;
    num ns = s_name.find("ion::");
    if (ns >= 0 && s_name.substr(0, ns) != "std")
        s_name = s_name.substr(ns + 5);
    cstr  name = util::copy(s_name.c_str());
    return name;
}

void describe_type(id *type, cstr name, sz_t sz, u64 traits) {
    type->name      = name;
    type->traits    = traits;
    type->base_sz   = sz;
    type->src       = type;
}

void push_type(id *type) {
    M key(type->name);
    M value(M::pointer(type));
    id::types->set(key, value);
}

void register_type(id *type, const std::string &sig, sz_t sz, u64 traits) {
    std::string copy = sig;
    cstr name = parse_fn(sig);
    describe_type(type, name, sz, traits);
    push_type(type);
}

/// these are called before registration, by ident::init
id *primitive_type(symbol name, sz_t sz) {
    id *type = new id();
    memset(type, 0, sizeof(id));
    describe_type(type, (cstr)name, sz, traits::primitive);
    return type;
}

M symbolize(cstr cs, id* type = typeof(string), i32 id = 0) {
    if (!type->symbols)
        type->symbols = new symbols2;

    M name(cs);
    type->symbols->by_name[name] = id;
    type->symbols->by_value[id]  = name;
    type->symbols->list += new asymbol(name.symbol(), id);
    return name;
}

prop::prop(const prop &ref) {
    key         = ref.key ? new M(*ref.key) : null;
    member_addr = ref.member_addr;
    offset      = ref.offset;
    type        = ref.type;
    parent_type = ref.parent_type;
    init_value  = ref.init_value;
    is_method   = ref.is_method;
}

string* prop::to_string() {
    return (string*)(key ? key->a_str->hold() : null);
}

int Pointer::compare(const M& b) const {
    if (type->f.compare)
        return type->f.compare(ptr, b.v_pointer->ptr);
    return (size_t)ptr - (size_t)b.v_pointer->ptr;
}

prop::prop() : key(null), member_addr(0), offset(0), type(null), is_method(false) { }


void *prop::member_pointer(void *M) { return (void *)handle_t(&cstr(M)[offset]); }

symbol prop::name() const { return key->symbol(); }

int string::compare(const M &arg) const {
    string *b = arg.get<string>();
    return strcmp(chars, b->chars);
}


arr *string::split(string *v) const {
    arr  *res    = new arr(length + 1);
    char *start  = chars;
    char *scan   = chars;
    char *sp     = v->chars;
    int   sp_len = v->len();
    assert(sp_len > 0);
    if (length > 0)
        for (;;) {
            if (*scan == 0 || strncmp(scan, sp, sp_len) == 0) {
                int str_len = (int)(size_t)(scan - start);
                if (str_len)
                    res->push(new string(start, str_len));
                scan += sp_len;
                start = scan;
                if (*scan == 0) break;
            } else
                scan++;
        }
    v->drop();
    return res;
}

string *string::format(arr *args) {
    string   *res = new string(length + 1 + args->len() * 128);
    char   *start = chars;
    char     *end = start + length;
    for (;*start;) {
        char *f = strstr(start, "{");
        if (f) {
            if (f[1] != '{') {
                res->append(start, (int)(size_t)(f - start));
                char *s = &f[1];
                char *number_start = s;
                for (; *s && *s != '}'; s++)
                    assert(isdigit(*s));
                assert(*s == '}');
                int   number_len = (int)(size_t)(s - number_start);
                char *number     = new char[number_len + 1];
                memcpy(number, number_start, number_len);
                number[number_len] = 0;
                int n = atoi(number);
                assert(n >= 0 && n < args->len());
                delete[] number;
                M &a  = args->get(n);
                M str = a.to_string();
                if (str.a_str) {
                    char *a_start = str.a_str->chars;
                    char *a_end   = str.a_str->chars + str.a_str->length;
                    res->append(a_start, (int)(size_t)(a_end - a_start));
                } else {
                    res->append("null", 4);
                }
                start = f + 1 + number_len + 1;
            } else {
                res->append(start, (int)(size_t)(f - start) + 1);
                start += 2;
            }
        } else {
            res->append(start, (int)(size_t)(end - start));
            break;
        }
    }
    args->drop();
    return res;
}

string *field::to_string() {
    return str("{0}, {1}");
}


item* hashmap::find_item(const M &key, list **h_list, u64 *phash) {
    const u64 hash = hash_value((M&)key);
    const u64 k    = hash % sz;
    if (phash) *phash = hash;
    list &hist = list_for_key(k);
    if (h_list) *h_list = (list*)&hist;
    for (item *fi: hist.items(hash)) {
        assert(fi->hash() == hash);
        field *f = fi->element.get<field>();
        if (f->key == key)
            return fi;
    }
    return null;
}

M hashmap::lookup(const M &key) {
    item *fi = find_item(key, null, null);
    if (!fi)
        return M();
    field *f = fi->element.get<field>();
    return f->value;
}

/// always creates a field with fetch
field *hashmap::fetch(const M &key) {
    list  *h_list = null;
    u64     hash = 0;
    item  *fi   = find_item(key, &h_list, &hash);
    field *f    = null;
    if (!fi) {
        f = new field(key, null);
        item* i = h_list->push(f); /// we iterate with a filter on hash id in doubly
        i->peer = list->push(f);
        count++;
    } else {
        f = fi->element.get<field>();
    }
    return f;
}

M &hashmap::value(const M &key) {
    field *f = fetch(key);
    return f->value;
}

M &hashmap::operator[](const M &key) {
    field *f = fetch(key);
    return f->value;
}

void hashmap::set(const M &key, const M &value) {
    field *f = fetch(key);
    if (value.a != f->value.a)
        f->value = value;
}

bool hashmap::remove(const M &key) {
    list *h_list = null;
    item *fi     = find_item(key, &h_list);
    if (fi && h_list) {
        list->remove(fi->peer); /// weak ref, no drop needed
        h_list->remove(fi);
        count--;
        return true;
    }
    return false;
}

bool hashmap::contains(const M &key) {
    return hashmap::find_item(key, null);
}

hashmap::hashmap(int sz) : A() {
    h_fields = new list[sz];
    list     = new list;
    count    = 0;
    this->sz = sz;
}


M M::get(const M& o, const M& prop) {
    id   *type = o.a->type == typeof(Pointer) ? o.v_pointer->type     : o.a->type;
    u8   *ptr  = o.a->type == typeof(Pointer) ? (u8*)o.v_pointer->ptr : (u8*)o.a;
    assert(type->meta);
    Hash &meta = *type->meta;
    assert(meta->len() > 0);
    field *f = meta->fetch(prop);
    assert(f);
    prop &pr = f->value; /// performs a type-check
    u8 *member_ptr = &ptr[pr.offset];
    assert(pr.type->f.m_ref);
    M* ref = pr.type->f.m_ref(member_ptr);
    M cp = *ref;
    delete ref;
    return cp;
}

void M::set(const M& o, const M& prop, const M& value) {
    M ref = M::get(o, prop);
    assert(ref.a->type == typeof(Pointer));
    /// check if value is a Value<T>
    /// that is for user types, primitives, misc structs (non-A)
    if (value.a->type->traits & traits::value) {
        /// values are copy constructed
        id*  value_type =  value.v_i32->value_type;
        void* value_ptr = &value.v_i32->value; /// will be the same for any value
        assert(value_type == ref.v_pointer->type);
        if (ref.v_pointer->type->f.dtr)
            ref.v_pointer->type->f.dtr(ref.v_pointer->ptr);
        ref.v_pointer->type->f.ctr_cp(ref.v_pointer->ptr, value_ptr);
    } else {

    }
}

void id::init() {
    static id* id_types;
    if (!id_types) id_types = new id(true);
}


id::id(bool init) {
    static bool once;
    if (!once) {
        once = true;
        id::char_t = typeof(char);
        id::i64_t  = typeof(i64);
        id::u64_t  = typeof(u64);
        id::types  = new hashmap(64);
        push_type(id::char_t); /// when hashmap/doubly are invoked, their specialized for_type2 methods should return their info
        push_type(id::i64_t);
        push_type(id::u64_t);
    }
}


size::size(num            sz) : buffer(typeof(size)) { memset(values, 0, sizeof(values)); values[0] = sz; count = 1; }
size::size(null_t           ) : size(num(0))  { }
size::size(size_t         sz) : size(num(sz)) { }
size::size(i8             sz) : size(num(sz)) { }
size::size(u8             sz) : size(num(sz)) { }
size::size(i16            sz) : size(num(sz)) { }
size::size(u16            sz) : size(num(sz)) { }
size::size(i32            sz) : size(num(sz)) { }
size::size(u32            sz) : size(num(sz)) { }

size::size(std::initializer_list<num> args) : buffer(typeof(size)) {
    size_t i = 0;
    for (auto &v: args)
        values[i++] = v;
    count = args.size();
}

size_t size::   x() const { return values[0];    }
size_t size::   y() const { return values[1];    }
size_t size::   z() const { return values[2];    }
size_t size::   w() const { return values[3];    }
num    size::area() const {
    num v = (count < 1) ? 0 : 1;
    for (size_t i = 0; i < count; i++)
        v *= values[i];
    return v;
}
size_t size::dims() const { return count; }

void size::assert_index(const size &b) const {
    assert(count == b.count);
    for (size_t i = 0; i < count; i++)
        assert(b.values[i] >= 0 && values[i] > b.values[i]);
}

bool size::operator==(size_t b) const { return  area() ==  b; }
bool size::operator!=(size_t b) const { return  area() !=  b; }
bool size::operator==(size &sb) const { return count == sb.count && memcmp(values, sb.values, count * sizeof(num)) == 0; }
bool size::operator!=(size &sb) const { return !operator==(sb); }

void   size::operator++(int)          { values[count - 1] ++; }
void   size::operator--(int)          { values[count - 1] --; }
size  &size::operator+=(size_t sz)    { values[count - 1] += sz; return *this; }
size  &size::operator-=(size_t sz)    { values[count - 1] -= sz; return *this; }

num &size::operator[](size_t index) const {
    return (num&)values[index];
}

size &size::zero() { memset(values, 0, sizeof(values)); return *this; }

            size::operator num() const { return     area();  }
size::operator  i8() const { return  i8(area()); }
size::operator  u8() const { return  u8(area()); }
size::operator i16() const { return i16(area()); }
size::operator u16() const { return u16(area()); }
size::operator i32() const { return i32(area()); }
size::operator u32() const { return u32(area()); }
//explicit size::operator i64() const { return i64(area()); }
size::operator u64() const { return u64(area()); }
size::operator r32() const { return r32(area()); }
size::operator r64() const { return r64(area()); }

num  &size::operator[](num i) { return values[i]; }

size &size::operator=(i8   i) { *this = size(i); return *this; }
size &size::operator=(u8   i) { *this = size(i); return *this; }
size &size::operator=(i16  i) { *this = size(i); return *this; }
size &size::operator=(u16  i) { *this = size(i); return *this; }
size &size::operator=(i32  i) { *this = size(i); return *this; }
size &size::operator=(u32  i) { *this = size(i); return *this; }
size &size::operator=(i64  i) { *this = size(i); return *this; }
size &size::operator=(u64  i) { *this = size(i64(i)); return *this; }

size &size::operator=(const size b) {
    memcpy(values, b.values, sizeof(values));
    count = b.count;
    return *this;
}

size_t size::index_value(const size &index) const {
    size &shape = (size &)*this;
    assert_index(index);
    num result = 0;
    for (size_t i = 0; i < count; i++) {
        num vdim = index.values[i];
        for (size_t si = i + 1; si < count; si++)
            vdim *= shape.values[si];
        
        result += vdim;
    }
    return result;
}























































logger console;

void free64(void* ptr) {
#if defined(WIN32)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void* calloc64(size_t count, size_t size) {
    // Calculate total bytes
    size_t total_bytes = count * size;

    // We'll use aligned_alloc on platforms that support it
#if defined(WIN32)
    void* ptr = _aligned_malloc(total_bytes, 64);
    if (!ptr) {
        return NULL;
    }
    memset(ptr, 0, total_bytes);
    return ptr;
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    void* ptr = aligned_alloc(64, total_bytes);
    if (!ptr) {
        return NULL;
    }
    memset(ptr, 0, total_bytes);
    return ptr;
#else
    // For platforms that don't support C11's aligned_alloc
    void* ptr = NULL;
    // posix_memalign allocates aligned memory
    if (posix_memalign(&ptr, 64, total_bytes) != 0) {
        return NULL;  // posix_memalign will return nonzero on failure
    }
    memset(ptr, 0, total_bytes);
    return ptr;
#endif
}

size_t length(std::ifstream& in) {
    std::streamsize base = in.tellg();
    in.seekg(0, std::ios::end);
    std::streamsize to_end = in.tellg();
    in.seekg(base);
    return to_end - base;
}

int snprintf(cstr str, size_t size, const char *format, ...) {
    int n;
    va_list args;
    va_start(args, format);
    
#ifdef _MSC_VER
    n = _vsnprintf_s(str, size, _TRUNCATE, format, args);
#else
    n = vsnprintf(str, size, format, args);
#endif
    
    va_end(args);
    if (n < 0 || n >= (int)size) {
        // handle error here
    }
    return n;
}

#ifdef _WIN32
struct timeval {
    long tv_sec;
    long tv_usec;
};

int gettimeofday(struct timeval* tp, struct timezone* tzp) {
    // Note: Some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME system_time;
    FILETIME file_time;
    uint64_t time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time  = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}
#endif

void wait_until(u64 to) {
    for (i64 t = microseconds(); t < to; t = microseconds()) {
        usleep(10);
    }
}

void wait_until(i64 to) {
    for (i64 t = millis(); t < to; t = millis()) {
        usleep(1000);
    }
}

i64 millis() {
    struct timeval time;
	gettimeofday(&time, NULL);
	return uint64_t(time.tv_sec) * 1000 + (time.tv_usec / 1000);
}

u64 microseconds() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return uint64_t(time.tv_sec) * 1000 * 1000 + time.tv_usec;
}

#ifdef WIN32
void usleep(__int64 usec) {
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}
#endif

bool chdir(std::string c) {
#if defined(_WIN32)
    utf16 ws(c.c_str());
    return SetCurrentDirectoryW((LPCWSTR)ws.wstr());
#else
    int ret = ::chdir(c.c_str());
    if (ret != 0) {
        printf("chdir failed: errno = %d\n", errno);
        return false;
    }
    return true;
#endif
}

}