#pragma once

struct map;
struct ex;

template <typename T>
using fun = std::function<T>;

/// mx-mock-type
struct ident {
    memory *mem;

    static inline hashmap *types; /// cstr -> type_t
    static inline idata *char_t; /// cannot use hashmap/mx::hash without a registered char type
    static inline idata *i64_t;
    static inline idata *u64_t;
    

    template <typename T>
    static idata* for_type();

    template <typename T>
    static idata* for_type2();

    ident(memory* mem) : mem(mem) { }
    ident(bool init);
    static void init();
};

template <> idata *ident::for_type2<null_t>();
template <> idata *ident::for_type2<char>();
template <> idata *ident::for_type2<i64>();
template <> idata *ident::for_type2<u64>();

memory *mem_symbol(symbol cs, type_t ty = typeof(char), i64 id = 0);

struct context_bind {
    type_t        ctx;
    type_t        data;
    size_t        base_sz;
    size_t        voffset;
};

struct alloc_schema {
    size_t        bind_count;  /// bind-count
    size_t        total_bytes; /// total-allocation-size
    context_bind *composition; /// vector-of-pairs (context and its data struct; better to store the data along with it!)
    context_bind *bind; // this is a pointer to the top bind
};

/// these should be vectorized but its not worth it for now
template <typename T> using   SetMemoryFn =          fun<void(T*, memory*)>;  /// a (inst), b (memory)
template <typename T> using         AddFn =          fun<void(const T&, const T&, T&)>;
template <typename T> using         MulFn =          fun<void(const T&, const T&, T&)>;
template <typename T> using         DivFn =          fun<void(const T&, const T&, T&)>;
template <typename T> using        DivSFn =          fun<void(const T&, const float, T&)>;
template <typename T> using        MulSFn =          fun<void(const T&, const float, T&)>;
template <typename T> using      EqualsFn =          fun<bool(T*, T*, size_t)>;
template <typename T> using         MixFn =          fun<T*  (T*, T*, double)>;
template <typename T> using     CompareFn =           fun<int(T*, T*, T*)>;  /// a, b
template <typename T> using     BooleanFn =          fun<bool(T*, T*)>;      /// src
template <typename T> using    ToStringFn =       fun<memory*(T*)>;          /// src (user implemented on external)
template <typename T> using  FromStringFn =            fun<T*(T*, cstr)>;    /// placeholder, src-cstring
template <typename T> using        CopyFn =          fun<void(T*, T*, T*)>;  /// dst, src (realloc case)
template <typename T> using    DestructFn =          fun<void(T*, T*)>;      /// src
template <typename T> using   ConstructFn =          fun<void(T*, T*)>;      /// src
template <typename T> using        InitFn =          fun<void(T*)>;          /// src
template <typename T> using         NewFn =            fun<T*()>;
template <typename T> using      VAllocFn =            fun<T*(T*, T*, size_t)>; /// simply 'free' this one.  made for basic structs
template <typename T> using         DelFn =          fun<void(T*, T*)>;      /// placeholder, instance
template <typename T> using      AssignFn =          fun<void(T*, T*, T*)>;  /// dst, src
template <typename T> using        HashFn =        size_t(*)(T*, size_t);  /// src
template <typename T> using     ProcessFn =          fun<void(T*, memory*)>; /// dst, src

using PushFn = void(*)(void*, void*); /// Array<T>* dst, T* src

using GenericLambda = fun<mx(void*, const array &)>;

///
struct idata {
    memory          *mem;     /// origin memory
    struct idata    *src;     ///
    cstr             name;
    size_t           base_sz; /// a types base sz is without regards to pointer state
    size_t           traits;
    bool             pointer; /// allocate enough space for a pointer to be stored
    doubly          *meta;    /// properties
    prop_map        *meta_map; // prop_map
    alloc_schema    *schema;  /// primitives dont need this one -- used by array and map for type aware contents, also any mx-derrived object containing data will populate this polymorphically
    
    struct f_table {
        //bool mix;     /// mix op allowed (these are virtual, so performing that takes just mx expression on the addresses)
        //bool compare; /// compare op allowed

        std::function<void(void*)>         dtr;
        std::function<void(void*)>         ctr;
        std::function<void(void*,memory*)> ctr_mem;  /// for mx objects (assign from memory)
        std::function<void(void*,memory*)> ctr_type; /// for mx objects (copy convert)
        std::function<void(void*,const str &)> ctr_str; /// for mx objects (copy convert)
        std::function<void(void*,void*)>   ctr_cp;
        std::function<void(void*,void*,void*,double)> mix;
        std::function<void(void*,void*)>   compare;
        std::function<bool(void*)>         boolean;
        std::function<memory*(void*)>      str;
    } f;

    symbol_data     *symbols;
    memory          *singleton;
    idata           *ref; /// if you are given a primitive enum, you can find the schema and symbols on ref (see: to_string)
    idata           *parent; 
    bool             secondary_init; /// used by enums but flag can be used elsewhere.  could use 'flags' too
    GenericLambda   *generic_lambda;

    void   *alloc();
    void   *ctr();
    void    dtr(void* alloc);
    void   *ctr_mem (memory *mem);
    void   *ctr_str (const str &v);
    void   *ctr_type(memory *mem);
    void   *ctr_cp  (void* b);
    size_t  size();
    memory *lookup(symbol sym);
    memory *lookup(i64 id);
    idata  *meta_lookup() {
        return meta ? this : (schema ? schema->bind->data : null);
    }

    template <typename T>
    bool classof() {
        idata *tcompare = typeof(T);
        idata *tself    = this;
        while (tself) {
            if (tself == tcompare)
                return true;
            tself = tself->parent;
        }
        return false;
    }
};

/// now that we have allowed for any, make entry for meta description
struct prop {
    memory *key;
    size_t  member_addr;
    size_t  offset; /// this is set by once-per-type meta fetcher
    type_t  type;
    type_t  parent_type;
    raw_t   init_value; /// value obtained after constructed on the parent type
    str    *s_key;
    
    prop() : key(null), member_addr(0), offset(0), type(null) { }

    template <typename M>
    prop(symbol name, M &member) : key(mem_symbol(name)), member_addr((size_t)&member), type(typeof(M)) { }

    template <typename M>
    M &member_ref(void *m) { return *(M *)handle_t(&cstr(m)[offset]); }

    void *member_pointer(void *m) { return (void *)handle_t(&cstr(m)[offset]); }

    symbol name() const {
        return symbol(mem_origin(key));
    }
};


struct memory:Memory<none> {
    enum attr { constant = 1 };

    template <typename T>
    operator Memory<T> &() const {
        return *(const Memory<T>*)this;
    }

    template <typename T>
    T &ref() { return *get<T>(0); }

    void    drop() { ion::drop(this); }
    memory *hold() { return ion::hold(this); }

    static memory *raw_alloc(type_t type, size_t sz, size_t count, size_t res);

    static memory *    alloc(type_t type, size_t count = 1, size_t reserve = 1, raw_t src = null);
           void   *  realloc(size_t res,  bool fill_default);

    /// destruct data and set count to 0
    void clear();

    attachment *attach(symbol id, void *data, func<void()> release);
    attachment *find_attachment(symbol id);

    static inline const size_t autolen = UINT64_MAX;

    static inline memory *wrap(type_t type, void *origin, size_t count = 1, bool managed = true) {
        memory*     mem = raw_alloc(type, sizeof(idata), count, count);
        mem->origin     = origin;
        mem->attrs     |= traits::managed;
        return mem;
    }

    static inline memory *window(type_t type, void *origin, size_t count = 1) {
        return wrap(type, origin, count, false);
    }

