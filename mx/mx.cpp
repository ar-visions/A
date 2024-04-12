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

MX::MX(bool   v)    : MX(memory::alloc(typeof(bool), 1, 1, &v)) { }
MX::MX(u8     v)    : MX(memory::alloc(typeof(u8),   1, 1, &v)) { }
MX::MX(i8     v)    : MX(memory::alloc(typeof(i8),   1, 1, &v)) { }
MX::MX(u16    v)    : MX(memory::alloc(typeof(u16),  1, 1, &v)) { }
MX::MX(i16    v)    : MX(memory::alloc(typeof(i16),  1, 1, &v)) { }
MX::MX(u32    v)    : MX(memory::alloc(typeof(u32),  1, 1, &v)) { }
MX::MX(i32    v)    : MX(memory::alloc(typeof(i32),  1, 1, &v)) { }
MX::MX(u64    v)    : MX(memory::alloc(typeof(u64),  1, 1, &v)) { }
MX::MX(i64    v)    : MX(memory::alloc(typeof(i64),  1, 1, &v)) { }
MX::MX(r32    v)    : MX(memory::alloc(typeof(r32),  1, 1, &v)) { }
MX::MX(r64    v)    : MX(memory::alloc(typeof(r64),  1, 1, &v)) { }

type_t mx::register_class() { return typeof(mx); }
type_t mx::register_data()  { return null; }

type_t array::register_class() { return typeof(array); }
type_t array::register_data()  { return null; }

mx_implement(str, mx, char)

void ldata::shift(mx* prev_first) {
    assert(ilast);
    ///
    item *i = ifirst;
    bool set_last = ilast == ifirst;
    ifirst        = ifirst->next;
    ///
    if (!ifirst) {
        ilast = null;
    } else
        ifirst->prev = 0;
    ///
    icount--;
    if (prev_first) {
        *prev_first = *(mx*)&i->mem; // location of mx is the location of memory pointer, not its value
    }
    delete i;
}

void ident::init() {
    static ident* id_types;
    if (!id_types) id_types = new ident(true);
}

void *idata::alloc() {
    void *alloc = calloc(1, base_sz);
    return alloc;
}

void *idata::ctr() {
    void *alloc = this->alloc();
    f.ctr(alloc);
    return alloc;
}

void idata::dtr(void* alloc) {
    f.dtr(alloc);
}

void *idata::ctr_str(const str &v) {
    void *alloc = this->alloc();
    f.ctr_str(alloc, v);
    return alloc;
}

void *idata::ctr_mem(memory *mem) {
    void *alloc = this->alloc();
    f.ctr_mem(alloc, mem);
    return alloc;
}

void *idata::ctr_type(memory *mem) {
    void *alloc = this->alloc();
    f.ctr_type(alloc, mem);
    return alloc;
}

void *idata::ctr_cp(void *b) {
    void *alloc = this->alloc();
    f.ctr_cp(alloc, b);
    return alloc;
}

MX::operator mx&() const {
    return *(mx*)(this);
}

//mx mx::mix(const mx& b, float f) const {
//    /// interpolate props here
//    return mx();
//}

int mx::compare(const mx& b) const {
    if (mem && type() == b.type() && mem->count == b.mem->count) {
        type_t ty = mem->type->src; /// we are using src now to resolve basic type from an Array<something>; typeof(Array<int>) -> src = int
        if ((ty->traits & traits::mx) || (ty->traits & traits::mx_obj)) {
            return origin<mx>()->compare(*b.origin<mx>());
        } else {
            size_t cn = mem->count;
            assert(ty->traits & traits::primitive);

            return memcmp(mem->origin, b.mem->origin, ty->base_sz * cn);
        }
    }
    return mem < b.mem ? -1 : 1;
}

