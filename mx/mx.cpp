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

raw_t memory::typed_data(type_t dtype, size_t index) const {
    size_t mxc = math::max(reserve, count);
    //static type_t mx_t  = typeof(mx);
    alloc_schema *schema = type->schema;
    if (dtype != type && schema) {
        for (size_t i = 0; i < schema->bind_count; i++) {
            context_bind &c = schema->composition[i];
            if (c.data == dtype)
                return (raw_t)&cstr(origin)[c.offset * mxc + c.data->base_sz * index];
        }
        //console.fault("type not found in schema: {0}", { str(dtype->name) });
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

str path::mime_type() {
    mx e = ext().mid(1).symbolize();
    static path data = "data/mime-type.json";
    static map  js   =  data.read<var>();
    field      *find = js->lookup(e);
    return find ? ion::hold(find->v) : ion::hold(js["default"]);
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
            mem->origin = null;
            // delete attachment lambda after calling it
            bool managed = mem->attrs & traits::managed;
            if (mem->atts) {
                for (const attachment &att: mem->atts->elements<attachment>())
                    att.release();
                delete mem->atts;
                mem->atts = null;
            }
            if (managed)
                free64(mem->origin);
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

memory* hold(const mx &o) {
    return hold(o.mem);
}
memory* drop(const mx &o) {
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
    size_t          mn        = math::min(alloc_reserve, count);
    const bool      prim      = !type->schema || (type->traits & (traits::primitive | traits::opaque)) != 0;

    /// if single primitive, it can be mem copied.  otherwise this is interleaved vector
    if (prim) {
        memcpy(dst, src, type_sz * mn);
    } else {
        for (size_t i = 0; i < type->schema->bind_count; i++) {
            context_bind &c  = type->schema->composition[i];
            for (size_t ii = 0; ii < mn; ii++) {
                const bool data_prim = !c.data->schema || (c.data->traits & (traits::primitive | traits::opaque)) != 0;
                if (data_prim) {
                    memcpy(&dst[c.offset + ii * type_sz], &src[c.offset + ii * type_sz], type_sz);
                } else {
                    c.data->functions->copy    ((none*)null, (none*)&dst[c.offset + ii * type_sz], (none*)&src[c.offset + ii * type_sz]); /// copy prior data
                    c.data->functions->destruct((none*)null, (none*)&src[c.offset + ii * type_sz]); /// destruct prior data
                }
            }
        }
    }
    /// this controls the 'count' which is in-effect the constructed count.  if we are not constructing, no updating count
    if (fill_default) {
        count = alloc_reserve;
        if (!prim) {
            for (size_t i = 0; i < type->schema->bind_count; i++) {
                context_bind &c  = type->schema->composition[i];
                if (!(c.data->traits & traits::primitive))
                    for (size_t ii = mn; ii < alloc_reserve; ii++)
                        c.data->functions->construct((none*)null, (none*)&dst[c.offset + ii * type_sz]);
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
    alloc_schema *mx  = type->schema;
    u8           *dst = (u8*)origin;
    if (mx) {
        for (size_t i = 0; i < mx->bind_count; i++) { /// count should be called bind_count or something; its too ambiguous with memory
            context_bind &c  = mx->composition[i];
            for (size_t ii = 0; ii < count; ii++)
                c.data->functions->destruct((none*)null, (none*)&dst[c.offset + ii * mx->total_bytes]);
        }
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

static int _raw_alloc_count = 0;

memory *memory::raw_alloc(type_t type, size_t sz, size_t count, size_t res) {
    memory*     mem = (memory*)calloc64(1, sizeof(memory)); /// there was a 16 multiplier prior.  todo: add address sanitizer support with appropriate clang stuff
    mem->count      = count;
    mem->reserve    = math::max(res, count);
    mem->refs       = 1; /// inc on construction for memory*
    mem->type       = type;
    mem->origin     = sz ? calloc64(sz, math::max(res, count)) : null; /// was doing inline origin.  its useful prior to realloc but adds complexity; can add back when optimizing
    _raw_alloc_count++;
    return mem;
}

int memory::raw_alloc_count() {
    return _raw_alloc_count;
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

bool path::get_modified_date(struct tm *res) {
    const char* filename = cs();
    struct stat file_stat;

    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        return false;
    }
    static mutex mx;
    mx.lock();
    *res = *gmtime(&file_stat.st_mtime);
    mx.unlock();
    return true;
}

str path::get_modified_string() {
    str result(size_t(128));
    struct tm gm_time;
    ///
    if (get_modified_date(&gm_time)) {
        // RFC 1123 format (used with http)
        if (strftime(result.data, result.reserve(), "%a, %d %b %Y %H:%M:%S GMT", &gm_time) == 0)
            fprintf(stderr, "strftime failed\n");
    }
    return result;
}

memory *memory::alloc(type_t type, size_t count, size_t reserve, raw_t v_src) {
    if (type->singleton)
        return ion::hold(type->singleton);
    
    size_t  type_sz = type->size(); /// this is the 'data size', should name the function just that; if the type has no schema the data size is its own size
    memory *mem = memory::raw_alloc(type, type_sz, count, reserve);

    if (type->traits & traits::singleton)
        type->singleton = mem;
    bool primitive = (type->traits & traits::primitive);

    /// if allocating a schema-based object (mx being first user of this)
    if (count > 0) {
        if (v_src) {
            if (primitive)
                memcpy(mem->origin, v_src, type_sz * count);
            else {
                /// if schema-copy-construct (call cpctr for each data type in composition)
                for (size_t i = 0; i < type->schema->bind_count; i++) {
                    context_bind &bind = type->schema->composition[i];
                    u8 *dst = &((u8*)mem->origin)[bind.offset];
                    u8 *src = &((u8*)      v_src)[bind.offset];
                    if (bind.data) {
                        for (size_t ii = 0; ii < count; ii++)
                            if (bind.data->traits & traits::primitive)
                                memcpy(&dst[ii * type_sz], &src[ii * type_sz], type_sz);
                            else
                                bind.data->functions->copy((none*)null, (none*)&dst[ii * type_sz], (none*)&src[ii * type_sz]);
                    }
                }
            }
        } else if (!primitive) {
            if (!type->schema) {
                /// restrict to structs with no initialization needed and simple bit copies suffice
                size_t type_sz = type->base_sz;
                u8 *dst = &((u8*)mem->origin)[0];
                if (v_src) {
                    u8 *src = &((u8*)v_src)[0];
                    memcpy(dst, src, count * type_sz);
                } else {
                    memset(dst, 0, count * type_sz);
                }
            } else {
                for (size_t i = 0; i < type->schema->bind_count; i++) {
                    context_bind &bind = type->schema->composition[i];
                    u8 *dst  = &((u8*)mem->origin)[bind.offset];
                    if (bind.data && !(bind.data->traits & traits::primitive)) {
                        for (size_t ii = 0; ii < count; ii++) {
                            bind.data->functions->construct((none*)null, (none*)&dst[ii * type_sz]);
                            
                            /// allow for trivial construction and subsequent init; this alllows one to use .fields and others,
                            /// and still have an init.
                            if (bind.data->traits & traits::init)
                                bind.data->functions->init((none*)&dst[ii * type_sz]); 
                                /// this may need to be called in the case where the context type has one too
                        }
                    }
                }
            }
        }
    }
    return mem;
}

memory *memory::copy(size_t reserve) {
    memory *a   = this;
    memory *res = alloc(a->type, a->count, reserve, a->origin);
    return  res;
}

size &size::operator=(const size b) {
    memcpy(values, b.values, sizeof(values));
    count = b.count;
    return *this;
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
    return memory::stringify(cs, len, 0, is_constant, typeof(char), 0);
}

idata *type_cache::lookup(str &name) {
    mx  sym  = name.symbolize();
    mx  info = (*type_cache::type_map)[sym];
    return info.type(); /// mapping with unintialized singular alloc
}

void type_cache::hash_type(type_t type) {
    str type_name = type->name;
    mx  sym = type_name.symbolize();
    if (!type_cache::type_map)
         type_cache::type_map = new hashmap(64);
    mx info = memory::alloc(type, 0, 0); /// create uninitialized
    type_cache::type_map->set(sym, info);
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
    bool   result  = p->type->functions->boolean((none*)null, (none*)p_value);
    return result;
}

memory *get_string(type_t type, raw_t data, str &name) {
    prop   *p;
    u8     *p_value = get_member_address(type, data, name, p);
    if (!p_value)
        return null;
    memory *m       = p->type->functions->to_string((none*)p_value);
    return  m;
}

memory  *mx::to_string() const {
    type_t type = mem->type->schema ? mem->type->schema->bind->data : mem->type;
    /// will likely need to iterate through the 2 types (in case of enumerable)

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
    else if   (type->functions->to_string)
        return type->functions->to_string((none*)mem->origin); /// call to_string() on context class
    
    else   if (type->schema &&
               type->schema->bind->data->functions &&
               type->schema->bind->data->functions->to_string)
        return type->schema->bind->data->functions->to_string((none*)mem->origin); /// or data...
    
    else if (type == typeof(char))
        return ion::hold(mem);
    
    else {
        static char buf[128];
        const int l = snprintf(buf, sizeof(buf), "%s/%p", type->name, (void*)mem);
        return memory::stringify(cstr(buf), l);
    }
}

}