    static memory *stringify(cstr cs, size_t len = autolen, size_t rsv = 0, bool constant = false, type_t ctype = typeof(char), i64 id = 0);
    static memory *string   (std::string s);
    static memory *cstring  (cstr        s);
    static memory *symbol   (ion::symbol s, type_t ty = typeof(char), i64 id = 0);
    
    ion::symbol symbol() {
        return ion::symbol(origin);
    }

    operator ion::symbol*() {
        assert(attrs & constant);
        return (ion::symbol *)origin;
    }

    memory *copy(size_t reserve = 0);
    
    /// now it has a schema with types to each data structure, their offset in allocation, and the total allocation bytes
    /// also supports the primative store, non-mx
    template <typename T>
    T *get(size_t index) const;

    template <typename T>
    T *set(size_t index, const T& data);

    type_t data_type() const;
    raw_t typed_data(type_t data_type, size_t index) const;
};

using properties = doubly;

template <typename T> T* mdata(memory *mem, size_t index) { return mem ? mem->get<T>(index) : null; }

mx mix_wrap(mx* obj, mx* other, float factor);
int compare_wrap(mx* a, mx* b);

struct mx:MX {
    using parent_class  = none;
    using context_class = none;
    using intern        = none;
    static inline type_t intern_t = null;

    static type_t register_class();
    static type_t register_data();

    //mx  mix    (const mx& b, float f) const;
    int compare(const mx& b) const;
    
    mx  to_string() const;
    u64 hash()      const;

    template <typename T>
    static mx move(const T &v);

    static mx   from_string(cstr cs, type_t type = null);

    template <typename T>
    T &ref() const {
        return *get<T>(0);
    }
    
    template <typename T>
    struct iterable {
        T*     data;
        size_t size;

        iter<T> begin() const { return iter<T>{data, 0};    }
        iter<T>   end() const { return iter<T>{data, size}; }
    };

    template <typename T>
    iterable<T> elements() const {
        return iterable<T> { get<T>(0), mem->count };
    }

    //void *realloc(size_t reserve, bool fill_default);

    raw_t find_prop_value(str name, prop *&rprop);

    mx raw_copy(size_t reserve) {
        type_t type = mem->type;
        void *m = calloc(type->base_sz, reserve ? reserve : mem->count);
        memcpy(m, mem->origin, type->base_sz * mem->count);
        return memory::wrap(type, m, mem->count);
    }

    sz_t total_size() {
        return count() * mem->type->base_sz;
    }
    
    sz_t reserve() { return math::max(mem->reserve, mem->count); }

    template <typename T>
    static inline memory *wrap(void *m, size_t count = 1, T *placeholder = (T*)null) {
        memory*     mem = (memory*)calloc64(1, sizeof(memory));
        mem->count      = count;
        mem->reserve    = 0;
        mem->refs       = 1;
        mem->type       = typeof(T);
        mem->origin     = m;
        return mem;
    }

    template <typename T>
    static inline memory *window(T *m, size_t count = 1) {
        return wrap((void*)m, count, (T*)null);
    }

    /// T is context, not always the data type.  sometimes it is for simple cases, but not in context -> data 
    template <typename T>
    static inline memory *alloc(void *cp, size_t count = 1, size_t reserve = 1, T *ph = null) {
        /// deal with string from the macro-based constructor
        if constexpr (identical<T, char>()) {
            reserve = math::max(reserve, count + 1);
        }
        return memory::alloc(typeof(T), count, reserve, raw_t(cp));
    }

    template <typename T>
    static inline memory *alloc(T *cp = null) {
        return memory::alloc(typeof(T), 1, 1, raw_t(cp));
    }

    mx(u8 *bytes, size_t len) : mx(memory::alloc(typeof(u8), len, len, (void*)bytes)) { }

    operator std::string() {
        return std::string((symbol)mem->origin);
    }
    
    mx(std::string s) : mx(memory:: string(s)) { }

    /// must move lambda code away from lambda table, and into constructor code gen
    template <typename T>
    memory *copy(T *src, size_t count, size_t reserve) {
        memory*     mem = (memory*)calloc64(1, sizeof(memory));
        mem->count      = count;
        mem->reserve    = math::max(count, reserve);
        mem->refs       = 1;
        mem->type       = typeof(T);
        mem->attrs     |= traits::managed;
        mem->origin     = (T*)calloc64(mem->reserve, sizeof(T));
        ///
        if constexpr (is_primitive<T>()) {
            memcpy(mem->origin, src, sizeof(T) * count);
        } else {
            for (size_t i = 0; i < count; i++)
                new (&((T*)mem->origin)[i]) T(&src[i]);
        }
        return mem;
    }

    template <typename T>
    T use() {
        return T(ion::hold(mem));
    }

    size  *shape() const { return mem->shape;  }

    attachment *find_attachment(symbol id) {
        return mem->find_attachment(id);
    }

    attachment *attach(symbol id, void *data, func<void()> release) {
        return mem->attach(id, data, release);
    }

    bool is_const() const { return mem->attrs & memory::constant; }

    prop *lookup(cstr cs) const { return lookup(mx(mem_symbol(cs))); }

    prop *lookup(mx key) const {
        if (key.is_const() && mem->type->schema) {
            doubly *meta = (doubly*)mem->data_type()->meta;
            if (meta)
                for (prop &p: meta->elements<prop>()) {
                    if (mx(key) == p.key) 
                        return &p;
                }
        }
        return null;
    }

    template <typename T>
    T *get(size_t index = 0) const {
        return mem->get<T>(index);
    }

    void set(memory *m) {
        if (m != mem) {
            ion::drop(mem);
            mem = ion::hold(m);
        }
    }

    template <typename T>
    T &set(const T &v) {
        memory *nm = alloc(v);
        set(nm);
        return *get<T>(0);
    }

    template <typename T>
    T *set(size_t index, const T &value) const {
        return mem->set<T>(index, value);
    }

    template <typename T>
    T *origin() const { return (T*)mem->origin; }

    void set_size(sz_t sz) {
        assert(mem->reserve >= sz);
        mem->count = sz;
    }

    mx get_meta(cstr cs) const { return get_meta(mx(mem_symbol(cs))); }

    mx get_meta(const mx &key) const {
        ion::prop *pr    = lookup(key);                    assert(pr);
        void      *src   = pr->member_pointer(mem->origin); assert(src);
        bool       is_mx = (pr->type->traits & traits::mx_obj) || (pr->type->traits & traits::mx);
        return    (is_mx) ? ion::hold(((mx*)src)->mem) : memory::window(pr->type, src);
    }

    void set_meta(cstr cs, mx value) { set_meta(mx(mem_symbol(cs)), value); }
    void set_meta(const mx &key,  const mx &value) {
        prop  *p = lookup(key);
        assert(p);
        type_t t = p->type;

        assert((t == value.type() || (t->traits & traits::mx_enum)) || 
               (t->schema && t->schema->bind->data == value.type()));
        
        void *dst = p->member_pointer(mem->origin);
        void *src = value.origin<void>();
        assert(dst);
        if (t->f.dtr)
            t->f.dtr(dst);
        if ((t->traits & traits::mx) || (t->traits & traits::mx_obj))
            t->f.ctr_mem(dst, ion::hold(value.mem)); /// issue is we are performing Array<int>& cp with *int
        else
            t->f.ctr_cp (dst, src); /// issue is we are performing Array<int>& cp with *int
    }

   ~mx() { ion::drop(mem); }
    
    mx(null_t = null): mx(alloc<null_t>()) { }
    mx(memory *mem)  : MX(mem) { }

    mx(symbol ccs, type_t type = typeof(char)) : mx(mem_symbol(ccs, type)) { }
    mx(cstr   cs,  type_t type = typeof(char)) : mx(memory::stringify(cs, memory::autolen, 0, false, type)) { }
    