mx mx::from_string(cstr cs, type_t type) {
    assert(type);
    
    if (type == typeof(str)) {
        return str(cs);
    } else if (type->traits & traits::integral) { /// u/i 8 -> 64
        if (type == typeof(bool)) {
            std::string s = std::string(cs);
            std::transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c){ return std::tolower(c); });
            return new bool(s == "true" || s == "1" || s == "tru" || s == "yes");
        }
        else if (type == typeof(u32))    return mx(   u32(std::stoi(cs)));
        else if (type == typeof(i32))    return mx(   i32(std::stoi(cs)));
        else if (type == typeof(u64))    return mx(   u64(std::stoi(cs)));
        else if (type == typeof(i64))    return mx(   i64(std::stoi(cs)));
        else if (type == typeof(u16))    return mx(   u16(std::stoi(cs)));
        else if (type == typeof(i16))    return mx(   i16(std::stoi(cs)));
        else if (type == typeof(u8))     return mx(    u8(std::stoi(cs)));
        else if (type == typeof(i8))     return mx(    i8(std::stoi(cs)));
        console.fault("unknown numeric conversion");
        return mx();
    /// float / double
    } else if (type->traits & traits::realistic) {
        if (type == typeof(float))       return mx( float(std::stod(cs)));
        else if (type == typeof(double)) return mx(double(std::stod(cs)));
        console.fault("unknown real type conversion");
        return mx();
    } else {
        /// enums handle this by implementing ctr_type (generic conversion)
        bool mx_based = (type->traits & traits::mx_obj);
        sz_t len = strlen(cs);
        static memory *dont_hold = memory::raw_alloc(typeof(char), 0, len, len+1); /// memory* can be passed to const mx&
        void *alloc = type->f.str(dont_hold); // it needs to be of type_t
        assert(dont_hold->refs == 1); /// this one should never have its ref count change from 1
        memory *mem;
        if (mx_based) {
            mem = hold(((mx*)alloc)->mem);
            type->f.dtr(alloc);
        } else {
            mem = memory::raw_alloc(type, 0, 1, 0);
            mem->origin = alloc;
        }
        return mem;
    }
}

u64 mx::hash() const {
    if (!mem) return 0;
    type_t type = mem->type;
    void*  k    = mem->origin;
    if (k == null) return 0;
    if (type == ident::char_t) return (u64)djb2(cstr(k));
    if (type == typeof(i64)   || type == typeof(u64))    return (u64)*(u64*)k;
    if (type == typeof(i32)   || type == typeof(u32))    return (u64)*(u32*)k;
    console.fault("implement hash in context for data: %s", { str(type->name) });
    return 0;
}

/// we cannot call the to_string on (this; if we are a direct type, the virtual would be used)
mx mx::to_string() const {
    type_t type = mem->data_type();

    if      (type == typeof(i8) ) return memory::string(std::to_string(*(i8*)  mem->origin));
    else if (type == typeof(i16)) return memory::string(std::to_string(*(i16*) mem->origin));
    else if (type == typeof(i32)) return memory::string(std::to_string(*(i32*) mem->origin));
    else if (type == typeof(i64)) return memory::string(std::to_string(*(i64*) mem->origin));
    else if (type == typeof(u8) ) return memory::string(std::to_string(*(u8*)  mem->origin));
    else if (type == typeof(u16)) return memory::string(std::to_string(*(u16*) mem->origin));
    else if (type == typeof(u32)) return memory::string(std::to_string(*(u32*) mem->origin));
    else if (type == typeof(u64)) return memory::string(std::to_string(*(u64*) mem->origin));
    else if (type == typeof(r32)) return memory::string(std::to_string(*(r32*) mem->origin));
    else if (type == typeof(r64)) return memory::string(std::to_string(*(r64*) mem->origin));
    else if (type == typeof(bool))return memory::string(std::to_string(*(bool*)mem->origin));

    else if  ((type->traits & traits::enum_primitive) && type->ref) {
        int iraw = *(int*)mem->origin;
        memory *res = type->ref->lookup(u64(iraw));
        return res;
    }
    else if (type->f.str) {
        memory *res;
        /// perform memory contruct because
        /// we dont always have full context in mx
        if (type->traits & traits::mx_obj) {
            void *temp = type->ctr_mem(hold(mem));
            res = type->f.str(temp);
            type->dtr(temp);
        } else {
            res = type->f.str(mem->origin);
        }
        return res;
    }
    else if (type->schema && type->schema->bind->data->f.str) {
        /// should assert the schema count is 1
        return type->schema->bind->data->f.str(mem->origin);
    }
    
    else if (type == typeof(char))
        return ion::hold(mem);
    
    static char buf[128];
    const int l = snprintf(buf, sizeof(buf), "%s/%p", type->name, (void*)mem);
    return memory::stringify(cstr(buf), l);
}

