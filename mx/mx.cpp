#include <mx/mx.hpp>

namespace ion {

external_implement(fs::path);
external_implement(std::nullptr_t);
external_implement(char);
external_implement(i8);
external_implement(i16);
external_implement(i32);
external_implement(i64);
external_implement(u8);
external_implement(u16);
external_implement(u32);
external_implement(u64);
external_implement(r32);
external_implement(r64);

logger console;

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

///
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
    /// prefix builds have their own resources gathered throughout.
    /// depend on a module and we copy (res/*) -> binary_dir/
    mx e = ext().symbolize();
    static path  data = "data/mime-type.json";
    static map<mx> js =  data.read<var>();
    return js->count(e) ? ((memory*)js[e])->grab() : ((memory*)js["default"])->grab();
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

memory *grab(memory *mem) {
    if (mem) mem->grab();
    return mem;
}

i64 millis() {
    return i64(
               std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                     std::chrono::system_clock::now().time_since_epoch()).count()
               );
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
    type_t          ty        = (type->traits & traits::array) != 0 ? type->schema->bind->data : type;
    size_t          sz        = ty->base_sz; /// size of actual data consumed -- we dont use functions that are emitted from array<T> we use T in vector form (which they are already)
    u8             *dst       = (u8*)calloc(alloc_reserve, sz);
    u8             *src       = (u8*)origin;
    size_t          mn        = math::min(alloc_reserve, count);
    const bool      prim      = (ty->traits & traits::primitive) != 0;
    alloc_schema   *mx        = ty->schema; // issues with array<str>, resolving element type
    size_t          stride    = mx->total_bytes;

    if (prim) {
        memcpy(dst, src, sz * mn);
    } else {
        for (size_t i = 0; i < mx->bind_count; i++) {
            context_bind &c  = mx->composition[i];
            for (size_t ii = 0; ii < mn; ii++) {
                c.data->functions->copy    (&dst[c.offset + ii * stride], &src[c.offset + ii * stride]); /// copy prior data
                c.data->functions->destruct(&src[c.offset + ii * stride]); /// destruct prior data
            }
        }
    }
    /// this controls the 'count' which is in-effect the constructed count.  if we are not constructing, no updating count
    if (fill_default) {
        count = alloc_reserve;
        if (!prim) {
            for (size_t i = 0; i < mx->bind_count; i++) {
                context_bind &c  = mx->composition[i];
                for (size_t ii = mn; ii < alloc_reserve; ii++)
                    c.data->functions->construct(&dst[c.offset + ii * stride]);
            }
        }
    }

    /// private destructors break meta, cant vectorize them
    free(origin); // char type destruct seems to be a free call on memory, not the no-op i thought
    
    origin  = raw_t(dst);
    reserve = alloc_reserve;
    return origin;
}