    mx(const mx &b) : mx(ion::hold(b.mem)) { }

    mx(bool   v) : MX(v) { }
    mx(u8     v) : MX(v) { }
    mx(i8     v) : MX(v) { }
    mx(u16    v) : MX(v) { }
    mx(i16    v) : MX(v) { }
    mx(u32    v) : MX(v) { }
    mx(i32    v) : MX(v) { }
    mx(u64    v) : MX(v) { }
    mx(i64    v) : MX(v) { }
    mx(r32    v) : MX(v) { }
    mx(r64    v) : MX(v) { }

    /// construct element of type with optional copy
    mx(type_t type, void *src = null) : mx(memory::alloc(type, 1, 0, src)) { }

    template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    mx(E v) : mx(alloc(&v)) { }

    size_t   byte_len() const {
        type_t type = mem->type;
        size_t t    = (type->schema && type->schema->total_bytes) ? type->schema->total_bytes : type->base_sz;
        return count() * t;
    }

    memory    *copy(size_t res = 0) const { return mem->copy((res == 0 && mem->type == typeof(char)) ? (mem->count + 1) : res); }
    memory *quantum()               const { return (mem->refs == 1) ? mem : mem->copy(); }

    bool is_string() const {
        return mem->data_type() == typeof(char);
    }

    size_t count() const { return mem ? mem->count : 0;    }
    type_t  type() const { return mem ? mem->type  : null; }

    bool operator==(const mx &b) const {
        if (mem == b.mem)
            return true;
        return compare(b) == 0;
    }

    bool operator!=(const mx &b)    const { return !operator==(b); }
    
    bool operator==(symbol b) const {
        if (mem && mem->type == typeof(char)) {
            if (mem->attrs & memory::attr::constant) {
                return mem == mem_symbol(b);
            } else {
                return strcmp(b, mem->get<char>(0)) == 0;
            }
        }
        return false;
    }
    bool operator!=(symbol b) const { return !operator==(b); }

    mx &operator=(const mx &b) {
        mx &a = *this;
        if (a.mem != b.mem) {
            ion::drop(a.mem);
            ion::hold(b.mem);
            a.mem = b.mem;
        }
        return *this;
    }

    /// without this as explicit, there are issues with enum mx types casting to their etype.
    explicit operator bool() const {
        if (mem) { /// if mem, origin is always set
            type_t ty = mem->type;

            /// satisfies use case
            /// uint8_t and int8_t types are merging at the moment
            /// the above is true for a non null first char or the count > 2 (h264 packets are Never Never 1 byte)
            /// however it may step on toes in different string management regimes
            if (is_string())
                return mem->origin && (*(char*)mem->origin != 0 || mem->count > 1);

            /// typeof null_t is false, even if it has 1 of them [?]
            if (ty == typeof(null_t))
                return false;
            
            if (ty == typeof(bool))
                return *((bool*)mem->origin); /// bool lies about its size in a couple of ways

            /// primitive array boolean
            if (ty->traits & traits::primitive) {
                /// if mem->count == 1 and reserve is 0, we can assume these are singular
                if (mem->count == 1 && mem->reserve == 0) {
                    for (int i = 0; i < ty->base_sz; i++) {
                        /// ambiguous between single instance vs array
                        /// todo: fix this case; an array of int with a value of 0 inside of it should still be truthy
                        /// to fix that, we would need to make array hold onto a custom data container with its own bool op
                        u8 b = ((u8*)mem->origin)[i];
                        if (b > 0)
                            return true;
                    }
                    return false;
                }
                /// used for array data; an array of count > 0 is by default, truthy
                return mem->count > 0;
            }

            /// use boolean operator on the data by calling the generated function table
            if (ty->schema && ty->schema->bind->data->f.boolean) {
                return mem->origin && ty->schema->bind->data->f.boolean(mem->origin);
            }
            
            return mem->count > 0;
        } else
            return false;
    }
    
    bool operator!() const { return !(operator bool()); }

    ///
    explicit operator   i8()      { return *get<i8>();  }
    explicit operator   u8()      { return *get<u8>();  }
    explicit operator  i16()      { return *get<i16>(); }
    explicit operator  u16()      { return *get<u16>(); }
    explicit operator  i32()      { return *get<i32>(); }
    explicit operator  u32()      { return *get<u32>(); }
    explicit operator  i64()      { return *get<i64>(); }
    explicit operator  u64()      { return *get<u64>(); }
    explicit operator  r32()      { return *get<r32>(); }
    explicit operator  r64()      { return *get<r64>(); }
    
    explicit operator  symbol()   { return (ion::symbol)mem->origin; }

    size_t alloc_size() const {
        size_t usz = size_t(mem->count);
        size_t ual = size_t(mem->reserve);
        return ual == 0 ? usz : ual;
    }

    template <typename V>
    static mx pointer(V* ptr, int sz = 1) {
        return memory::window(typeof(V), ptr, sz);
    }
};

template <typename T>
struct lambda;

template <typename R, typename... Args>
struct lambda<R(Args...)>:mx {
    using fdata = std::function<R(Args...)>;
    
    /// just so we can set it in construction
    struct container {
        fdata *lfn;
        ~container() {
            delete lfn;
        }
    };
    
    mx_object(lambda, mx, container);
    
    template <typename F>
    lambda(F&& fn);

    template <typename CL, typename F>
    lambda(CL* cl, F fn);

    R operator()(Args... args) const {
        return (*data->lfn)(std::forward<Args>(args)...);
    }
    
    operator bool() const {
        return data && data->lfn && *data->lfn;
    }
};

template <typename T> struct is_lambda<lambda<T>> : true_type  { };

template <typename R, typename... Args>
template <typename F>
lambda<R(Args...)>::lambda(F&& fn) : mx() {
    if constexpr (ion::inherits<mx, F>() || is_lambda<std::remove_reference_t<F>>::value) {
        mx::mem = ion::hold(fn);
        data    = (container*)mem->origin;
    } else {
        if constexpr (std::is_invocable_r_v<R, F, Args...>) {
            mx::mem  = mx::alloc<lambda>();
            data     = (container*)mem->origin;
            data->lfn = new fdata(std::forward<F>(fn));
        } else {
            static_assert("F type is not a functor");
        }
    }
}


template <typename R, typename... Args>
template <typename CL, typename F>
lambda<R(Args...)>::lambda(CL* cl, F fn) {
    mx::mem  = mx::alloc<lambda>();
    data     = (container*)mem->origin;

    using traits     = lambda_traits<lambda>;
    using args_t     = typename traits::arg_types;
    data->lfn = new fdata([=](Args... args) { return (cl->*fn)(std::forward<Args>(args)...); });
}

mx call(mx lambda, const array &args);

struct array:mx {
    array(memory*   mem) : mx(mem) { }
    array(const mx   &o) : array(ion::hold(o.mem)) { }
    array()              : array(mx::alloc<array>(null, 0, 0)) { } /// this must be lazy-alloc'd; generically, if we have a void type, then allocation must be lazy
    
    static type_t register_class();
    static type_t register_data();
    /// there is no default data type for generic array, as we dont want to swap out default allocations for the users data
    /// as such we need to handle this

    template <typename T>
    array(std::initializer_list<T> args) : array() { //  size_t(args.size()) -- doesnt work with the pre-alloc; find out why.
        for (auto &v:args) {
            T &element = (T &)v;
            push(element);
        }
    }
    array(type_t type, size al, size sz = size(0)) : 
            array(memory::alloc(type, sz, al)) {
        if (al.count > 1)
            mem->shape = new size(al); /// allocate a shape if there are more than 1 dims
    }