/// cstr operator overload
str operator+(cstr cs, str rhs) { return str(cs) + rhs; }

item& liter_item::operator *     () const { return *cur; }

      liter_item::operator item& () const { return *cur; }

void ldata::pop(mx *prev) {
    assert(icount);
    if (prev)
       *prev = ion::hold(ilast->mem);
    remove(-1);
}


doubly::doubly(memory* mem) : mem(mem), data(mem->get<ldata>(0)) { }

doubly::doubly() : doubly(memory::alloc(typeof(ldata))) { }

doubly::doubly(const doubly &ref) : doubly(ion::hold(ref.mem)) { }

doubly::~doubly() {
    ion::drop(mem);
}

item   *ldata::first_item() const { return ifirst; }
item   *ldata:: last_item() const { return ilast; }

mx     &ldata::  first_mx() const { return *(mx*)&ifirst->mem; }
mx     &ldata::   last_mx() const { return *(mx*)&ilast ->mem; }

template <>
str convert_str<str>(const str& s) {
    return s;
}

/// can call invoke, call, etc
mx call(mx lambda, const array &args) {
    type_t lt = lambda.type();
    /// there are things that disallow generic lambda production and thats the use of pointers and references
    /// with these functions the return type can be void or it can be convertible to mx
    /// if the return type falls outside of that, no generic lambda can be called
    /// at the moment it still creates it but that should change
    assert(lt->generic_lambda);
    mx result = (*lt->generic_lambda)(lambda.mem->origin, args);
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

type_t memory::data_type() const {
    return type->schema ? type->schema->bind->data : type;
}

raw_t memory::typed_data(type_t dtype, size_t index) const {
    sz_t res = math::max(reserve, count);
    //static type_t mx_t  = typeof(mx);
    alloc_schema *schema = type->schema;
    if (dtype != type && schema) {
        sz_t voffset = 0;
        for (size_t i = 0; i < schema->bind_count; i++) {
            context_bind &c = schema->composition[i];
            if (c.data == dtype) return (raw_t)&cstr(origin)[voffset * res + c.data->base_sz * index];
            voffset += c.voffset;
        }
        console.fault("type not found in schema: {0}", { str(dtype->name) });
        return (raw_t)null;
    } else
        return (raw_t)(cstr(origin) + dtype->base_sz * index);
}

size_t length(std::ifstream& in) {
    std::streamsize base = in.tellg();
    in.seekg(0, std::ios::end);
    std::streamsize to_end = in.tellg();
    in.seekg(base);
    return to_end - base;
}

size_t idata::size() {
    return schema ? schema->total_bytes : base_sz;
}

memory *idata::lookup(symbol sym) {
    u64 hash   = djb2(cstr(sym));
    mx  symbol = symbols ? symbols->djb2.lookup(hash) : mx();
    return symbol ? ion::hold(symbol) : null;
}

memory *idata::lookup(i64 id) {
    mx symbol = symbols ? symbols->ids.lookup(id) : null;
    return symbol ? ion::hold(symbol) : null;
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

i64 integer_value(memory *mem) {
    symbol   c = mdata<char>(mem, 0);
    bool blank = c[0] == 0;
    while (isalpha(*c))
        c++;
    return blank ? i64(0) : i64(strtol(c, nullptr, 10));
}

memory *drop(memory *mem) {
    if (mem) {
        if (--mem->refs <= 0 && !mem->constant) { /// <= because mx_object does a defer on the actual construction of the container class
            bool managed = mem->attrs & traits::managed;
            if (mem->atts) {
                for (const attachment &att: mem->atts->elements<attachment>())
                    att.release();
                delete mem->atts;
                mem->atts = null;
            }
            if (managed) { /// array does not handle this properly on clear
                mem->clear();
                free64(mem->origin);
            }
            if (mem->shape) {
                delete mem->shape;
                mem->shape = null;
            }
            free64(mem);
        }
        return mem;
    } else
        return null;
}

memory *hold(memory *mem) {
    if (mem)
        mem->refs++;
    return mem;
}

memory* hold(const MX &o) {
    return hold(o.mem);
}
memory* drop(const MX &o) {
    return drop(o.mem);
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

/// attach arbs to memory (uses a pointer)
attachment *memory::attach(ion::symbol id, void *data, func<void()> release) {
    if (!atts)
         atts = new doubly();
    doubly &a = *atts;
    a->push(attachment {id, data, release});
    return &a->last<attachment>();
}

attachment *memory::find_attachment(ion::symbol id) {
    if (!atts) return nullptr;
    /// const char * symbol should work fine for the cases used
    for (attachment &att:atts->elements<attachment>())
        if (id == att.id)
            return &att;
    return nullptr;
}

void *memory::realloc(size_t alloc_reserve, bool fill_default) {
    size_t          type_sz   = type->size(); /// size of actual data consumed -- we dont use functions that are emitted from array<T> we use T in vector form (which they are already)
    u8             *dst       = (u8*)calloc64(alloc_reserve, type_sz);
    u8             *src       = (u8*)origin;
    const sz_t      src_res   = reserve;
    size_t          mn        = math::min(alloc_reserve, count);
    const bool      prim      = !type->schema || (type->traits & (traits::primitive | traits::opaque)) != 0;
    const sz_t      res       = alloc_reserve;

    /// if single primitive, it can be mem copied.  otherwise this is interleaved vector
    if (prim) {
        memcpy(dst, src, type_sz * mn);
    } else {
        bool none = true;
        for (size_t i = 0; i < type->schema->bind_count; i++) {
            context_bind &c  = type->schema->composition[i];
            if (!c.data) continue;
            const bool data_prim = !c.data->schema || (c.data->traits & (traits::primitive | traits::opaque)) != 0;
            if (none)
                none = mn == 0;
            for (size_t ii = 0; ii < mn; ii++) {
                if (data_prim) {
                    memcpy(&dst[c.voffset * res + ii * c.base_sz], &src[c.voffset * src_res + ii * c.base_sz], c.base_sz);
                } else {
                    c.data->f.ctr_cp(&dst[c.voffset * res + ii * c.base_sz], &src[c.voffset * src_res + ii * c.base_sz]); /// copy prior data
                    c.data->f.dtr   (&src[c.voffset * src_res + ii * c.base_sz]); /// destruct prior data
                }
            }
        }
        assert(!none);
    }
    /// this controls the 'count' which is in-effect the constructed count.  if we are not constructing, no updating count
    if (fill_default) {
        count = alloc_reserve;
        if (!prim) {
            for (size_t i = 0; i < type->schema->bind_count; i++) {
                context_bind &c  = type->schema->composition[i];
                if (!(c.data->traits & traits::primitive))
                    for (size_t ii = mn; ii < alloc_reserve; ii++)
                        c.data->f.ctr(&dst[c.voffset * res + ii * c.base_sz]);
            }
        }
    }
    free64(origin);
    origin  = raw_t(dst);
    reserve = alloc_reserve;
    return origin;
}

/// put primitive conversions here, and mx.hpp needs the declarations
memory *_to_string(cstr data) {
    return memory::stringify(data, memory::autolen, 0, false);
}

size_t  _hash(cstr data, size_t count) {
    return djb2(data, count);
}

/// mx-objects are clearable which brings their count to 0 after destruction
void memory::clear() {
    u8 *src = (u8*)origin;
    for (size_t ii = 0; ii < count; ii++) {
        if (type->schema) {
            sz_t voffset = 0;
            for (size_t i = 0; i < type->schema->bind_count; i++) {
                context_bind &c  = type->schema->composition[i];
                c.data->f.dtr(&src[voffset * reserve + ii * c.base_sz]); /// always make sure reserve is set to valid +value
                voffset += c.voffset;
            }
        } else
            type->f.dtr(&src[type->base_sz * ii]);
    }
    count = 0;
}

memory *memory::stringify(cstr cs, size_t len, size_t rsv, bool constant, type_t ctype, i64 id) {
    ion::symbol sym = (ion::symbol)(cs ? cs : "");
    if (constant) {
        if(!ctype->symbols)
            ctype->symbols = new symbol_data { };
        u64  h_sym = djb2(cstr(sym));
        mx &m = ctype->symbols->djb2[h_sym];
        if (!m) {
            size_t ln = (len == memory::autolen) ? strlen(sym) : len; /// like auto-wash
            m.mem = memory::alloc(typeof(char), ln, ln + 1, raw_t(sym));
            m.mem->id = id;
            m.mem->id    = id;
            m.mem->attrs = attr::constant;
            ctype->symbols->ids[id] = m.mem; /// was not hashing by id, it was the djb2 again (incorrect)
            //ctype->symbol_djb2[h_sym] = m; 'redundant due to the setting of the memory*& (which [] operator always inserts item)
            ctype->symbols->list->push(m);
        }
        return ion::hold(m);
    } else {
        size_t     ln = (len == memory::autolen) ? strlen(sym) : len;
        size_t     al = (rsv >= (ln + 1)) ? rsv : (ln + 1);
        memory*   mem = memory::alloc(typeof(char), ln, al, raw_t(sym));
        mem->id       = id;
        cstr  start   = mem->get<char>(0);
        start[ln]     = 0;
        return mem;
    }
}

memory *memory::string (std::string s) { return stringify(cstr(s.c_str()), s.length(), 0, false, typeof(char), 0); }
memory *memory::cstring(cstr s)        { return stringify(cstr(s),         strlen(s),  0, false, typeof(char), 0); }

memory *memory::symbol (ion::symbol s, type_t ty, i64 id) {
    return stringify(cstr(s), strlen(s), 0, true, ty, id);
}

memory *memory::raw_alloc(type_t type, size_t sz, size_t count, size_t res) {
    if (sz == 0 && type)
        sz = type->base_sz;
    memory*     mem = (memory*)calloc64(1, sizeof(memory)); /// there was a 16 multiplier prior.  todo: add address sanitizer support with appropriate clang stuff
    mem->count      = count;
    mem->reserve    = math::max(res, count);
    mem->refs       = 1; /// inc on construction for memory*
    mem->type       = type;
    mem->origin     = sz ? calloc64(mem->reserve, sz) : null; /// was doing inline origin.  its useful prior to realloc but adds complexity; can add back when optimizing
    return mem;
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

memory *memory::alloc(type_t type, size_t count, size_t reserve, raw_t v_src) {
    if (type->singleton)
        return ion::hold(type->singleton);
    
    size_t  type_sz = type->size(); /// this is the 'data size', should name the function just that; if the type has no schema the data size is its own size
    sz_t    res = math::max(count, reserve);
    memory *mem = memory::raw_alloc(type, type_sz, count, res);

    if (type->traits & traits::singleton)
        type->singleton = mem;
    bool primitive = (type->traits & traits::primitive);

    /// if allocating a schema-based object (mx being first user of this)
    if (count > 0) {
        if (primitive) {
            if (v_src)
                memcpy(mem->origin, v_src, type_sz * count);
            else if (type->f.ctr) {
                for (size_t ii = 0; ii < count; ii++) {
                    u8 *dst = &((u8*)mem->origin)[type_sz * ii];
                    type->f.ctr(dst);
                }
            }
        } else if (type->schema) {
            /// if schema-copy-construct (call cpctr for each data type in composition)
            for (size_t i = 0; i < type->schema->bind_count; i++) {
                context_bind &bind = type->schema->composition[i];
                if (!bind.data) continue;
                u8 *dst = &((u8*)mem->origin)[bind.voffset * res];
                if (bind.data->traits & traits::primitive) {
                    if (v_src) {
                        u8 *src = &((u8*)v_src)[bind.voffset * res];
                        memcpy(dst, src, type_sz * count); /// better to do this over the entire vector when we can, for primitive-structs
                    }
                } else {
                    if (v_src) {
                        assert (bind.data->f.ctr_cp);
                        u8 *src = &((u8*)v_src)[bind.voffset * res];
                        for (size_t ii = 0; ii < count; ii++)
                            bind.data->f.ctr_cp(&dst[ii * type_sz], &src[ii * type_sz]);
                    } else if (bind.data->f.ctr)
                        for (size_t ii = 0; ii < count; ii++)
                            bind.data->f.ctr(&dst[ii * type_sz]);
                }
            }
        } else {
            if (v_src)
                type->f.ctr_cp(mem->origin, v_src);
            else if (type->f.ctr)
                type->f.ctr(mem->origin);
        }
    }
    return mem;
}

memory *memory::copy(size_t reserve) {
    memory *a   = this;
    memory *res = alloc(a->type, a->count, reserve, a->origin);
    return  res;
}

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

memory* mem_symbol(ion::symbol cs, type_t ty, i64 id) {
    return memory::symbol(cs, ty, id);
}

void *mem_origin(memory *mem) {
    return mem->origin;
}

memory *cstring(cstr cs, size_t len, size_t reserve, bool is_constant) {
    ident::init();
    return memory::stringify(cs, len, 0, is_constant, typeof(char), 0);
}

u8* get_member_address(type_t type, raw_t data, str &name, prop *&rprop) {
    if (type->meta && type->meta_map) {
        mx sym = name.symbolize();
        mx p = type->meta_map->lookup(sym);
        if (p) { /// gets prop position for this struct data
            rprop = (prop *)p.mem->origin;
            u8 *p_value = &(((u8*)data)[rprop->offset]);
            return p_value;
        }
    }
    return null;
}

/// find schema-bound meta property from memory and field
u8* property_find(void *origin, type_t type, str &name, prop *&rprop) {
    u8  *pos = (u8*)origin;
    type_t t =      type;
    assert(name.len() > 0);
    if (t->schema)
        for (int i = 0; i < t->schema->bind_count; i++) {
            context_bind &c = t->schema->composition[i];
            prop*    member = null;
            u8*      addr   = get_member_address(c.data, pos, name, member);
            if (addr) {
                rprop = member;
                return addr;
            }
            pos += c.data->base_sz;
        }
    rprop = null;
    return null;
}

raw_t mx::find_prop_value(str name, prop *&rprop) {
    return property_find(mem->origin, mem->type, name, rprop);
}

bool get_bool(type_t type, raw_t data, str &name) {
    prop  *p;
    u8    *p_value = get_member_address(type, data, name, p);
    assert(p_value);
    bool   result  = p->type->f.boolean(p_value);
    return result;
}

memory *get_string(type_t type, raw_t data, str &name) {
    prop   *p;
    u8     *p_value = get_member_address(type, data, name, p);
    assert(p_value);
    memory *m       = p->type->f.str(p_value);
    return  m;
}

}