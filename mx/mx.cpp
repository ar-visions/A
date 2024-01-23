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

mx call(mx lambda, array<str> args) {
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
        size_t offset = 0;
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
    u64 hash = djb2(cstr(sym));
    memory **result = symbols ? symbols->djb2.lookup(hash) : null;
    return   result ? *result : null;
}

memory *idata::lookup(i64 id) {
    memory **result = symbols ? symbols->ids.lookup(id) : null;
    return   result ? *result : null;
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
    static path  data = "data/mime-type.json";
    static map<mx> js =  data.read<var>();
    field<mx> *find = js->lookup(e);
    return find ? find->value.hold() : ((memory*)js["default"])->hold();
}

i64 integer_value(memory *mem) {
    symbol   c = mdata<char>(mem, 0);
    bool blank = c[0] == 0;
    while (isalpha(*c))
        c++;
    return blank ? i64(0) : i64(strtol(c, nullptr, 10));
}

memory *drop(memory *mem) {
    if (mem) mem->drop();
    return mem;
}

memory *hold(memory *mem) {
    if (mem) mem->hold();
    return mem;
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
         atts = new doubly<attachment>();
    doubly<attachment> &a = *atts;
    a->push(attachment {id, data, release});
    return &a->last();
}

attachment *memory::find_attachment(ion::symbol id) {
    if (!atts) return nullptr;
    /// const char * symbol should work fine for the cases used
    for (attachment &att:*atts)
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
                    c.data->functions->copy    (raw_t(0), &dst[c.offset + ii * type_sz], &src[c.offset + ii * type_sz]); /// copy prior data
                    c.data->functions->destruct(raw_t(0), &src[c.offset + ii * type_sz]); /// destruct prior data
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
                        c.data->functions->construct(raw_t(0), &dst[c.offset + ii * type_sz]);
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
                c.data->functions->destruct(raw_t(0), &dst[c.offset + ii * mx->total_bytes]);
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
        memory *&m = ctype->symbols->djb2[h_sym];
        if (!m) {
            size_t ln = (len == memory::autolen) ? strlen(sym) : len; /// like auto-wash
            m = memory::alloc(typeof(char), ln, ln + 1, raw_t(sym));
            m->id    = id;
            m->attrs = attr::constant;
            ctype->symbols->ids[id] = m; /// was not hashing by id, it was the djb2 again (incorrect)
            //ctype->symbol_djb2[h_sym] = m; 'redundant due to the setting of the memory*& (which [] operator always inserts item)
            ctype->symbols->list->push(m);
        }
        return m->hold();
    } else {
        size_t     ln = (len == memory::autolen) ? strlen(sym) : len;
        size_t     al = (rsv >= (ln + 1)) ? rsv : (ln + 1);
        memory*   mem = memory::alloc(typeof(char), ln, al, raw_t(sym));
        mem->id       = id;
        cstr  start   = mem->data<char>(0);
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
    size_t elements = math::max(count, res);
    memory*     mem = (memory*)calloc64(1, sizeof(memory)); /// there was a 16 multiplier prior.  todo: add address sanitizer support with appropriate clang stuff
    mem->count      = count;
    mem->reserve    = res;//math::max(res, count);
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

/// starting at 1, it should remain active.  shall not be freed as a result
void memory::drop() {
    if (--refs <= 0 && !constant) { /// <= because mx_object does a defer on the actual construction of the container class
        origin = null;
        // delete attachment lambda after calling it
        if (atts) {
            for (attachment &att: *atts)
                att.release();
            delete atts;
            atts = null;
        }
        if (managed)
            free64(origin);
        if (shape) {
            delete shape;
            shape = null;
        }
        free64(this);
    }
}

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
    size_t  type_sz = type->size(); /// this is the 'data size', should name the function just that; if the type has no schema the data size is its own size

    if (type->singleton)
        return type->singleton->hold();
    
    memory *mem = memory::raw_alloc(type, type_sz, count, reserve);

    if (type->traits & traits::singleton)
        type->singleton = mem;
    bool primitive = (type->traits & traits::primitive);
    bool has_init  = (type->traits & traits::init);

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
                                bind.data->functions->copy(raw_t(0), &dst[ii * type_sz], &src[ii * type_sz]);
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
                            bind.data->functions->construct(raw_t(0), &dst[ii * type_sz]);
                            
                            /// allow for trivial construction and subsequent init; this alllows one to use .fields and others,
                            /// and still have an init.
                            if (bind.data->traits & traits::init)
                                bind.data->functions->init(&dst[ii * type_sz]); 
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

memory *memory::hold() {
    if ((bool)(void*)this) refs++; // this is basically fine to do and by basically i mean probably and by probably i mean dont show this to forum people
    return this;
}

size &size::operator=(const size b) {
    memcpy(values, b.values, sizeof(values));
    count = b.count;
    return *this;
}


bool chdir(std::string c) {
#if defined(_WIN32)
    return SetCurrentDirectory(c.c_str());
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

idata *types::lookup(str &name) {
    memory *sym = name.symbolize();
    type_t type = (*types::type_map)[(symbol)sym->origin];
    return type;
}

void types::hash_type(type_t type) {
    str  type_name = type->name;
    memory    *sym = type_name.symbolize(); // issue with using symbol table on type bootstrap; the hmap and doubly would have to set flags to avoid the schema registration
    if (!types::type_map)
         types::type_map = new hmap<ion::symbol, type_t>(64);
    type_t &n_data = (*types::type_map)[(symbol)sym->origin];
    n_data = type;
}

u8* get_member_address(type_t type, raw_t data, str &name, prop *&rprop) {
    if (type->meta && type->meta_map) {
        memory *sym = name.symbolize();
        prop  **p   = type->meta_map->lookup((symbol)sym->origin);
        if (p) {
            rprop       = *p;
            u8 *p_value = &(((u8*)data)[(*p)->offset]);
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
    bool   result  = p->type->functions->boolean(null, p_value);
    return result;
}

memory *get_string(type_t type, raw_t data, str &name) {
    prop   *p;
    u8     *p_value = get_member_address(type, data, name, p);
    if (!p_value)
        return null;
    memory *m       = p->type->functions->to_string(p_value);
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
        return type->functions->to_string(mem->origin); /// call to_string() on context class
    
    else   if (type->schema &&
               type->schema->bind->data->functions &&
               type->schema->bind->data->functions->to_string)
        return type->schema->bind->data->functions->to_string(mem->origin); /// or data...
    
    else if (type == typeof(char))
        return mem->hold();
    
    else {
        static char buf[128];
        const int l = snprintf(buf, sizeof(buf), "%s/%p", type->name, (void*)mem);
        return memory::stringify(cstr(buf), l);
    }
}



void var::push(mx v) {        array<mx>((mx*)this).push(v); }
mx  &var::last()     { return array<mx>((mx*)this).last();  }

mx var::parse_obj(cstr *start, type_t type) {
    /// supports primitive, meta-bound structs and mx objects with data type
    bool       is_map = !type || (type->traits & traits::map);
    mx      mx_result;
    map<mx>  m_result;

    if (!is_map) {
        void *alloc = type->functions->alloc_new(null, null);
        /// if its an mx-based object, we need to get its memory
        if (type->traits & traits::mx_obj) {
            memory *mem = ((mx*)alloc)->mem->hold();
            mx_result = mx(mem);
            type->functions->del(null, alloc);
        } else
            mx_result = memory::wrap(type, alloc, 1);
    }

    /// if type is not mx-based, lets say its an int type, we must wrap this parse result
    cstr cur = *start;
    assert(*cur == '{');
    ws(&(++cur));

    type_t p_type = (type && !is_map) ? type->meta_lookup() : null;
    cstr test1;

    /// read this object level, parse_values work recursively with 'cur' updated
    while (*cur != '}') {
        /// parse next field name
        cstr prev = cur;
        mx parsed = parse_quoted(&cur, null);
        mx field = mem_symbol((symbol)parsed.mem->origin);

        /// assert field length, skip over the ':'
        ws(&cur);
        assert(field);
        assert(*cur == ':');
        ws(&(++cur));

        type_t f_type = null;
        prop** p = null;
        
        //assert(!p_type || (p_type->meta_map && (p = p_type->meta_map->lookup((symbol)field.mem->origin, null, null))));
        type_t chosen_t = null;
        if (p_type) {
            p = p_type->meta_map->lookup((symbol)field.mem->origin, null, null);
            chosen_t = (*p)->type;
        }

        /// parse value at newly listed mx value in field, upon requesting it
        *start   = cur;
        mx value = parse_value(start, chosen_t);
        if (is_map)
            m_result[field] = value;
        else
            mx_result.set_meta(field, value);
        
        cur = *start;
        /// skip ws, continue past ',' if there is another
        if (ws(&cur) == ',')
            ws(&(++cur));
    }

    /// assert we arrived at end of block
    assert(*cur == '}');
    ws(&(++cur));

    *start = cur;
    if (is_map)
        mx_result = m_result.hold();
    return mx_result;
}

/// no longer will store compacted data
mx var::parse_arr(cstr *start, type_t type) {
    type_t   e_type = type ? type->schema->bind->data : null;
    void *container = null;
    array<mx> a_result;

    if (type) {
        container = type->functions->alloc_new(null, null);
    } else
        a_result = array<mx>();

    cstr cur = *start;
    assert(*cur == '[');
    ws(&(++cur));
    if (*cur == ']')
        ws(&(++cur));
    else {
        for (;;) {
            *start = cur;
            mx  pv = parse_value(start, e_type);
            if (container)
                type->functions->push(container, pv.mem);
            else
                a_result += pv;
            cur = *start;
            ws(&cur);
            if (*cur == ',') {
                ws(&(++cur));
                continue;
            }
            else if (*cur == ']') {
                ws(&(++cur));
                break;
            }
            assert(false);
        }
    }
    *start = cur;
    if (container) {
        memory *mem = ((mx*)container)->mem->hold();
        type->functions->del(null, container);
        return mx(mem);
    }
    return a_result;
}

void var::skip_alpha(cstr *start) {
    cstr cur = *start;
    while (*cur && isalpha(*cur))
        cur++;
    *start = cur;
}

mx var::parse_value(cstr *start, type_t type) {
    char first_chr = **start;
    bool numeric   =   first_chr == '-' || isdigit(first_chr);

    if (first_chr == '{') {
        /// type must have meta info
        /// issue is how to automatically prefer context, but still fall back to data
        assert(!type || type->meta_lookup());
        return parse_obj(start, type);
    } else if (first_chr == '[') {
        /// simple runtime check for array
        assert(!type || type->traits & traits::array);
        return parse_arr(start, type); /// needs to pass in array<Test3> not Test3, unless we can get the type back
    } else if (first_chr == 't' || first_chr == 'f') {
        assert(!type || type == typeof(bool));
        bool   v = first_chr == 't';
        skip_alpha(start);
        return mx(mx::alloc(&v));
    } else if (first_chr == '"') {
        assert(!type || (type == typeof(str) || type->functions->from_string));
        symbol    b64 = "data:application/octet-stream;base64,";
        int      blen = strlen(b64);
        symbol start1 = (symbol)&(*start)[1];
        bool   is_b64 = strncmp(start1, b64, blen) == 0;
        if (is_b64)
            *start += 1 + blen;
        
        mx ret;
        if (!is_b64 && type == typeof(array<u8>)) {
            mx file = parse_quoted(start, null);
            FILE *f = fopen(file.origin<char>(), "rb");
            fseek(f, 0, SEEK_END);
            ulong sz = ftell(f);
            u8   *b  = (u8*)malloc(sz);
            fseek(f, 0, SEEK_SET);
            assert(fread(b, 1, sz, f) == sz);
            ret = memory::wrap(typeof(u8), b, sz, true);
        } else
            ret = parse_quoted(start, is_b64 ? typeof(array<u8>) : type); /// this updates the start cursor

        return ret;
    } else if (numeric) {
        bool floaty;
        str value = parse_numeric(start, floaty);
        assert(value != "");

        if (floaty) {
            real v = value.real_value<real>();
            if (type) {
                assert(type == typeof(float) || type == typeof(double));
                if (type == typeof(float)) {
                    float v32 = (float)v;
                    return mx::alloc(&v32);
                }
                return mx::alloc(&v);
            }
            return mx::alloc(&v);
        } else if (type && (type->traits & traits::mx_enum)) {
            void *temp = type->functions->from_string(null, value.cs());
            memory *mem = ((mx*)temp)->hold();
            type->functions->del(null, temp);
            return mem;
        }

        i64 v = value.integer_value();
        if (type) {
            if (type == typeof(char)) { char vc  = (char)v; return mx::alloc(&vc);   }
            if (type == typeof(i8))   { i8  vi8  = (i8)v;   return mx::alloc(&vi8);  }
            if (type == typeof(u8))   { u8  vu8  = (u8)v;   return mx::alloc(&vu8);  }
            if (type == typeof(i16))  { i16 vi16 = (i16)v;  return mx::alloc(&vi16); }
            if (type == typeof(u16))  { u16 vu16 = (u16)v;  return mx::alloc(&vu16); }
            if (type == typeof(i32))  { i32 vi32 = (i32)v;  return mx::alloc(&vi32); }
            if (type == typeof(u32))  { u32 vu32 = (u32)v;  return mx::alloc(&vu32); }
            if (type == typeof(i64))  { i64 vi64 = (i64)v;  return mx::alloc(&vi64); }
            if (type == typeof(u64))  { u64 vu64 = (u64)v;  return mx::alloc(&vu64); }
            if (type == typeof(size_t))  { size_t sz64 = (size_t)v; return mx::alloc(&sz64); }

            if (type == typeof(r32))  { r32 vr32 = (r32)v;  return mx::alloc(&vr32); }
            if (type == typeof(r64))  { r64 vr64 = (r64)v;  return mx::alloc(&vr64); }
            assert(false);
        }
        return mx::alloc(&v);
    } 

    /// symbol can be undefined or null.  stored as instance of null_t
    skip_alpha(start);
    return memory::alloc(typeof(null_t), 1, 1, null);
}

str var::parse_numeric(cstr * cursor, bool &floaty) {
    cstr  s = *cursor;
    floaty  = false;
    if (*s != '-' && !isdigit(*s))
        return "";
    ///
    const int max_sane_number = 128;
    cstr      number_start = s;
    ///
    for (++s; ; ++s) {
        if (*s == '.' || *s == 'e' || *s == '-') {
            floaty = true;
            continue;
        }
        if (!isdigit(*s))
            break;
    }
    ///
    size_t number_len = s - number_start;
    if (number_len == 0 || number_len > max_sane_number)
        return null;
    
    /// 
    *cursor = &number_start[number_len];
    return str(number_start, int(number_len));
}

/// \\ = \ ... \x = \x
mx var::parse_quoted(cstr *cursor, type_t type) {
    symbol first = *cursor;
    str    result { size_t(256) };

    if (type == typeof(array<u8>)) {
        cstr start = *cursor; /// no quote
        /// this is hacky but the string is offset from the "data:base64/header,base64,[here]
        /// so it does NOT have the start quote (has end quote).
        /// read until we see the ".  nothing else to do for base64 (not hacky!)
        *cursor = strchr(*cursor, '"');
        size_t len = std::distance(start, *cursor);
        result = str(size_t(len));
        memcpy(result.data, start, len);
        result.mem->count = len;
        result.data[len] = 0;
        (*cursor)++;
    } else if (*first == '"') {
        ///
        char         ch = 0;
        bool last_slash = false;
        cstr      start = ++(*cursor);
        
        size_t     read = 0;
        ///
        
        for (; (ch = start[read]) != 0; read++) {
            if (ch == '\\')
                last_slash = !last_slash;
            else if (last_slash) {
                switch (ch) {
                    case 'n': ch = '\n'; break;
                    case 'r': ch = '\r'; break;
                    case 't': ch = '\t'; break;
                    ///
                    default:  ch = ' ';  break;
                }
                last_slash = false;
            } else if (ch == '"') {
                read++; /// make sure cursor goes where it should, after quote, after this parse
                break;
            }
            
            if (!last_slash)
                result += ch;
        }
        *cursor = &start[read];
    }
    
    if (type) {
        cstr cs_result = result.cs();
        if (type == typeof(array<u8>)) {
            array<u8> buffer = base64::decode(cs_result, result.len());
            return buffer;
        } else {
            /// must contain a cstr or symbol constructor
            assert(type->functions->from_string);
            void *v_result = type->functions->from_string(null, result.cs());
            if (type->traits & traits::mx_obj) {
                return ((mx*)v_result)->hold();
            }
            return memory::wrap(type, v_result);
        }

    }
    return result;
}

char var::ws(cstr *cursor) {
    cstr s = *cursor;
    while (isspace(*s))
        s++;
    *cursor = s;
    if (*s == 0)
        return 0;
    return *s;
}

var::operator std::string() {
    if(mem->type == typeof(char))
        return std::string((symbol)mem->origin, mem->count);
    return "";
}

mx *var::get(str key) {
    if (mem->type != typeof(map<mx>))
        return null;
    map<mx>::mdata *m = (map<mx>::mdata*)mem->origin;
    field<mx> *f = m->lookup(key);
    return f ? &f->value : null;
}

/// called from path::read<var>()
mx var::parse(cstr js, type_t type) {
    return parse_value(&js, type);
}

str var::stringify() const {
    auto encode_str = [](memory *m) -> str {
        str res(m->count * 2);
        char *s = (char*)m->origin;
        while (*s) {
            if ((*s & 0b10000000) == 0) {
                if      (*s == '\r') res += "\\r";
                else if (*s == '\n') res += "\\n";
                else if (*s == '\"') res += "\\\"";
                else if (*s ==    8) res += "\\b";
                else if (*s ==    9) res += "\\t";
                else if (*s == 0x0C) res += "\\f";
                else res += *s;
                s++;
            } else {
                bool i4 = (*s & 0b1111100) == 0b1111000;
                bool i3 = (*s & 0b1111000) == 0b1110000;
                bool i2 = (*s & 0b1110000) == 0b1100000;
                int code = 0;
                if (i4) {
                    assert(s[1] && s[2] && s[3]);
                    code = int(0b00000111 & s[0]) << 18 |
                            int(0b00111111 & s[1]) << 12 |
                            int(0b00111111 & s[2]) <<  6 |
                            int(0b00111111 & s[3]) <<  0;
                    s += 4;
                } else if (i3) {
                    assert(s[1] && s[2]);
                    code = int(0b00001111 & s[0]) << 12 |
                            int(0b00111111 & s[1]) <<  6 |
                            int(0b00111111 & s[2]) <<  0;
                    s += 3;
                } else if (i2) {
                    assert(s[1]);
                    code = int(0b00011111 & s[0]) <<  6 |
                            int(0b00111111 & s[1]) <<  0;
                    s += 2;
                }
                char buf[32];
                snprintf(buf, sizeof(buf), "%04x", code);
                res += buf;
            }
        }
        return res;
    };

    /// main recursion function
    lambda<str(const mx&)> fn;
    fn = [&](const mx &i) -> str {

        /// used to output specific mx value -- can be any, may call upper recursion
        auto format_v = [&](const mx &e) {
            type_t  t = e.type();
            type_t vt = t->schema ? t->schema->bind->data : t;

            /// check if this is an object with meta data; then we can describe with the above
            properties *meta = vt->meta;
            if (meta && *meta)
                return fn(e);
            
            str res(size_t(1024));
            if (!e.mem || vt == typeof(null_t))
                res += "null";
            else {
                if (vt == typeof(mx) || vt == typeof(map<mx>))
                    res += fn(e);
                else {
                    memory *smem = e.to_string();
                    type_t  mt   = smem ? smem->type : null;
                    if (!smem || smem->type == typeof(null_t))
                        res += "null";
                    else if (vt->traits & (traits::integral | traits::realistic))
                        res += smem; /// string of the number, no quotes!
                    else if (mt == typeof(char) || mt == typeof(const char)) {
                        res += "\"";
                        res += encode_str(smem);
                        res += "\"";
                    }
                    else
                        assert(false);
                }
            }
            return res;
        };
        ///
        str ar(size_t(1024));
        type_t t = i.type();

        /// top level fields output for meta-enabled object
        doubly<prop> *meta = t->schema ? t->schema->bind->data->meta : null;
        if (meta && *meta) {
            if (i) {
                ar += "{";
                size_t n_fields = 0;
                for (prop &p: *meta) {
                    str &skey = *p.s_key;
                    mx  value = i.get_meta(skey.hold());
                    if (n_fields)
                        ar += ",";
                    ar += "\"";
                    ar += skey;
                    ar += "\"";
                    ar += ":";
                    ar += format_v(value);
                    n_fields++;
                }
                ar += "}";
            } else {
                ar += "null";
            }
        } else if (t == typeof(map<mx>)) {
            map<mx> m = map<mx>(i);
            ar += "{";
            size_t n_fields = 0;
            for (field<mx> &fx: m) {
                str skey = str(fx.key.mem->hold());
                if (n_fields)
                    ar += ",";
                ar += "\"";
                ar += skey;
                ar += "\"";
                ar += ":";
                ar += format_v(fx.value);
                n_fields++;
            }
            ar += "}"; /// needs array output if below doesnt cover
        } else if (t == typeof(mx)) {
            mx *mx_p = i.mem->data<mx>(0);
            ar += "[";
            for (size_t ii = 0; ii < i.count(); ii++) {
                mx &e = mx_p[ii];
                if (ii)
                    ar += ",";
                ar += format_v(e);
            }
            ar += "]";
        } else
            ar += format_v(i);

        return ar;
    };

    return fn(*this);
}

map<mx> var::items() {
    if (mem->type->traits & traits::map)
        return mem->hold();
    return map<mx>();
}

array<mx> var::list() {
    if (mem->type->traits & traits::array)
        return mem->hold();
    return array<mx>();
}

/// default constructor constructs map
var::var()          : mx(mx::alloc<map<mx>>()) { } /// var poses as different classes.
var::var(mx b)      : mx(b.mem->hold()) { }
var::var(map<mx> m) : mx(m.mem->hold()) { }

mx var::json(mx i, type_t type) {
    type_t ty = i.type();
    cstr   cs = null;
    if (ty == typeof(u8))
        cs = cstr(i.data<u8>());
    else if (ty == typeof(i8))
        cs = cstr(i.data<i8>());
    else if (ty == typeof(char))
        cs = cstr(i.data<char>());
    else {
        console.fault("unsupported type: {0}", { str(ty->name) });
        return null;
    }
    return parse(cs, type);
}

memory *var::string() {
    return stringify().hold(); /// output should be ref count of 1.
}

var::operator str() {
    assert(type() == typeof(char));
    return is_string() ? str(mx::mem->hold()) : str(stringify());
}

var::operator cstr() {
    assert(is_string()); /// this operator should not allocate data
    return mem->data<char>(0);
}

var::operator i64() {
    type_t t = type();
    if (t == typeof(char)) {
        return str(mx::mem->hold()).integer_value();
    } else if (t == typeof(real)) {
        i64 v = i64(ref<real>());
        return v;
    } else if (t == typeof(i64)) {
        i64 v = ref<i64>();
        return v;
    } else if (t == typeof(null_t)) {
        return 0;
    }
    assert(false);
    return 0;
}

var::operator real() {
    type_t t = type();
    if (t == typeof(char)) {
        return str(mx::mem->hold()).real_value<real>();
    } else if (t == typeof(real)) {
        real v = ref<real>();
        return v;
    } else if (t == typeof(i64)) {
        real v = real(ref<i64>());
        return v;
    } else if (t == typeof(null_t)) {
        return 0;
    }
    assert(false);
    return 0;
}

var::operator bool() {
    type_t t = type();
    if (t == typeof(char)) {
        return mx::mem->count > 0;
    } else if (t == typeof(real)) {
        real v = ref<real>();
        return v > 0;
    } else if (t == typeof(i64)) {
        real v = real(ref<i64>());
        return v > 0;
    }
    return false;
}

liter<field<var>> var::begin() const { return map<var>(mx::mem->hold())->fields.begin(); }
liter<field<var>> var::end()   const { return map<var>(mx::mem->hold())->fields.end();   }






    str::operator std::string() { return std::string(data); }

    memory *str::symbolize() { return mem_symbol(data, typeof(char)); } 

    str str::rand(size_t sz, char from, char to) {
        str res(sz);
        for (int i = 0; i < sz; i++)
            res += rand::uniform(from, to);
        return res;
    }

    str str::escape(str chars, char esc) {
        str res { len() * 2 };
        for (int i = 0; i < len(); i++) {
            char x = data[i];
            if (chars.index_of(x) > 0)
                res += '\\';
            res += x;
        }
        return res;
    }

    /// \\ = \ ... \x = \x
    str str::parse_quoted(cstr *cursor, size_t max_len) {
        ///
        cstr first = *cursor;
        if (*first != '"')
            return "";
        ///
        bool last_slash = false;
        cstr start     = ++(*cursor);
        cstr s         = start;
        ///
        for (; *s != 0; ++s) {
            if (*s == '\\')
                last_slash = true;
            else if (*s == '"' && !last_slash)
                break;
            else
                last_slash = false;
        }
        if (*s == 0)
            return "";
        ///
        size_t len = (size_t)(s - start);
        if (max_len > 0 && len > max_len)
            return "";
        ///
        *cursor = &s[1];
        str result = "";
        for (int i = 0; i < int(len); i++)
            result += start[i];
        ///
        return result;
    }

    str::str(i64 value, u8 base, int width) : str(size_t(256)) {
        if (base < 2 || base > 36)
            fprintf(stderr, "str: base should be between 2 and 36\n");
        
        char  buffer[8 * sizeof(int) + 1];
        char* ptr = &buffer[sizeof(buffer) - 1];
        *ptr = '\0';

        do {
            int digit = value % base;
            value /= base;
            *--ptr = "0123456789abcdefghijklmnopqrstuvwxyz"[abs(digit)];
        } while (value != 0);

        if (value < 0) {
            *--ptr = '-';
        }

        int length = strlen(ptr);
        if (width) {
            int padding = width - length;
            if (padding > 0) {
                memmove(ptr + padding, ptr, length + 1);  // Include null terminator
                memset(ptr, '0', padding);
                length += padding;
            }
        }
        
        strcpy(data, ptr);
        mem->count = length;
    }

    str::str(utf16 d) : str(d.len()) { /// utf8 is a doable object but a table must be lazy loaded at best
        for (int i = 0; i < d.len(); i++)
            data[i] = d[i] <= 255 ? d[i] : '?';
    }

    /// static methods
    str str::combine(const str sa, const str sb) {
        cstr       ap = (cstr)sa.data;
        cstr       bp = (cstr)sb.data;
        ///
        if (!ap) return sb.copy();
        if (!bp) return sa.copy();
        ///
        size_t    ac = sa.count();
        size_t    bc = sb.count();
        str       sc = sa.copy(((ac + bc) + 64) * 2);
        cstr      cp = (cstr)sc.data;
        ///
        memcpy(&cp[ac], bp, bc);
        sc.mem->count = ac + bc;
        return sc;
    }

    /// skip to next non-whitespace, this could be 
    char &str::non_wspace(cstr cs) {
        cstr sc = cs;
        while (isspace(*sc))
            sc++;
        return *sc;
    }

    str::str(memory *mem) : mx(mem->type == typeof(null_t) ? alloc<char>(null, 0, 16) : mem), data(&mx::ref<char>()) { }
    str::str(null_t)      : str(alloc<char>(null, 0, 16))      { }
    str::str(char   ch)   : str(alloc<char>(null, 1, 2))       { *data = ch; }
    str::str(size_t sz)   : str(alloc<char>(null, 0, sz + 1))  { }

    str::str(std::ifstream &in) : str(alloc<char>(null, 0, ion::length(in) + 1)) {
        mem->count = mem->reserve - 1;
        in.read((char*)data, mem->count);
    }

    void str::code_to_utf8(int code, char *output) {
        if (code > 128) {
            // If codePage is greater than 128, convert it to UTF-8
            // For simplicity, this example handles only characters up to 0x7FF.
            if (code <= 0x7FF) {
                output[0] = 0xC0 | (code >> 6);
                output[1] = 0x80 | (code & 0x3F);
                output[2] = '\0';
            }  else if (code <= 0x7FF) {
                // Handle 2-byte UTF-8 encoding
                output[0] = 0xC0 | ((code >> 6) & 0x1F);
                output[1] = 0x80 | (code & 0x3F);
                output[2] = '\0';
            } else if (code <= 0xFFFF) {
                // Handle 3-byte UTF-8 encoding
                output[0] = 0xE0 | ((code >> 12) & 0x0F);
                output[1] = 0x80 | ((code >> 6) & 0x3F);
                output[2] = 0x80 | (code & 0x3F);
                output[3] = '\0';
            } else if (code <= 0x10FFFF) {
                // Handle 4-byte UTF-8 encoding
                output[0] = 0xF0 | ((code >> 18) & 0x07);
                output[1] = 0x80 | ((code >> 12) & 0x3F);
                output[2] = 0x80 | ((code >> 6) & 0x3F);
                output[3] = 0x80 | (code & 0x3F);
                output[4] = '\0';
            } else {
                // Code point is not a valid Unicode code point
                fprintf(stderr, "Invalid code point U+%X\n", code);
                output[0] = '\0';
            }
        } else {
            output[0] = code;
            output[1] = 0;
        }
    }

    str str::from_code(int code) {
        str s { size_t(32) };
        if (code < 128)
            s[0] = code;
        else {
            code_to_utf8(code, s.data);
        }
        s.mem->count = strlen(s.data);
        return s;
    }

    str str::from_integer(i64 i) { return str(memory::string(std::to_string(i))); }

    str::str(float  f32, int n) : str(memory::string(string_from_real(f32, n))) { }
    str::str(double f64, int n) : str(memory::string(string_from_real(f64, n))) { }

    /// no more symbol usage in str. thats garbage
    str::str(symbol cs, size_t len, size_t rs) : str(cstring((cstr)cs, len, rs)) { }
    str::str(cstr   cs, size_t len, size_t rs) : str(cstring(      cs, len, rs)) { }
    str::str(std::string s) : str(cstr(s.c_str()), s.length()) { }
    str::str(const str &s)  : str(s.mem->hold()) { }

    cstr str::cs() const { return cstr(data); }

    str str::expr(lambda<str(str)> fn) const {
        cstr   pa = data;
        auto   be = [&](int i) -> bool { return pa[i] == '{' && pa[i + 1] != '{'; };
        auto   en = [&](int i) -> bool { return pa[i] == '}'; };
        bool   in = be(0);
        int    fr = 0;
        size_t ln = byte_len();
        static size_t static_m = 4;
        for (;;) {
            bool exploding = false;
            size_t sz      = math::max(size_t(1024), ln * static_m);
            str    rs      = str(sz);
            for (int i = 0; i <= int(ln); i++) {
                if (i == ln || be(i) || en(i)) {
                    bool is_b = be(i);
                    bool is_e = en(i);
                    int  cr = int(i - fr);
                    if (cr > 0) {
                        if (in) {
                            str exp_in = str(&pa[fr], cr);
                            str out = fn(exp_in);
                            if ((rs.byte_len() + out.byte_len()) > sz) {
                                exploding = true;
                                break;
                            }
                            rs += out;
                        } else {
                            str out = str(&pa[fr], cr);
                            if ((rs.byte_len() + out.byte_len()) > sz) {
                                exploding = true;
                                break;
                            }
                            rs += out;
                        }
                    }
                    fr = i + 1;
                    in = be(i);
                }
            }
            if (exploding) {
                static_m *= 2;
                continue;
            }
            return rs;

        }
        return null;
    }

    /// format is a user of expr
    str str::format(array<mx> args) const {
        return expr([&](str e) -> str {
            size_t index = size_t(e.integer_value());
            if (index >= 0 && index < args.len()) {
                mx     &a    = args[index];
                memory *smem = a.to_string();
                return  smem;
            }
            return null;
        });
    }

    /// just using cs here, for how i typically use it you could cache the strings
    str str::format(symbol cs, array<mx> args) {
        return str(cs).format(args);
    }

    str::operator fs::path() const { return fs::path(std::string(data));  }
    
    void str::clear() const {
        if (mem) {
            if (mem->refs == 1)
                mem->count = *data = 0;
        }
    }

    bool  str::contains   (array<str>   a) const { return index_of_first(a, null) >= 0; }
    str   str::operator+  (symbol       s) const { return combine(*this, (cstr )s);     }
    bool  str::operator<  (const str    b) const { return strcmp(data, b.data) <  0;    }
    bool  str::operator>  (const str    b) const { return strcmp(data, b.data) >  0;    }
    bool  str::operator<  (symbol       b) const { return strcmp(data, b)      <  0;    }
    bool  str::operator>  (symbol       b) const { return strcmp(data, b)	     >  0;   }
    bool  str::operator<= (const str    b) const { return strcmp(data, b.data) <= 0;    }
    bool  str::operator>= (const str    b) const { return strcmp(data, b.data) >= 0;    }
    bool  str::operator<= (symbol       b) const { return strcmp(data, b)      <= 0;    }
    bool  str::operator>= (symbol       b) const { return strcmp(data, b)      >= 0;    }
  //bool  str::operator== (std::string  b) const { return strcmp(data, b.c_str()) == 0;  }
  //bool  str::operator!= (std::string  b) const { return strcmp(data, b.c_str()) != 0;  }
    bool  str::operator== (str          b) const { return strcmp(data, b.data)  == 0;  }
    bool  str::operator== (symbol       b) const { return strcmp(data, b)       == 0;  }
    bool  str::operator!= (symbol       b) const { return strcmp(data, b)       != 0;  }
    char& str::operator[] (size_t       i) const { return (char&)data[i];               }
    int   str::operator[] (str          b) const { return index_of(b);                  }
          str::operator             bool() const { return count() > 0;                  }
    bool  str::operator!()                 const { return !(operator bool());           }
    str   str::operator+    (const str sb) const { return combine(*this, sb);           }
    
    str &str::operator+=(str b) {
        if (mem->reserve >= (mem->count + b.mem->count) + 1) {
            cstr    ap =   data;
            cstr    bp = b.data;
            size_t  bc = b.mem->count;
            size_t  ac =   mem->count;
            memcpy(&ap[ac], bp, bc); /// when you think of data size changes, think of updating the count. [/mops-away]
            ac        += bc;
            ap[ac]     = 0;
            mem->count = ac;
        } else {
            *this = combine(*this, b);
        }
        
        return *this;
    }

    str &str::operator+= (const char b) {
        if (mem->reserve >= (mem->count + 1) + 1) {
            memcpy(&data[mem->count], &b, 1); /// when you think of data size changes, think of updating the count. [/mops-away]
            data[++mem->count] = 0;
        } else {
            *this = combine(*this, str(b));
        }
        return *this;
    }

    str &str::operator+= (symbol b) { return operator+=(str((cstr )b)); } /// not utilizing cchar_t yet.  not the full power.

    bool str::iequals(str b) const { return len() == b.len() && lcase() == b.lcase(); }
    
    int str::index_of_first(array<str> elements, int *str_index) const {
        int  less  = -1;
        int  index = -1;
        for (iter<str> it = elements.begin(), e = elements.end(); it != e; ++it) {
            ///
            str &find = (str &)it;
            ///
            //for (str &find:elements) {
            ++index;
            int i = index_of(find);
            if (i >= 0 && (less == -1 || i < less)) {
                less = i;
                if (str_index)
                    *str_index = index;
            }
            //}
        }
        if (less == -1 && str_index) *str_index = -1;
        return less;
    }

    bool str::starts_with(symbol s) const {
        size_t l0 = strlen(s);
        size_t l1 = len();
        if (l1 < l0)
            return false;
        return memcmp(s, data, l0) == 0;
    }

    size_t str::len() const {
        return count();
    }

    size_t str::utf_len() const {
        int ilen = utf::len((uint8_t*)data);
        return size_t(ilen >= 0 ? ilen : 0);
    }

    bool str::ends_with(symbol s) const {
        size_t l0 = strlen(s);
        size_t l1 = len();
        if (l1 < l0) return false;
        cstr e = &data[l1 - l0];
        return memcmp(s, e, l0) == 0;
    }

    str str::read_file(fs::path path) {
        std::ifstream in(path);
        return str(in);
    }
                                  
    str str::read_file(std::ifstream& in) {
        return str(in);
    }
                                  
    str str::recase(bool lower) const {
        str     b  = copy();
        cstr    rp = b.data;
        num     rc = b.byte_len();
        int     iA = lower ? 'A' : 'a';
        int     iZ = lower ? 'Z' : 'z';
        int   base = lower ? 'a' : 'A';
        for (num i = 0; i < rc; i++) {
            char c = rp[i];
            rp[i]  = (c >= iA && c <= iZ) ? (base + (c - iA)) : c;
        }
        return b;
    }

    str str::ucase() const { return recase(false); }
    str str::lcase() const { return recase(true);  } 

    int str::nib_value(char c) {
        return (c >= '0' && c <= '9') ? (     c - '0') :
               (c >= 'a' && c <= 'f') ? (10 + c - 'a') :
               (c >= 'A' && c <= 'F') ? (10 + c - 'A') : -1;
    }

    char str::char_from_nibs(char c1, char c2) {
        int nv0 = nib_value(c1);
        int nv1 = nib_value(c2);
        return (nv0 == -1 || nv1 == -1) ? ' ' : ((nv0 * 16) + nv1);
    }

    str str::replace(str fr, str to, bool all) const {
        str&   sc   = (str&)*this;
        cstr   sc_p = sc.data;
        cstr   fr_p = fr.data;
        cstr   to_p = to.data;
        size_t sc_c = math::max(size_t(0), sc.byte_len() - 1);
        size_t fr_c = math::max(size_t(0), fr.byte_len() - 1);
        size_t to_c = math::max(size_t(0), to.byte_len() - 1);
        size_t diff = to_c > fr_c ? math::max(size_t(0), to_c - fr_c) : 0;
        str    res  = str(sc_c + diff * (sc_c / fr_c + 1));
        cstr   rp   = res.data;
        size_t w    = 0;
        bool   once = true;

        /// iterate over string, check strncmp() at each index
        for (size_t i = 0; i < sc_c; ) {
            if ((all || once) && strncmp(&sc_p[i], fr_p, fr_c) == 0) {
                /// write the 'to' string, incrementing count to to_c
                memcpy (&rp[w], to_p, to_c);
                i   += to_c;
                w   += to_c;
                once = false;
            } else {
                /// write single char
                rp[w++] = sc_p[i++];
            }
        }

        /// end string, set count (count includes null char in our data)
        rp[w++] = 0;
        res.mem->count = w;

        /// validate allocation and write
        assert(w <= res.mem->reserve);
        return res;
    }

    /// mid = substr; also used with array so i thought it would be useful to see them as same
    str str::mid(num start, num len) const {
        int ilen = int(count());
        assert(std::abs(start) <= ilen);
        if (start < 0) start = ilen + start;
        int cp_count = len < 0 ? (ilen - start) : len;
        assert(start + cp_count <= ilen);
        return str(&data[start], cp_count);
    }

    iter<char>   str::begin() { return { data, 0 }; }
    iter<char>   str::end()   { return { data, size_t(byte_len()) }; }

    array<str> str::split(symbol s) const { return split(str(s)); }
    array<str> str::split() { /// common split, if "abc, and 123", result is "abc", "and", "123"}
        array<str> result;
        str        chars(mem->count + 1);
        ///
        cstr pc = data;
        for (;;) {
            char &c = *pc++;
            if  (!c) break;
            bool is_ws = isspace(c) || c == ',';
            if (is_ws) {
                if (chars) {
                    result += chars.copy();
                    chars = str(mem->count + 1);
                }
            } else
                chars += c;
        }
        ///
        if (chars || !result)
            result += chars;
        ///
        return result;
    }

    int str::index_of(char b, int from) const {
        if (!mem || mem->count == 0) return -1;
        
        cstr   ap =   data;
        int    ac = int(mem->count);

        if (from < 0) {
            for (int index = ac - 1 + from + 1; index >= 0; index--) {
                if (ap[index] == b)
                    return index;
            }
        } else {
            for (int index = from; index <= ac - 1; index++)
                if (ap[index] == b)
                    return index;
        }
        return -1;
    }

    int str::index_of(str b, int from) const {
        if (!b.mem || b.mem->count == 0) return  0;
        if (!  mem ||   mem->count == 0) return -1;

        cstr   ap =   data;
        cstr   bp = b.data;
        int    ac = int(  mem->count);
        int    bc = int(b.mem->count);
        
        /// dont even try if b is larger
        if (bc > ac) return -1;

        /// search for b, reverse or forward dir
        if (from < 0) {
            for (int index = ac - bc + from + 1; index >= 0; index--) {
                if (strncmp(&ap[index], bp, bc) == 0)
                    return index;
            }
        } else {
            for (int index = from; index <= ac - bc; index++)
                if (strncmp(&ap[index], bp, bc) == 0)
                    return index;
        }
        
        /// we aint found ****. [/combs]
        return -1;
    }

    int str::index_of(MatchType ct, symbol mp) const {
        int  index = 0;
        
        using Fn = func<bool(const char &)>;
        static umap<MatchType, Fn> match_table {
            { Alpha,     Fn([&](const char &c) -> bool { return  isalpha (c); }) },
            { Numeric,   Fn([&](const char &c) -> bool { return  isdigit (c); }) },
            { WS,        Fn([&](const char &c) -> bool { return  isspace (c); }) }, // lambda must used copy-constructed syntax
            { Printable, Fn([&](const char &c) -> bool { return !isspace (c); }) },
            { String,    Fn([&](const char &c) -> bool { return  strcmp  (&c, mp) == 0; }) },
            { CIString,  Fn([&](const char &c) -> bool { return  strcmp  (&c, mp) == 0; }) }
        };
        
        /// msvc thinks its ambiguous, so i am removing this iterator from str atm.
        cstr pc = data;
        for (;;) {
            char &c = pc[index];
            if  (!c)
                break;
            if (match_table[ct](c))
                return index;
            index++;
        }
        return -1;
    }

    i64 str::integer_value() const {
        return ion::integer_value(mx::mem);
    }

    bool str::has_prefix(str i) const {
        char    *s = data;
        size_t isz = i.byte_len();
        size_t  sz =   byte_len();
        return  sz >= isz ? strncmp(s, i.data, isz) == 0 : false;
    }

    bool str::numeric() const {
        return byte_len() && (data[0] == '-' || isdigit(*data));
    }

    bool str::matches(str input) const {
        lambda<bool(cstr, cstr)> fn;
        str&  a = (str &)*this;
        str&  b = input;
             fn = [&](cstr s, cstr p) -> bool {
                return (p &&  *p == '*') ? ((*s && fn(&s[1], &p[0])) || fn(&s[0], &p[1])) :
                      (!p || (*p == *s && fn(&s[1],   &p[1])));
        };
        for (cstr ap = a.data, bp = b.data; *ap != 0; ap++)
            if (fn(ap, bp))
                return true;
        return false;
    }

    str str::trim() const {
        cstr  s = data;
        int   c = int(byte_len());
        int   h = 0;
        int   t = 0;

        while (isspace(s[h]))
            h++;

        while (isspace(s[c - 1 - t]) && (c - t) > h)
            t++;

        return str(&s[h], c - (h + t));
    }

    size_t str::reserve() const { return mx::mem->reserve; }

	utf16::utf16(size_t sz)   : utf16(mx::alloc<u16>(null, sz, sz)) { }

	utf16::utf16(char *input) : utf16(strlen(input)) {
		char *i = input;
		num cursor = 0;
		while (*i) {
			data[cursor++] = *i;
			i++;
		}
	}

    utf16::utf16(symbol s) : utf16((char*)s) { }

    utf16::utf16(wstr input, size_t len) : utf16(len) {
        memcpy(data, input, len * sizeof(char_t));
    }

    utf16 utf16::operator+(symbol s) {
        size_t ln = strlen(s);
        utf16  r { mem->count + ln };
        memcpy( r.data, data, mem->count * 2 );
        memcpy(&r.data[mem->count], s, ln );
        return r;
    }


	utf16::char_t &utf16::operator[](num index) const {
		if (index < 0)
			return data[mem->count + index];
		return data[index];
	}

    int utf16::index_of(wchar_t find) const {
        int result = -1;
        for (int i = 0, ln = len(); i < ln; i++) {
            if (data[i] == find) {
                result = i;
                break;
            }
        }
        return result;
    }

    utf16 utf16::escape(utf16 chars, char esc) const { // probably not the greatest
        size_t n = 0;
        for (int i = 0; i < len(); i++) {
            char x = data[i];
            if (chars.index_of(x) > 0)
                n += 2;
            else
                n += 1;
        }
        if (!n)
            return utf16();
        utf16 res { n };
        for (int i = 0; i < len(); i++) {
            char_t x = data[i];
            if (chars.index_of(char(x)) > 0)
                res[i] = wchar_t(esc);
            res[i] = x;
        }
        return res;
    }

	utf16 utf16::mid(num start, num length) const {
		char_t *s  = start  >= 0 ? data + start  : &data[mem->count + start ];
		char_t *e  = length >= 0 ? data + length : &data[mem->count + length];
		if (e > s)
			return utf16();
		size_t sz = std::distance(s, e);
		utf16  res { sz };
		///
		memcpy(res.data, s, sz * sizeof(char_t));
		return res;
	}

	size_t utf16::len() const {
		return mem ? mem->count : 0;
	}

	utf16 utf16::join(array<utf16> &src, utf16 j) {
		size_t sz = 0;
		num cursor = 0;
		for (utf16 &s: src) {
			sz += s.mem->count;
			if (cursor < src.len() - 1)
				sz += j.len();
			cursor++;
		}

		utf16 res { sz };
		num   pos = 0;
		cursor = 0;
		for (utf16 &s: src) {
			memcpy(&res.data[pos], s.data, s.len());
			pos += s.len();
			if (cursor < src.len() - 1) {
				memcpy(&res.data[pos], s.data, j.len());
				pos += j.len();
			}
			cursor++;
		}
		return res;
	}






}