    /// constructor for allocating with a space to fill (and construct; this allocation does not run the constructor (if non-primitive) until append)
    /// indexing outside of shape space does cause error
    array(type_t type, size_t  reserve) : array(memory::alloc(type, 0, size_t(reserve))) { }
    array(type_t type,    u32  reserve) : array(memory::alloc(type, 0, size_t(reserve))) { }
    array(type_t type,    i32  reserve) : array(memory::alloc(type, 0, size_t(reserve))) { }
    array(type_t type,    i64  reserve) : array(memory::alloc(type, 0, size_t(reserve))) { }

    type_t element_type() const { return mem->data_type(); }

    template <typename T>
    static void pushv(array *a, memory *m_item) {
        if constexpr (is_convertible<memory*, T>()) {
            T item(ion::hold(m_item));
            a->push(item);
        } else {
            T &item = *(T*)m_item->origin;
            a->push(item);
        }
    }

    template <typename T>
    static array read_file(symbol filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("failed to open file!");

        size_t byte_sz = (size_t) file.tellg();
        array result(typeof(T), byte_sz / sizeof(T), byte_sz / sizeof(T));
        file.seekg(0);
        file.read((char*)result.data<T>(), byte_sz);
        file.close();
        return result;
    }

    template <typename T>
    T *data() const {
        return (T*)mem->origin;
    }

    /// immutable
    template <typename T>
    array unshift(T &v) const {
        array res { typeof(T), mem->count + 1 };
        T *data = array::data<T>();
        res += v;
        for (size_t i = 0; i < mem->count; i++)
            res += data[i];
        return res;
    }

    /// immutable 
    template <typename T>
    array shift() const {
        if (mem->count <= 1)
            return {};
        array res { typeof(T), mem->count - 1 };
        T *data = array::data<T>();
        for (size_t i = 1; i < mem->count; i++)
            res.push<T>(data[i]);
        return res;
    }

    /// likely need to do a constexpr to check if its able to concat
    /// in the condition its not it would be best to return default
    /// and not implement by other means
    template <typename T>
    T join(T s = T()) const {
        T *data = array::data<T>();
        T v = data[0];
        for (size_t i = 1; i < mem->count; i++)
            v += data[i];
        return v;
    }

    template <typename T>
    array reverse() const {
        array res;
        T *data = array::data<T>();
        for (size_t i = 0; i < mem->count; i++)
            res += ((T*)data)[mem->count - 1 - i];
        return res;
    }

    template <typename T>
    array slice(size_t start, size_t end) const {
        size_t count = end - start;
        assert(count <= mem->count);
        array res;
        T *data = array::data<T>();
        for (size_t i = start; i < start + count; i++) {
            res += ((T*)data)[i];
        }
        return res;
    }

    template <typename T>
    array every(lambda<bool(T&)> fn) const {
        array res(typeof(T), mem->count);
        for (T &v: elements<T>()) {
            if (fn(v))
                res.push(v);
        }
        return res;
    }

    template <typename T>
    bool some(lambda<bool(T&)> fn) const {
        bool res = false;
        for (T &v: elements<T>()) {
            if (fn(v)) {
                res = true;
                break;
            }
        }
        return res;
    }

    array &operator=(const array &b) {
        if (mem != b.mem) {
            ion::drop(mx::mem);
            mx::mem = ion::hold(b);
            assert(b.type() == type());
        }
        return *this;
    }

    static sz_t next_size(sz_t from) {
        return math::max(sz_t(16), from * 2);
    }

    /// push an element, return its reference
    template <typename T>
    T &push(const T &v) {
        if (mem->count >= alloc_size()) {
            assert(mem->type == typeof(array) || mem->data_type() == typeof(T));
            if (mem->type == typeof(array))
                mem->type = typeof(T);
            realloc(next_size(mem->count));
        }
        T *data = array::data<T>();
        new ((T*)&data[mem->count]) T(v);
        return (T&)data[mem->count++];
    }
    
    void *push_memory(memory* m) {
        if (mem->count >= alloc_size()) {
            assert(m->type == typeof(array) || m->data_type() == m->type);
            realloc(next_size(mem->count));
        }
        type_t e_type = element_type();
        assert(m->type == e_type);
        sz_t type_sz = m->type->base_sz;
        u8 *dst = &((u8*)mem->origin)[type_sz * mem->count];
        if ((e_type->traits & traits::mx) || (e_type->traits & traits::mx_obj))
            e_type->f.ctr_mem(dst, hold(m));
        else
            e_type->f.ctr_cp(dst, m->origin);
        mem->count++;
        return dst;
    }

    template <typename T>
    T &push() {
        T *data = array::data<T>();
        if (mem->count >= alloc_size()) {
            assert(mem->type == typeof(array) || mem->type == typeof(T));
            mem->type = typeof(T);
            realloc(next_size(mem->count));
        }
        new (&data[mem->count]) T();
        return data[mem->count++];
    }

    template <typename T>
    void push(T *pv, size_t push_count) {
        T *data = array::data<T>();
        if (!push_count) return; 
        ///
        size_t usz = size_t(mem->count);
        size_t rsv = size_t(mem->reserve);

        if (usz + push_count > rsv) {
            assert(mem->type == typeof(array) || mem->type == typeof(T));
            mem->type = typeof(T);
            realloc(usz + push_count);
        }
        
        if constexpr (is_primitive<T>()) {
            memcpy(&data[usz], pv, sizeof(T) * push_count);
        } else {
            /// call copy-constructor
            for (size_t i = 0; i < push_count; i++)
                new (&data[usz + i]) T(&pv[i]);
        }
        usz += push_count;
        mem->count = usz;
    }

    template <typename T>
    void pop(T *prev = null) {
        T *data = array::data<T>();
        size_t i = size_t(mem->count);
        assert(i > 0);
        if (prev)
            *prev = ((T*)data)[i];
        if constexpr (!is_primitive<T>())
            ((T*)data)[i]. ~T();
        --mem->count;
    }

    static inline type_t data_type() { return typeof(intern); }
    
    template <typename T>
    static array empty() { return array(typeof(T), size_t(1)); }

    void reserve_size(size sz) {
        if (mem->reserve < sz) {
            /// must have a type set!
            assert(mem->type != typeof(array));
            realloc(sz); // this was fill-default before, not for reserve so thats a bug
        }
    }

    void set_size(size_t sz) {
        mem->count = sz;
    }

    template <typename T>
    size_t count_of(const T& v) {
        T *data = array::data<T>();
        size_t c = 0;
        for (size_t i = 0; i < mem->count; i++)
            if (((T*)data)[i] == v)
                c++;
        return c;
    }

    template <typename T>
    int index_of(const T &v) const {
        T *data = array::data<T>();
        for (size_t i = 0; i < mem->count; i++) {
            if (((T*)data)[i] == (T&)v)
                return int(i);
        }
        return -1;
    }

    template <typename T>
    int index_of(lambda<bool(T&)> v) const {
        T *data = array::data<T>();
        for (size_t i = 0; i < mem->count; i++) {
            if (v(((T*)data)[i]))
                return int(i);
        }
        return -1;
    }

    template <typename T>
    bool contains(const T &v) const {
        return index_of(v) != -1;
    }

    template <typename R, typename T>
    R select_first(lambda<R(T&)> qf) const {
        T *data = array::data<T>();
        for (size_t i = 0; i < mem->count; i++) {
            R r = qf(((T*)data)[i]);
            if (r)
                return r;
        }
        if constexpr (is_numeric<R>())
            return R(0);
        else
            return R();
    }

    template <typename T, typename R>
    array map(lambda<R(T&)> qf) const {
        T *data = array::data<T>();
        array res(typeof(R), mem->count); /// we need to properly convert array<T>(sz) to this
        ///
        for (size_t i = 0; i < mem->count; i++) {
            R r = qf(((T*)data)[i]);
            res += r;
        }
        return res;
    }