/// mx-objects are clearable which brings their count to 0 after destruction
void memory::clear() {
    alloc_schema *mx  = type->schema;
    u8           *dst = (u8*)origin;
    if (mx) {
        for (size_t i = 0; i < mx->bind_count; i++) { /// count should be called bind_count or something; its too ambiguous with memory
            context_bind &c  = mx->composition[i];
            for (size_t ii = 0; ii < count; ii++)
                c.data->functions->destruct(&dst[c.offset + ii * mx->total_bytes]);
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
            m = memory::alloc(typeof(char), len, len + 1, raw_t(sym));
            m->attrs = attr::constant;
            ctype->symbols->ids[id] = m; /// was not hashing by id, it was the djb2 again (incorrect)
            //ctype->symbol_djb2[h_sym] = m; 'redundant due to the setting of the memory*& (which [] operator always inserts item)
            ctype->symbols->list->push(m);
        }
        return m->grab();
    } else {
        size_t     ln = (len == memory::autolen) ? strlen(sym) : len;
        size_t     al = rsv ? rsv : (strlen(sym) + 1);
        memory*   mem = memory::alloc(typeof(char), ln, al, raw_t(sym));
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

memory *memory::raw_alloc(type_t type, size_t sz, size_t count, size_t res) {
    size_t elements = math::max(count, res);
    memory*     mem = (memory*)calloc(1, sizeof(memory) + elements * sz); /// there was a 16 multiplier prior.  todo: add address sanitizer support with appropriate clang stuff
    mem->count      = count;
    mem->reserve    = res;
    mem->refs       = 1;
    mem->type       = type;
    mem->origin     = (void*)&mem[1];
    return mem;
}
/*
memory *memory::wrap(raw_t m, type_t ty) {
    memory*     mem = (memory*)calloc(1, sizeof(memory)); /// there was a 16 multiplier prior.  todo: add address sanitizer support with appropriate clang stuff
    mem->count      = 1;
    mem->reserve    = 1;
    mem->refs       = 1;
    mem->type       = ty;
    mem->origin     = m;
    return mem;
}
*/

/// starting at 1, it should remain active.  shall not be freed as a result
void memory::drop() {
    if (--refs <= 0 && !constant) { /// <= because ptr does a defer on the actual construction of the container class
        origin = null;
        // delete attachment lambda after calling it
        if (atts) {
            for (attachment &att: *atts)
                att.release();
            delete atts;
            atts = null;
        }
        if (managed)
            free(origin);
        if (shape) {
            delete shape;
            shape = null;
        }
        //free(this);
    }
}

/// now we start allocating the total_size (or type->base_sz if not an mx/schema-based class)
memory *memory::alloc(type_t type, size_t count, size_t reserve, raw_t v_src) {
    if (strstr(type->name, "lambda")) {
        int test = 0;
        test++;
    }
    memory *result = null;
    size_t type_sz = type->size();

    if (type->singleton)
        return type->singleton->grab();
    
    memory *mem = memory::raw_alloc(type, type_sz, count, reserve);

    if (type->traits & traits::singleton)
        type->singleton = mem;

    /// from here we need to use the contained type within array, and all copies are within the schema of the element
    type_t ty = (type->traits & traits::array) != 0 ? type->schema->bind->data : type;

    const bool prim = (ty->traits & traits::primitive);

    /// if allocating a schema-based object (mx being first user of this)
    if (count > 0) {
        size_t stride = ty->schema->total_bytes;
        if (v_src) {
            /// if schema-copy-construct (call cpctr for each data type in composition)
            for (size_t i = 0; i < ty->schema->bind_count; i++) {
                context_bind &bind = ty->schema->composition[i];
                u8 *dst = &((u8*)mem->origin)[bind.offset];
                u8 *src = &((u8*)      v_src)[bind.offset];
                if (bind.data)
                    for (size_t ii = 0; ii < count; ii++) {
                        if (bind.data->functions) /// enums dont have this set; in order for that to be the case we cannot embed them and must do 2 part decl/impl
                            bind.data->functions->copy(&dst[ii * stride], &src[ii * stride]);
                        else
                            memcpy(&dst[ii * stride], &src[ii * stride], bind.data_sz); /// needs an assert check to be sure its primitive
                    }
            }
        } else {
            /// ctr: call construct across the composition
            for (size_t i = 0; i < ty->schema->bind_count; i++) {
                context_bind &bind = ty->schema->composition[i];
                u8 *dst  = &((u8*)mem->origin)[bind.offset];
                if (bind.data && bind.data->functions)
                    for (size_t ii = 0; ii < count; ii++) {
                        bind.data->functions->construct(&dst[ii * stride]);
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

memory *memory::grab() {
    refs++;
    return this;
}

size &size::operator=(const size b) {
    memcpy(values, b.values, sizeof(values));
    count = b.count;
    return *this;
}


void chdir(std::string c) {
#if defined(_WIN32)
    // replace w global for both cases; its not worth it
    //SetCurrentDirectory(c.c_str());
#else
    ::chdir(c.c_str());
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
}