    template <typename T, typename R>
    array flat_map(lambda<R(T&)> qf) const {
        T *data = array::data<T>();
        array res(typeof(R), mem->count);
        ///
        for (size_t i = 0; i < mem->count; i++) {
            array r = qf(((T*)data)[i]);
            for (R &ri: r.elements<R>())
                res += ri;
        }
        return res;
    }

    ///
    template <typename T, typename R>
    R select(lambda<R(T&)> qf) const {
        T *data = array::data<T>();
        array res(typeof(R), mem->count);
        ///
        for (size_t i = 0; i < mem->count; i++) {
            R r = qf(((T*)data)[i]);
            if (r)
                res += r;
        }
        return res;
    }

    /// quick-sort
    template <typename T>
    array sort(lambda<int(T &a, T &b)> cmp) {
        T *data = array::data<T>();
        /// create reference list of identical size as given, pointing to the respective index
        size_t sz = len();
        T **refs = (T **)calloc64(len(), sizeof(T*));
        for (size_t i = 0; i < sz; i++)
            refs[i] = &((T*)data)[i];

        /// recursion lambda
        lambda<void(int, int)> qk;
        qk = [&](int s, int e) {
            if (s >= e)
                return;
            int i  = s, j = e, pv = s;
            while (i < j) {
                while (cmp(*refs[i], *refs[pv]) <= 0 && i < e)
                    i++;
                while (cmp(*refs[j], *refs[pv]) > 0)
                    j--;
                if (i < j)
                    std::swap(refs[i], refs[j]);
            }
            std::swap(refs[pv], refs[j]);
            qk(s, j - 1);
            qk(j + 1, e);
        };

        /// launch recursion
        qk(0, len() - 1);

        /// create final result array from references

        size_t ln = len();
        array result { typeof(T), ln };
        for (size_t i = 0; i < ln; i++) {
            T &r = *refs[i];
            result.push(r);
        }

        free(refs);
        return result;
    }
    
    size     shape() const { return mem->shape ? *mem->shape : size(mem->count); }
    size_t     len() const { return mem->count; }
    size_t  length() const { return mem->count; }
    size_t reserve() const { return mem->reserve; }

    template <typename T>
    T &push_default()  { return push<T>();  }

    /// important that this should always assign or
    /// copy construct the elements in, as supposed to  a simple reference
    template <typename T>
    array mid(int start, int len = -1) const {
        T *data = array::data<T>();
        /// make sure we dont stride out of bounds
        int ilen = int(this->len());
        assert(abs(start) < ilen);
        if (start < 0) start = ilen + start;
        size_t cp_count = len < 0 ? math::max(0, (ilen - start) + len) : len; /// 0 is a value to keep as 0 len, auto starts at -1 (+ -1 from remaining len)
        assert(start + cp_count <= ilen);

        array result(typeof(T), cp_count);
        for (size_t i = 0; i < cp_count; i++)
            result.push(((T*)data)[start + i]);
        ///
        return result;
    }

    /// get has more logic in the indexing than the delim.  wouldnt think of messing with delim logic but this is useful in general
    template <typename T>
    T &get(num i) const {
        T *data = array::data<T>();
        if (i < 0) {
            i += num(mem->count);
            assert(i >= 0);
        }
        assert(i >= 0 && i < num(mem->count));
        return ((T*)data)[i];
    }

    template <typename T>
    T&  first() const {
        T *data = array::data<T>();
        return ((T*)data)[0];
    }

    template <typename T>
	T&   last() const {
        T *data = array::data<T>();
        return ((T*)data)[mem->count - 1];
    }
    
    virtual void realloc(size s_to) {
        mem->realloc(s_to, false);
    }

    operator  bool() const { return mx::mem && mem->count > 0; }
    bool operator!() const { return !(operator bool()); }
    bool operator==(const array &b) const;
    bool operator!=(array b) const { return !(operator==(b)); }
    
    template <typename T>
    T &operator[](size index) const {
        T *data = array::data<T>();
        if (index.count == 1) {
            size_t i = size_t(index.values[0]);
            assert(i >= 0 && i < mem->count);
            /// make sure this is within valid count range
            /// want to make sure all arrays even with primitive values do this
            return (T &)((T*)data)[i];
        } else {
            assert(mem->shape);
            mem->shape->assert_index(index);
            size_t i = mem->shape->index_value(index);
            return (T &)((T*)data)[i];
        }
    }

    template <typename T>
    T &operator[](size_t index) {
        T *data = array::data<T>();
        return ((T*)data)[index];
    }

    template <typename T, typename IX>
    T &operator[](IX index) {
        T *data = array::data<T>();
        if constexpr (identical<IX, size>()) {
            mem->shape->assert_index(index);
            size_t i = mem->shape->index_value(index);
            return (T &)((T*)data)[i];
        } else if constexpr (std::is_enum<IX>() || is_integral<IX>()) { /// just dont bitch at people.  people index by so many types.. most type casts are crap and noise as result
            size_t i = size_t(index);
            return (T &)((T*)data)[i];
        } else {
            /// now this is where you bitch...
            assert(!"index type must be size or integral type");
            return (T &)data[0];
        }
    }

    template <typename T>
    T &operator+=(const T &v) {
        return push(v);
    }

    void clear() {
        type_t t = mem->type;
        sz_t sz = t->base_sz;
        u8 *src = (u8*)mem->origin;
        assert(t->f.dtr);
        for (int i = 0; i < mem->count; i++) {
            void* ptr = &src[sz * i];
            t->f.dtr(ptr);
        }
        mem->count = 0;
    }

    void destruct() {
        clear();
    }
};

template <> struct is_array<array> : true_type { };

template<typename T>
struct Array:array {
    const static inline type_t intern_t = typeof(T); 
    using parent_class   = array;
    using context_class  = Array;
    using intern         = T;
    T* window;

    static type_t register_class() { return typeof(Array<T>); }
    static type_t register_data()  { return typeof(T); }

    void realloc(size s_to) override {
        array::realloc(s_to);
        window = mx::get<T>(0);
    }
    Array(memory*     mem) : array(mem), window((T*)mem->origin) { }
    Array(const mx     &o) : Array(hold(o.mem)) { }
    Array(const array &ar) : Array(hold(ar.mem)) { }
    Array(sz_t sz, sz_t count = 0) : array(typeof(T), sz, count) { }
    Array()                : array(memory::alloc(typeof(Array), 0, 1)) { }
    Array(std::initializer_list<T> a) : Array() {
        for (auto v: a) {
            push(v);
        }
    }
    T &operator[](num i) const {
        return window[i];
    }
    iter<T> begin() const { return iter<T>{(T*)mem->origin, 0};    }
    iter<T>   end() const { return iter<T>{(T*)mem->origin, mem->count}; }
};

/// make Array<T> use the function table for array (explicit use in ident::for_type)
template <typename T> struct is_array<Array<T>> : true_type { };
template <typename T> struct is_typed_array<Array<T>> : true_type { };



///
struct ex:mx {
    inline static const type_t intern_t = null;
    static bool matches(symbol a, symbol b, int len) {
        for (int i = 0; i < len; i++) {
            if (!a[i] || !b[i])
                return false;
            if (a[i] == b[i] || (a[i] == '-' && b[i] == '_') ||
                                (b[i] == '-' && a[i] == '_'))
                continue;
            return false;
        }
        return true;
    }
    ///
    template <typename C>
    ex(memory *m, C *inst) : mx(m) {
        mem->attach("container", m->type->ref, null); /// idata should facilitate this without attachment into doubly
    }

    /// called in construction by enum class
    template <typename C>
    static typename C::etype convert(mx raw, symbol S, C *p) {
        type_t type = typeof(C);
        ex::initialize((C*)null, (typename C::etype)0, S, type);
        mx psym;
        if (raw.type() == typeof(char)) {
            char  *d = raw.get<char>();
            /// in json, the enum can sometimes be given in "23124" form; no enum begins with a numeric so we can handle this
            if (d[0] == '-' || isdigit(*d)) {
                std::string str = (symbol)d;
                i64 num = (i64)std::stoi(str);
                psym = type->symbols->ids.lookup(num);
            } else {
                u64 hash = djb2(d);
                psym     = type->symbols->djb2.lookup(hash);
            }
            if (!psym) {
                /// if lookup fails, compare by the number of chars given
                for (memory *mem: type->symbols->list.elements<memory*>()) {
                    if (raw.mem->count > mem->count)
                        continue;
                    if (matches((symbol)mem->origin, (symbol)d, raw.mem->count))
                        return (typename C::etype)mem->id;
                }
            }
        } else if (raw.type() == typeof(int)) {
            i64   id = i64(*raw.get<int>());
            psym     = type->symbols->ids.lookup(id);
        } else if (raw.type() == typeof(i64)) {
            i64   id = *raw.get<i64>();
            psym     = type->symbols->ids.lookup(id);
        } else if (raw.type() == typeof(typename C::etype)) {
            i64   id = *raw.get<typename C::etype>();
            psym     = type->symbols->ids.lookup(id);
        }
        if (!psym) {
            printf("symbol: %s, raw: %s\n", S, (char*)raw.mem->origin);
            fflush(stdout);
            throw C();
        }
        return (typename C::etype)(psym.mem->id);
    }

    /// we need to bootstrap the symbols from the construction level of enum classes
    template <typename C, typename E>
    static E initialize(C *p, E v, symbol names, type_t ty);

    ///
    ex() : mx() { }
    ///
    template <typename E, typename C>
    ex(E v, C *inst) : ex(alloc<E>(&v), this) { }

    ex(memory *mem) : mx(mem) {}
};

/// useful for constructors that deal with ifstream
size_t length(std::ifstream& in);


using wstr = unsigned short*;

struct utf16:mx {
	using char_t = unsigned short; /// this is so at design time we can know what code to use using constexpr

	mx_object(utf16, mx, u16);

	utf16(size_t sz);
	utf16(char *input);

    utf16(symbol s);
    utf16(ion::wstr input, size_t len);

    utf16 operator+(symbol s);
	char_t &operator[](num index) const;
    int index_of(wchar_t find) const;
    utf16 escape(utf16 chars, char esc = '\\') const;
	utf16 mid(num start, num length = -1) const;
	size_t len() const;
	static utf16 join(const array &src, utf16 j);

    ion::wstr wstr() {
        return (ion::wstr)mem->origin;
    }

    cstr to_utf8() {
        int   sz = mem->count;
        cstr res = (cstr)calloc(1, sz + 1);
        #ifdef _WIN32
        WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)mem->origin, -1, res, sz, NULL, NULL);
        #else
        for (int i = 0; i < sz; i++)
            res[i] = (char)((char_t*)(mem->origin))[i];
        #endif
        res[sz] = 0;
        return res;
    }
};

using wchar = unsigned short;

struct str:mx {
    enum MatchType {
        Alpha,
        Numeric,
        WS,
        Printable,
        String,
        CIString
    };
    
    //using intern = char;
    //inline static const type_t intern_t = typeof(char);

    using char_t = unsigned short;

    mx_declare(str, mx, char);

    operator std::string();

    memory * symbolize();

    explicit operator symbol() {
        return (symbol)symbolize()->origin;
    }

    static str rand(size_t sz, char from, char to);

    str escape(str chars, char esc = '\\');

    /// \\ = \ ... \x = \x
    static str parse_quoted(cstr *cursor, size_t max_len = 0);

    str(i64 value, u8 base, int width);
    str(utf16 d);
    str(null_t);
    str(size_t sz);
    str(float          f32, int n = 6);
    str(double         f64, int n = 6);
    str(symbol cs, size_t len = memory::autolen, size_t rs = 0);
    str(std::string s);
    str(std::ifstream &in);

    /// static methods
    static str combine(const str sa, const str sb);

    /// skip to next non-whitespace
    static char &non_wspace(cstr cs);

    static void code_to_utf8(int code, char *output);

    static str from_code(int code);

    static str from_integer(i64 i);

    cstr cs() const;

    /// tested.
    str expr(lambda<str(str)> fn) const;

    /// format is a user of expr
    str format(const Array<mx> &args) const;

    /// just using cs here, for how i typically use it you could cache the strings
    static str format(symbol cs, const Array<mx> &args);

    operator fs::path() const;
    
    void        clear() const;

    bool        contains   (const array &a) const;
    str         operator+  (symbol       s) const;
    bool        operator<  (const str   &b) const;
    bool        operator>  (const str   &b) const;
    bool        operator<  (symbol       b) const;
    bool        operator>  (symbol       b) const;
    bool        operator<= (const str   &b) const;
    bool        operator>= (const str   &b) const;
    bool        operator<= (symbol       b) const;
    bool        operator>= (symbol       b) const;
  //bool        operator== (std::string  b) const;
  //bool        operator!= (std::string  b) const;
    bool        operator== (const str   &b) const;
    bool        operator!= (const str   &b) const;
    bool        operator== (symbol       b) const;
    bool        operator!= (symbol       b) const;
    char&		operator[] (size_t       i) const;
    int         operator[] (const str   &b) const;
                operator             bool() const;
    bool        operator!()                 const;
    str         operator+    (const str &sb) const;
    str        &operator+=   (const str &b);

    str &operator+= (const char b);

    str &operator+= (symbol b);

    /// add some compatibility with those iostream things.
    friend std::ostream &operator<< (std::ostream& os, str const& s) {
        return os << std::string(cstr(s.data));
    }

    bool iequals(str b) const;

    template <typename F>
    static str fill(size_t n, F fn) {
        auto ret = str(n);
        for (size_t i = 0; i < n; i++)
            ret += fn(num(i));
        return ret;
    }
    
    int index_of_first(const array &elements, int *str_index) const;

    bool starts_with(symbol s) const;

    size_t len() const;

    size_t utf_len() const;

    bool ends_with(symbol s) const;

    static str read_file(fs::path path);
                                  
    static str read_file(std::ifstream& in);
                                  
    str recase(bool lower = true) const;

    str ucase() const;
    str lcase() const;

    static int nib_value(char c);

    static char char_from_nibs(char c1, char c2);

    str replace(str fr, str to, bool all = true) const;

    /// mid = substr; also used with array so i thought it would be useful to see them as same
    str mid(num start, num len = -1) const;

    ///
    template <typename L>
    array split(L delim) const {
        array  result(typeof(str), 16);
        size_t start = 0;
        size_t end   = 0;
        cstr   pc    = (cstr)data;
        ///
        if constexpr (ion::inherits<str, L>() || identical<symbol, L>() || identical<cstr, L>()) {
            str s_delim = delim;
            int  delim_len = int(s_delim.byte_len());
            ///
            if (len() > 0) {
                while ((end = index_of(s_delim, int(start))) != -1) {
                    int test1 = 0;
                    test1++;
                    str  mm = mid(start, int(end - start));
                    
                    int test2 = 0;
                    test2++;

                    result += mm;

                    int test3 = 0;
                    test3++;

                    start   = end + delim_len;
                }
                result += mid(start);
            } else
                result += str();
        } else {
            ///
            if (mem->count) {
                for (size_t i = 0; i < mem->count; i++) {
                    int sps = delim(pc[i]);
                    bool discard;
                    if (sps != 0) {
                        discard = sps == 1;
                        result += str { &pc[start], (i - start) };
                        start   = discard ? int(i + 1) : int(i);
                    }
                    continue;
                }
                result += &pc[start];
            } else
                result += str();
        }
        ///
        return result;
    }

    iter<char> begin();
    iter<char>   end();

    array split(symbol s) const;
    array split();

    enum index_base {
        forward,
        reverse
    };

    int index_of(char b, int from = 0) const;

    int index_of(str b, int from = 0) const;

    int index_of(MatchType ct, symbol mp = null) const;

    i64 integer_value() const;

    template <typename T>
    T real_value() const {
        return T(ion::real_value<T>(mx::mem));
    }

    bool has_prefix(str i) const;
    bool numeric() const;
    bool matches(str input) const;
    str trim() const;
    size_t reserve() const;
};

template <typename C, typename E>
E ex::initialize(C *p, E v, symbol names, type_t ty) {
    /// names should support normal enum syntax like abc = 2, abc2 = 4, abc3, abc4; we can infer what C++ does to its values
    /// get this value from raw.origin (symbol) instead of the S
    if (ty->secondary_init) return v;
    ty->secondary_init = true;
    str snames = str((cstr)names);
    array   sp = snames.split(", ");
    int      c = (int)sp.len();
    i64   next = 0;

    Array<str> split(sp); /// this is just a cast with different windowing
                                 /// it has the operator[] overloaded
                                 /// will probably do the same for Map where we use it that way
    for (int i = 0; i < c; i++) {
        num idx = split[i].index_of(str("="));
        if (idx >= 0) {
            str sym = split[i].mid(0, idx).trim();
            str val = split[i].mid(idx + 1).trim();
            mem_symbol(sym.data, ty, val.integer_value());
        } else {
            mem_symbol(split[i].data, ty, i64(next));
        }
        next = i + 1;
    };
    return v;
}

template <typename T, typename = std::void_t<>> struct has_str_op: false_type {};
template <typename T> struct has_str_op<T, std::void_t<decltype(static_cast<str>(std::declval<T&>()))>> : true_type {};
template <typename T> constexpr bool has_str = has_str_op<T>::value;

enums(split_signal, none,
     none, discard, retain)

struct fmt:str {
    struct hex:str {
        template <typename T>
        cstr  construct(T* v) {
            type_t id = typeof(T);
            static char buf[1024];
            snprintf(buf, sizeof(buf), "%s/%p", id->name, (void*)v);
            return buf;
        }
        template <typename T>
        hex(T* v) : str(construct(v)) { }
    };

    /// format string given template, and mx-based arguments
    fmt(str templ, const Array<mx> &args) : str(templ.format(args)) { }
    fmt():str() { }

    operator memory*() { return ion::hold(mem); } // ? lol
};

template <typename T> using MetaFn = properties(*)(T*);

/// must cache by cstr, and id; ideally support any sort of id range
/// symbols 
using symbol_djb2  = hashmap; // <u64, memory*>;
using symbol_ids   = hashmap; // <i64, memory*>;
using     prop_map = hashmap; // <symbol, prop*>;

/// it makes sense to make schema for all types, not just mx.  it simplifies the allocation / realloc
/// the base (or top level), if not mx, shall not goto the else switch which is meant for mx
template <typename B, typename T>
size_t schema_info(alloc_schema *schema, int depth, B *top, T *p, idata *ctx_type) {
    /// for simple types (and mx base), we have a single schema entry of itself
    /// mx inherited types do not get a mx base because it would be redundant
    /// combine design-time check into something like is has_schema <B>, but the quirk is is_mx; basic mx classes have singular mx schema
    if constexpr (!is_hmap<B>::value && !is_doubly<B>::value && (!ion::inherits<ion::mx, B>() || is_mx<B>())) {
        if (schema->bind_count) {
            context_bind &bind = *schema->composition;
            bind.ctx     = ctx_type;
            bind.data    = ctx_type;
            bind.base_sz = sizeof(T);
            assert(sizeof(T) == ctx_type->base_sz);
            assert(typeof(T) == ctx_type);
        }
        return 1;
    } else {
        /// this crawls through mx-compatible for schema data
        if constexpr (!identical<T, none>() && !identical<T, mx>()) {
            /// count is set to schema_info after first return
            if (schema->bind_count) {
                context_bind &bind = schema->composition[schema->bind_count - 1 - depth]; /// [ base data ][ mid data ][ top data ]
                bind.ctx     = ctx_type ? ctx_type : typeof(T);
                bind.data    = T::register_data(); // this is defined in module and has access to the data class
                //bind.data    = identical<typename T::intern, none>() ? null : typeof(typename T::intern);
                bind.base_sz = bind.data ? bind.data->base_sz : 0; /// if array stores a str, it wont store char, so it must be its mx container.  if array stores char, its base sz is char.  if its a struct, same deal
            }
            typename T::parent_class *placeholder = null;
            return schema_info(schema, depth + 1, top, placeholder, null);
            ///
        } else
            return depth;
    }
    return 0;
}

template <typename T>
mx mx::move(const T &v) {
    return memory::alloc(typeof(T), 1, 1, (void*)&v);
}


/// scan schema info from struct from design-time info, then populate and sum up the total bytes along with the primary binding
template <typename T>
void schema_populate(idata *type, T *p) {
    type->schema         = (alloc_schema*)calloc64(1, sizeof(alloc_schema));
    alloc_schema &schema = *type->schema;
    schema.bind_count    = schema_info(&schema, 0, (T*)null, (T*)null, type);
    schema.composition   = (context_bind*)calloc64(schema.bind_count, sizeof(context_bind));
    schema_info(&schema, 0, (T*)null, (T*)null, type);
    size_t voffset = 0;
    for (size_t i = 0; i < schema.bind_count; i++) {
        context_bind &bind = schema.composition[i];
        bind.voffset       = voffset;
        voffset           += bind.base_sz;
    }
    schema.total_bytes = voffset;
    schema.bind = &schema.composition[schema.bind_count - 1];
}


/// wouldnt include the doubly or the hashmap; for those we wont have a function table
template<typename T>
struct is_allowed_type {
    static constexpr bool value = 
        !std::is_pointer_v  <T> && 
        !std::is_reference_v<T> &&
        (is_primitive<T>() || ion::inherits<mx, T>());

};

template<typename... Ts>
struct allowed_types {
    static constexpr bool value = (... && is_allowed_type<Ts>::value);
};

/// mix method
template <typename T, typename = void>
struct has_mix : std::false_type {};
template <typename T>
struct has_mix<T, std::void_t<decltype(&T::mix)>> : std::true_type {};

/// compare method
template <typename T, typename = void>
struct has_compare : std::false_type {};
template <typename T>
struct has_compare<T, std::void_t<decltype(&T::compare)>> : std::true_type {};

template<typename T, typename = void>
struct has_ctr_mem : std::false_type {};
template<typename T>
struct has_ctr_mem<T, std::enable_if_t<std::is_constructible<T, memory*>::value>> : std::true_type {};

template<typename T, typename = void>
struct has_ctr_type : std::false_type {};
template<typename T>
struct has_ctr_type<T, std::enable_if_t<std::is_constructible<T, memory*, type_t>::value>> : std::true_type {};

template<typename T, typename = void>
struct has_ctr_cp : std::false_type {};
template<typename T>
struct has_ctr_cp<T, std::enable_if_t<std::is_copy_constructible<T>::value>> : std::true_type {};

template<typename T, typename = void>
struct has_ctr_str : std::false_type {};
template<typename T>
struct has_ctr_str<T, std::enable_if_t<std::is_constructible<T, const str&>::value>> : std::true_type {};

/// converts T&, T* [const], to T
template <typename T>
struct pure_type_ {
    using type = typename std::remove_pointer<
        typename std::remove_reference<
            typename std::remove_const<T>::type
        >::type
    >::type;
};
template <typename T>
using pure_type = typename pure_type_<T>::type;

void register_type(memory *mem, const std::string &sig, sz_t sz, u64 traits);

template <typename TT>
idata *ident::for_type2() {
    ident::init();
    using T = pure_type<TT>;
    static type_t type; if (type) return type; /// assert the name and traits are set

    /// static identity: make sure we only proceed to pure type's definition; otherwise we are redefining type_t multiple times
    /// side effect is one more call
    if constexpr (!identical<T, TT>()) return for_type2<T>(); 

    memory *mem   = memory::raw_alloc((type_t)null, sizeof(idata), 1, 1);
    mem->type     = (type_t)calloc(1, sizeof(idata));
    mem->origin   = mem->type;
    type          = mem->type;

    bool is_mx    = ion::is_mx<T>(); /// this one checks just for type == mx
    bool is_obj   = !is_mx && ion::inherits<mx, T>(); /// used in composer for assignment; will merge in is_mx when possible

    u64 traits = (is_primitive<T> ()  ? traits::primitive : 0) |
                 (is_integral <T> ()  ? traits::integral  : 0) |
                 (is_realistic<T> ()  ? traits::realistic : 0) | // if references radioshack catalog
                 (is_singleton<T> ()  ? traits::singleton : 0) |
                 (is_array    <T> ()  ? traits::array     : 0) |
                 (is_typed_array<T> ()  ? traits::typed_array : 0) |
                 (is_lambda   <T> ()  ? traits::lambda    : 0) |
                 (is_map      <T> ()  ? traits::map       : 0) |
                 (is_mx               ? traits::mx        : 0) |
                 (has_etype<T>::value ? traits::mx_enum   : 0) |
                 (is_obj              ? traits::mx_obj    : 0);

    if constexpr (!is_lambda<T>()) {
        //type->f.mix     = (!identical<T, mx>() && has_mix<T>    ::value);
        //type->f.compare = (!identical<T, mx>() && has_compare<T>::value);
        
        if constexpr (std::is_class<T>::value && std::is_default_constructible<T>::value)
            type->f.ctr     = [](void* a)                -> void { new (a) T(); };
        if constexpr (has_ctr_str<T>::value)
            type->f.ctr_str = [](void* a, const str &v)  -> void { new (a) T(v); };
        if constexpr (!std::is_trivially_destructible<T>::value)
            type->f.dtr     = [](void* a)                -> void { ((T*)a) -> ~T(); };
        if constexpr (has_ctr_mem <T>::value)
            type->f.ctr_mem  = [](void* a, memory* mem)  -> void { new (a) T(mem); };
        if constexpr (std::is_copy_constructible<T>::value)
            type->f.ctr_cp   = [](void* a, void* b)      -> void { new (a) T(*(const T*)b); };
        if constexpr (has_ctr_type<T>::value)
            type->f.ctr_type = [](void* a, memory* mem)  -> void { new (a) T(mem, mem->type); }; /// user must implement this ctr
        if constexpr (has_bool    <T>)
            type->f.boolean  = [](void* a)               -> bool { return bool(*(T*)a); };
        if constexpr (has_str     <T>)
            type->f.str      = [](void* a)               -> memory* { str r(*(T*)a); return hold(a); };
        
        if constexpr (has_mix     <T>::value)
            type->f.mix      = [](void* a, void *b, void *c, double f) -> void {
                *(T*)c = ((T*)a)->mix(*(T*)b, f);
            };
        if constexpr (has_compare <T>::value)
            type->f.compare  = [](void* a, void *b) -> int {
                return ((T*)a)->compare(*(T*)b);
            };
        if constexpr (has_etype   <T>::value) {
            type_t etype = typeof(typename T::etype);
            etype->ref = type; ///
            etype->traits |= traits::enum_primitive;
            //type->ref = etype; // they reference each other
        }
        if constexpr (!identical<mx, T>() && ion::inherits<mx, T>())
            type->parent = typeof(typename T::parent_class);
        
    } else {
        using lcontainer = typename T::container;
        using traits     = lambda_traits<T>;
        using args_t     = typename traits::arg_types;
        using rtype      = typename traits::return_type;
        if constexpr (false)
        if constexpr (allowed_types<args_t>::value && (identical<void, rtype>() || is_convertible<rtype, mx>()))
            type->generic_lambda = new GenericLambda([type=type](void* ldata, const array &args) -> mx {
                ///
                lcontainer *data = (lcontainer*)ldata;
                constexpr size_t n_args = std::tuple_size_v<args_t>;
                if (args.len() != n_args)
                    throw std::runtime_error("arg count mismatch");
                mx result;
                #define ARG(N) \
                    using   T ## N = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<N, args_t>>>;\
                    type_t  t ## N = typeof(T ## N);\
                    T ## N* a ## N = (T ## N *)(t ## N)->f.ctr_type(null, args.get<str>(N).mem);
                if constexpr (n_args == 0) {
                    if constexpr (identical<void, rtype>()) 
                        (*data->lfn)();
                    else
                        result = (*data->lfn)();
                } else if constexpr (n_args == 1) {
                    ARG(0)
                    if constexpr (identical<void, rtype>())
                        (*data->lfn)(*a0);
                    else
                        result = (*data->lfn)(*a0);
                    delete a0;
                } else if constexpr (n_args == 2) {
                    ARG(0) ARG(1)
                    if constexpr (identical<void, rtype>())
                        (*data->lfn)(*a0, *a1);
                    else
                        result = (*data->lfn)(*a0, *a1);
                    delete a0; delete a1;
                } else if constexpr (n_args == 3) {
                    ARG(0) ARG(1) ARG(2)
                    if constexpr (identical<void, rtype>())
                        (*data->lfn)(*a0, *a1, *a2);
                    else
                        result = (*data->fn)(*a0, *a1, *a2);
                    delete a0; delete a1; delete a2;
                } else if constexpr (n_args == 4) {
                    ARG(0) ARG(1) ARG(2) ARG(3)
                    if constexpr (identical<void, rtype>())
                        (*data->lfn)(*a0, *a1, *a2, *a3);
                    else
                        result = (*data->lfn)(*a0, *a1, *a2, *a3);
                    delete a0; delete a1; delete a2; delete a3;
                }
                #undef ARG
                return result;
            });
    }

    register_type(mem, __PRETTY_FUNCTION__, sizeof(T), traits);

    if constexpr (!is_lambda<T>()) {
        if constexpr (ion::inherits<ion::mx, T>() || is_hmap<T>::value || is_doubly<T>::value)
            schema_populate(type, (T*)null);
        
        if constexpr (registered_instance_meta<T>()) {
             static T *def = new T();
            type->meta    = new doubly { def->meta() }; /// make a reference to this data
            for (prop &p: type->meta->elements<prop>()) {
                p.offset     = size_t(p.member_addr) - size_t(def);
                p.s_key      = new str(ion::hold(p.key));
                p.parent_type = type; /// we need to store what type it comes from, as we dont always have this context

                /// this use-case is needed for user interfaces without css defaults
                u8 *prop_def = &(((u8*)def)[p.offset]);
                p.init_value = calloc64(1, p.type->base_sz);
                if (p.type->f.ctr_cp)
                    p.type->f.ctr_cp(p.init_value, prop_def);
            }
            delete def;
            prop_map *pmap = new prop_map(size_t(16));
            doubly   *meta = (doubly*)type->meta;
            for (ion::prop &prop: meta->elements<ion::prop>()) {
                symbol prop_name = prop.name();
                mx sym = mem_symbol(prop_name);
                (*pmap)[sym] = mx::pointer(&prop);
            }
            type->meta_map = pmap;
        }
    }

    return type_t(type);
}
