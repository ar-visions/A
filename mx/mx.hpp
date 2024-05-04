#pragma once

#pragma warning(disable:4005) /// skia
#pragma warning(disable:4566) /// escape sequences in canvas
#pragma warning(disable:5050) ///
#pragma warning(disable:4244) /// skia-header warnings
#pragma warning(disable:5244) /// odd bug
#pragma warning(disable:4291) /// 'no exceptions'
#pragma warning(disable:4996) /// strncpy warning
#pragma warning(disable:4267) /// size_t to int warnings (minimp3)

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h> /// must do this if you include windows
#include <windows.h>
#else
#include <sched.h>
#endif

#include <functional>
#include <atomic>
#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <memory>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include <initializer_list>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <new>
#include <set>
#include <stack>
#include <queue>
#include <utility>
#include <type_traits>
#include <string.h>
#include <stdarg.h>
#include <typeinfo>
#include <inttypes.h>

#ifndef _WIN32
#include <unistd.h>
#include <limits.h>
#else
#include <direct.h>
#endif

#include <assert.h>

namespace ion {

#define A_decl(U, _A_) \
    private: \
    void init_type() { ((A*)data)->type = typeof(_A_); } \
    public: \
    struct _A_* data; \
    using intern = _A_;\
    U(const M& ref); \
    U(M& ref); \
    U(A* data); \
    U(_A_* data); \
    U(const U &b); \
    U(U &b); \
    U(); \
    template <typename... Arg> \
    U(Arg&&... arg) : data(new _A_(std::forward<Arg>(arg)...)) { init_type(); } \
    template <typename T> U& operator *= (const T& v) { (*data) *= v; return *this; }\
    template <typename T> U& operator /= (const T& v) { (*data) /= v; return *this; }\
    template <typename T> U& operator += (const T& v) { (*data) += v; return *this; }\
    template <typename T> U& operator -= (const T& v) { (*data) -= v; return *this; }\
    template <typename T> U  operator *  (const T& v) { return (*data) * v; }\
    template <typename T> U  operator /  (const T& v) { return (*data) / v; }\
    template <typename T> U  operator +  (const T& v) { return (*data) + v; }\
    template <typename T> U  operator -  (const T& v) { return (*data) - v; }\
    explicit operator bool() const; \
    operator M() const; \
    _A_* operator->() const; \
    operator _A_*() const; \
    operator _A_&() const; \
    U &operator=(const U &b); \
   ~U(); \

#define A_impl(U, _A_) \
    U::U(const M& ref)    : data((_A_*)(ref.a->hold())) { init_type(); } \
    U::U(M& ref)          : U((const M&)ref) { } \
    U::U(A* data)         : data((_A_*)data) { init_type(); } \
    U::U(_A_* data)       : data((_A_*)data) { init_type(); } \
    U::U(const U &b)      : data((_A_*)b.data->hold()) { } \
    U::U(U &b)            : U((const U&)b) { } \
    U::U()                : data(new _A_())  { data->type = typeof(_A_); } \
    _A_* U::operator->   () const { return data; } \
         U::operator _A_*() const { return (_A_*)(data ? data->hold() : null); } \
         U::operator _A_&() const { return *data; } \
    U   &U::operator=(const U &b) { \
        if (data != b.data) { \
            data->drop(); \
            data = (_A_*)b.data->hold(); \
        } \
        return *this; \
    } \
    U::operator bool() const { return data ? bool(*data) : false; } \
    U::operator M() const { return M(data->hold()); } \
    U::~U() { data->drop(); } \

#define A_type(_A_) { ((A*)data)->type = typeof(_A_); }
#define A_forward(_A_, ...)  data(new _A_(__VA_ARGS__)) A_type(_A_)


#define enums(C,D,...) \
    struct C { \
        private: \
        E* data; \
        void init_type() { \
            data->type = typeof(C); \
        } \
        public: \
        enum etype { __VA_ARGS__ }; \
        enum etype    value; \
        using intern = E; \
        static int         lookup_value(ion::symbol sym) { return typeof(C)->symbols->by_name [sym];   } \
        static ion::symbol lookup_name (i32       value) { return typeof(C)->symbols->by_value[M(value)]; } \
        static List &symbols() { return typeof(C)->symbols->list; } \
        inline static const int count  = num_args(__VA_ARGS__); \
        inline static const str raw   = str_args(__VA_ARGS__); \
        str name() { return (char*)symbol(); } \
        ion::symbol symbol() { return data->type->symbols->by_value[value]; } \
        C(enum etype t = etype::D):data(new E(E::initialize(t, (ion::symbol)raw, typeof(C)))), value(t) { init_type(); } \
        C(int t)                  :C((enum etype)t) { } \
        C(const str &sraw)       :C(E::convert(sraw, (ion::symbol)raw, typeof(C))) { } \
        C(const M &mraw)          :C(E::convert(mraw, (ion::symbol)raw, typeof(C))) { } \
        C(ion::symbol sym)        :C(E::convert(sym,  (ion::symbol)raw, typeof(C))) { } \
        C(E* mem)                 :data((E*)mem->hold()) { } \
        C(A* mem)                 :data((E*)mem->hold()) { } \
        operator   etype() const { return value; } \
        C &operator=(const C &b) { \
            if(data != b.data) { \
                if (data) data->drop(); \
                data = (E*)b.data->hold(); \
                value = b.value; \
            } \
            return *this;\
        } \
        bool    operator== (const enum etype &v) const { return value == v; }\
        bool    operator== (ion::symbol sym) const { \
            if (!data && !sym) \
                return true; \
            i32 id = data->type->symbols->by_name[sym]; \
            return id == (i32)value; \
        } \
        bool    operator!= (const enum etype &v) const { return value != v; } \
        bool    operator>  (const C &b)          const { return value >  b.value; } \
        bool    operator<  (const C &b)          const { return value <  b.value; } \
        bool    operator>= (const C &b)          const { return value >= b.value; } \
        bool    operator<= (const C &b)          const { return value <= b.value; } \
        explicit operator int() const   { return value; } \
        explicit operator i64() const   { return i64(value); } \
        operator str()                 { return symbol(); } \
    };

/// AR.. here be some fine types. #not-a-real-sailor
typedef signed char        i8;
typedef signed short       i16;
typedef signed int         i32;
typedef signed long long   i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef unsigned long      ulong;
typedef float              r32;
typedef double             r64;
typedef r64                real;
typedef char*              cstr;
typedef const char*        symbol;
typedef const char         cchar_t;
typedef i64                num;
typedef void*              handle_t;
typedef size_t             sz_t;

/// none is generic context, its internal is void.  so when we define origin it must be of the internal pointer
struct none {
    using intern = void;
};

#define debug_break() do { int test = 0; } while(0);

struct id;

using raw_t = void*;
using type_t = id *;

void*     calloc64(size_t count, size_t size);
void        free64(void* ptr);

using        null_t      = std::nullptr_t;
static const null_t null = nullptr;

struct true_type {
    static constexpr bool value = true;
    constexpr operator bool() const noexcept { return value; }
};

struct false_type {
    static constexpr bool value = false;
    constexpr operator bool() const noexcept { return value; }
};

template <typename> struct is_external   : false_type { };
template <typename> struct is_opaque     : false_type { };

template <typename T> const T nan() { return std::numeric_limits<T>::quiet_NaN(); };

constexpr bool  is_win() {
#if defined(_WIN32) || defined(_WIN64)
	return true;
#else
	return false;
#endif
};

constexpr bool  is_android() {
#if defined(__ANDROID_API__)
	return true;
#else
	return false;
#endif
};

constexpr bool  is_apple() {
#if defined(__APPLE__)
	return true;
#else
	return false;
#endif
};

#ifdef WIN32
void usleep(__int64 usec);
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define infinite_loop() { for (;;) { usleep(1000); } }

template <typename T, typename = std::void_t<>> struct has_bool_op: false_type {};
template <typename T> struct has_bool_op<T, std::void_t<decltype(static_cast<bool>(std::declval<T&>()))>> : true_type {};
template <typename T> constexpr bool has_bool = has_bool_op<T>::value;

template<typename T, typename = void>
struct has_intern : false_type { };
template<typename T>
struct has_intern<T, std::void_t<typename T::intern>> : true_type { };

template<typename T, typename = void>
struct has_parent : false_type { };
template<typename T>
struct has_parent<T, std::void_t<typename T::parent>> : true_type { };

template <typename T> constexpr bool is_realistic() { return std::is_floating_point<T>(); }
template <typename T> constexpr bool is_integral()  { return std::is_integral<T>(); }
template <typename T> constexpr bool is_numeric()   {
    return std::is_integral<T>()       ||
           std::is_floating_point<T>();
}

template <typename A, typename B>
constexpr bool inherits() { return std::is_base_of<A, B>(); }

template <typename A, typename B> constexpr bool identical()    { return std::is_same<A, B>(); }
template <typename T>             constexpr bool is_primitive() {
    return identical<T, std::nullptr_t>() || 
           identical<T, char*>() || 
           std::is_pointer<T>::value || 
           is_numeric<T>() || 
           std::is_enum<T>();
}
template <typename T>             constexpr bool is_class()        { return !is_primitive<T>() && std::is_default_constructible<T>(); }
template <typename T>             constexpr bool is_destructible() { return  is_class<T>()     && std::is_destructible<T>(); }
template <typename A, typename B> constexpr bool is_convertible()  { return std::is_same<A, B>() || std::is_convertible<A, B>::value; }

template <typename> struct is_lambda : false_type { };

template <typename T, typename = void>
struct has_etype : false_type {
    constexpr operator bool() const { return value; }
};

template <typename T>
struct has_etype<T, std::void_t<typename T::etype>> : true_type {
    constexpr operator bool() const { return value; }
};


template<typename T, typename Arg, typename = void>
struct has_constructor : std::false_type {};

template<typename T, typename Arg>
struct has_constructor<T, Arg, std::void_t<decltype(T(std::declval<Arg>()))>> : std::true_type {};

template <typename T, typename = void> struct registered_instance_meta : false_type { };
template <typename T>
struct registered_instance_meta<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().meta()), List>>> : true_type { };

template<typename T, bool hasIntern>
struct intern_of_ {
    using type = T;
};

template<typename T>
struct intern_of_<T, true> {
    using type = typename T::intern;
};

template<typename T>
using intern_of = typename intern_of_<T, has_intern<T>::value>::type;

constexpr int num_occurances(const char* cs, char c) {
    return cs[0] ? (cs[0] == c) + num_occurances(cs + 1, c) : 0; 
}

#define num_args(...) (num_occurances(#__VA_ARGS__, ',') + 1)
#define str_args(...) (str(#__VA_ARGS__))

template<typename...>
using void_t = void;

template <typename T> using func    = std::function<T>;

template <typename K, typename V> using umap = std::unordered_map<K,V>;
namespace fs  = std::filesystem;

template <typename T, typename B>
                      T mix(T a, T b, B f) { return a * (B(1.0) - f) + b * f; }
template <typename T> T radians(T degrees) { return degrees * static_cast<T>(0.01745329251994329576923690768489); }
template <typename T> T degrees(T radians) { return radians * static_cast<T>(57.295779513082320876798154814105); }

constexpr bool is_debug() {
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
}

static inline void yield() {
#ifdef _WIN32
    SwitchToThread();
#else
    sched_yield();
#endif
}

static inline void sleep(u64 u) {
    #ifdef _WIN32
        Sleep(u);
    #else
        usleep(useconds_t(u) * 1000);
    #endif
}

struct traits {
    enum bit {
        primitive       = 1,
        integral        = 2,
        realistic       = 4,
        singleton       = 8,
        lambda          = 16,
        array           = 32,
        typed_array     = 64,
        map             = 128,
        mx              = 256,
        mx_obj          = 512,
        init            = 1024, /// has an init method
        mx_enum         = 2048,
        enum_primitive  = 4096,
        opaque          = 8192,
        managed         = 16384,
        value           = 32768
    };
};

#define typeof(T)                      (id::for_type<T>())
#define signatureof(CL,M,MPTR,MPTR_SZ) (id::for_type<M,CL>((void*)MPTR,MPTR_SZ))

u64 fnv1a_hash(const void* data, size_t length, u64 hash = OFFSET_BASIS);

struct method_info {
    struct id** args;
    struct id*  r_type;
    int arg_count;
    std::function<struct A*(void*, struct M*, int)> call; /// for methods
};

struct id {
    struct id       *src;     ///
    cstr             name;
    size_t           base_sz; /// a types base sz is without regards to pointer state
    size_t           traits;
    bool             pointer; /// allocate enough space for a pointer to be stored
    struct Hash     *meta; // prop_map
    method_info     *method;

    id() { }

    id(bool);

    static inline struct hashmap *types;
    static inline id *char_t; /// cannot use hashmap/mx::hash without a registered char type
    static inline id *i64_t;
    static inline id *u64_t;

    struct f_table {
        std::function<void(void*)>         dtr;
        std::function<void*(void*)>        ctr;
        std::function<void*(void*, struct A*)>    ctr_mem;  /// for mx objects (assign from memory)
        std::function<void*(void*, struct String *)> ctr_str; /// for mx objects (copy convert)
        std::function<void*(void*,void*)>  ctr_cp;
        std::function<void(void*,void*,void*,double)> mix;
        std::function<bool(void*)>         boolean;
        std::function<u64(void*)>          hash;
        std::function<int(void*,void*)>    compare;
        std::function<struct M*(void*)>    m_ref;
        std::function<struct String*(void*)> to_str;
    } f;

    struct symbols  *symbols;
    id              *intern; /// the A-type inside a user class
    id              *ref; /// if you are given a primitive enum, you can find the schema and symbols on ref (see: to_string)
    id              *parent; 
    bool             secondary_init; /// used by enums but flag can be used elsewhere.  could use 'flags' too

    void   *alloc();
    void   *ctr();
    void    dtr(void* alloc);
    void   *ctr_mem (A *mem);
    void   *ctr_str (struct String *v);
    void   *ctr_cp  (void* b);
    size_t  data_size();
    template <typename TT, typename CL=none>
    static id *for_type(void *MPTR=nullptr, size_t MPTR_SZ=0);
    static void init();
};

template <> id *id::for_type<null_t>(void*, size_t);
template <> id *id::for_type<char>  (void*, size_t);
template <> id *id::for_type<i64>   (void*, size_t);
template <> id *id::for_type<u64>   (void*, size_t);

void register_type2(id *type, const std::string &sig, sz_t sz, u64 traits);

/// now that we have allowed for any, make entry for meta description
struct prop {
    struct M *key;
    size_t  member_addr;
    size_t  offset; /// this is set by once-per-type meta fetcher
    id*     type;
    id*     parent_type;
    raw_t   init_value; /// value obtained after constructed on the parent type
    bool    is_method; /// make sure we dont enumerate the data on methods

    prop() ;

    template <typename M>
    prop(symbol name, const M &member) ;
    
    template <typename M, typename CL>
    prop(symbol name, const M &member, const CL* inst) ;

    prop(const prop &ref);

    template <typename M>
    M &member_ref(void *m) ;

    void *member_pointer(void *m) ;

    symbol name() const ;

    struct String* to_string();
};

struct A { /// runtime of silver-lang
    id*     type;
    u64     h;
    int     refs; /// no schema, since we'll use traditional c++ 17
    A(id *type = null) : type(type), refs(1), h(0) { }
    A   *hold() { refs++; return this; }
    void drop() { if (--refs <= 0) { delete this; } }
    virtual u64 hash() {
        console.fault("implement hash() for type {0}", { str(type->name) });
        return 0;
    }
    virtual int compare(const struct M &ref) const {
        console.fault("implement compare() for type {0}", { str(type->name) });
        return -1;
    }
    virtual struct String *to_string() {
        console.fault("implement to_string() for type {0}", { str(type->name) });
        return null;
    }
    virtual ~A() { }
};

/// intentionally different from String
struct symbol:A {
    i32   id;
    int   len;
    char *name;
    u64 hash() {
        if (A::h) return A::h;
        A::h = 10000 + id;
        return (u64)id;
    }
    symbol(const char *name, i32 id = 0) : A(typeof(symbol)) {
        len  = strlen(name);
        this->id   = id;
        this->name = new char[len + 1];
        memcpy(this->name, name, len + 1);
    }
};

template <typename T, typename = void> struct has_to_string : false_type { };
template <typename T>
struct has_to_string<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().to_string()), String*>>> : true_type { };

template <typename T>
struct Vector; /// A-type

template <typename T>
struct vector; /// U-type

struct M;

using array = vector<M>; /// array will facilitate what we did before

/// always working on string theory..
struct String:A {
    char *chars;
    int   length;
    int   alloc;

    String(int size = 64) : A(typeof(String)) {
        chars = new char[size];
        alloc = size;
        length = 0;
        chars[0] = 0;
    }

    String(const char v) : A(typeof(String)) {
        length = 1;
        alloc  = 2;
        chars  = new char[2];
        memcpy(chars, &v, length);
        chars[length] = 0;
    }

    String(const char *v, int v_len = -1) : A(typeof(String)) {
        length = v_len == -1 ? strlen(v) : v_len;
        alloc = 64;
        while (alloc < length)
            alloc <<= 1;
        chars = new char[64];
        memcpy(chars, v, length);
        chars[length] = 0;
    }

    static String *input(int size) {
        String *str = new String(size);
        fgets(str->chars, size, stdin);
        return str;
    }

    String *format(arr *args);

    String(char *v, int v_len = -1) : String((const char *)v, v_len) { }

    arr *split(String *v) const;

    int index_of(String *s) const {
        char *f = strstr(chars, s->chars);
        s->drop();
        return f ? (int)(size_t)(f - chars) : -1;
    }

    String *mid(int start, int slen = -1) const {
        if (start < 0)
            start = start + length;
        int mid_len = slen >= 0 ? slen : (length - start + (slen + 1));
        assert(mid_len >= 0);
        return new String(&chars[start], mid_len);
    }

    String *trim() const {
        int offset = 0;
        while (chars[offset] == ' ') {
            offset++;
        }
        int slen = length - offset;
        for (;slen;slen--) {
            if (chars[offset + slen - 1] != ' ')
                break;
        }
        return new String(&chars[offset], slen);
    }

    char &operator[](int i) const {
        return chars[i];
    }

    void append(const char* v, int append_len = -1) {
        if (append_len == -1)
            append_len = strlen(v);
        if (alloc <= length - append_len) {
            alloc = math::max(128, alloc << 1);
            char *n = new char[alloc];
            memcpy(n, chars, length);
            memcpy(&n[length], &v, append_len);
            n[length += append_len] = 0;
            delete[] chars;
            chars = n;
        } else {
            memcpy(&chars[length], v, append_len);
            chars[length += append_len] = 0;
        }
    }

    void append(char v)                { append(&v, 1); }
    void append(String* v)             { append(v->chars, v->length); v->drop(); }
    void operator += (char          v) { append(&v, 1); }
    void operator += (const char*   v) { append(v, strlen(v)); }
    void operator += (String* v)       { append(v->chars, v->length); v->drop(); }

    explicit operator        cstr() const { return chars; }
    explicit operator ion::symbol() const { return ion::symbol(chars); }

    explicit operator i64() {
        char *last;
        return strtoll(chars, &last, 10);
    }

    explicit operator double() {
        char *last;
        return strtod(chars, &last);
    }

    int len() const { return length; }

    String* to_string() override {
        return (String*)hold();
    }

    u64 hash() {
        if (A::h) return A::h;
        return A::h = fnv1a_hash(chars, length);
    }

    int compare(const M &b) const override;

    explicit operator bool() const { return length > 0; }
};

template <typename> struct is_value : false_type { };

template <typename T>
struct Value:A {
    id *value_type;
    T value;

    Value(const T& v) : A(typeof(Value)), value(v), value_type(typeof(T)) { }

    int compare(const struct M &ref) const;
    
    u64 hash() {
        if (A::h) return A::h;
        id* type = typeof(T);
        if (type->traits & traits::primitive)
            return (A::h = fnv1a_hash(&value, sizeof(T)));
        if constexpr (ion::inherits<A, T>()) /// is also in traits (or will be)
            return (A::h = value->hash());
        console.fault("implement hash for non-A-type");
        return 0;
    }

    String *to_string() {
        char buf[128];
        if constexpr (identical<T, bool>()) {
            strcpy(buf, value ? "true" : "false");
        } else if constexpr (is_integral<T>()) {
            snprintf(buf, sizeof(buf), "%lld", (i64)value);
        } else if constexpr (is_realistic<T>()) {
            snprintf(buf, sizeof(buf), "%.4f", value);
        } else if constexpr (has_to_string<T>()) {
            return value.to_string();
        } else {
            printf("non-A type %s needs to_string\n", value_type->name);
        }
        return new String((ion::symbol)buf);
    }

    operator bool() const { return bool(value); }
};

template <typename T> struct is_value<Value<T>> : true_type { };

/// these can be managed or unmanaged
struct Pointer:A {
    id   *type;
    void *ptr;
    bool  managed;
    Pointer(id* type, void *ptr, bool managed) : A(null), type(type), ptr(ptr), managed(managed) { }
    ~Pointer() {
        if (managed) free(ptr);
    }
    int compare(const M& b) const override;
    u64 hash() {
        if (type->f.hash)
            return A::h = type->f.hash(ptr);
        return A::h = (u64)ptr;
    }
    String *to_string() {
        if (type->f.to_str)
            return type->f.to_str(ptr);
        char buf[128];
        snprintf(buf, sizeof(buf), "%s/%p", type->name, ptr);
        return new String(buf);
    }
};

struct M {
    union {
        Value<u64>  *v_u64;
        Value<u32>  *v_u32;
        Value<u16>  *v_u16;
        Value<u8>   *v_u8;
        Value<i64>  *v_i64;
        Value<i32>  *v_i32;
        Value<i16>  *v_i16;
        Value<i8>   *v_i8;
        Value<bool> *v_bool;
        Pointer     *v_pointer;
        String      *a_str;
        symbol     *a_symbol;
        A           *a;
    };
    M()           : a(null) { }
    M(null_t)     : M()     { }
    M(const u64  &i);
    M(const i64  &i);
    M(const u32  &i);
    M(const i32  &i);
    M(const u16  &i);
    M(const i16  &i);
    M(const u8   &i);
    M(const i8   &i);
    M(const char &i); /// we use const refs here so we have no ambiguity with the const T& ctr
    M(ion::symbol sym);
    M(cstr   str) : M((ion::symbol)str) { }
    M(A       *a) : a(a) { } /// this lets us pass around actual dynamically allocated A classes
    M(String  *a) : a(a) { } /// this lets us pass around actual dynamically allocated A classes
    M(const M& b) : a(b.a ? b.a->hold() : null) { }
    template <typename T>
    M(const T& any) {
        using TT = std::remove_pointer<T>::type;
        if constexpr (inherits<A, TT>()) {
            /// if its an A pointer, we can merely hold it (we never put A's on stack, this reference can be stack but it should not be)
            if constexpr (std::is_pointer<T>::value)
                a = ((TT*)any)->hold();
            else
                a = ((TT&)any).hold();
        } else if constexpr (has_intern<TT>()) {
            /// hold onto A class within user type
            a = any.data->hold();
        } else {
            /// this is so we can hold onto any data; it must be copied
            if constexpr (std::is_pointer<T>::value)
                a = new Pointer(typeof(TT), new TT((TT&)*any), true);
            else
                a = new Pointer(typeof(TT), new TT(any), true);
            a->type = typeof(Pointer);
        }
    }


    static M from_string(cstr cs, id* type) {
        assert(type);
        
        if (type == typeof(String)) {
            return new String(cs);
        } else if (type->traits & traits::integral) { /// u/i 8 -> 64
            if (type == typeof(bool)) {
                std::string s = std::string(cs);
                std::transform(s.begin(), s.end(), s.begin(),
                    [](unsigned char c){ return std::tolower(c); });
                return new bool(s == "true" || s == "1" || s == "tru" || s == "yes");
            }
            else if (type == typeof(u32))    return M(   u32(std::stoi(cs)));
            else if (type == typeof(i32))    return M(   i32(std::stoi(cs)));
            else if (type == typeof(u64))    return M(   u64(std::stoi(cs)));
            else if (type == typeof(i64))    return M(   i64(std::stoi(cs)));
            else if (type == typeof(u16))    return M(   u16(std::stoi(cs)));
            else if (type == typeof(i16))    return M(   i16(std::stoi(cs)));
            else if (type == typeof(u8))     return M(    u8(std::stoi(cs)));
            else if (type == typeof(i8))     return M(    i8(std::stoi(cs)));
            console.fault("unknown numeric conversion");
            return M();
        /// float / double
        } else if (type->traits & traits::realistic) {
            if (type == typeof(float))       return M( float(std::stod(cs)));
            else if (type == typeof(double)) return M(double(std::stod(cs)));
            console.fault("unknown real type conversion");
            return M();
        } else {
            bool mx_based = (type->traits & traits::mx_obj);
            sz_t len = strlen(cs);
            String *value = new String(cs);
            void *alloc = type->f.ctr_str(null, value);
            if (mx_based) {
                value->drop();
                return M((A*)alloc);
            }
            value->drop();
            return M::pointer(type, alloc, true);
        }
    }

    static M pointer(id* type, void *ptr, bool managed = false) {
        M result;
        result.v_pointer       = new Pointer(type, (void*)ptr, managed);
        result.v_pointer->type = type;
        result.a->type         = typeof(Pointer);
        return result;
    }

    template <typename T>
    static M pointer(T *ptr, bool managed = false) {
        return pointer(typeof(T), (void*)ptr, managed);
    }

    M& operator=(const M &b) {
        if (a != b.a) {
            if (a) a->drop();
            a = b.a->hold();
        }
        return *this;
    }
    u64 hash() {
        return a ? a->hash() : 0;
    }
    ~M() { if (a) a->drop(); }
    id *type() { return a ? a->type : typeof(null_t); }

    bool operator==(const M &ref) const {
        return compare(ref) == 0;
    }

    int compare(const M &ref) const {
        if (a == ref.a) return 0;
        if (a->type != ref.a->type) return -1;
        return a->compare(ref);
    }
    
    operator bool() const { return (a && a->type->f.boolean) ? a->type->f.boolean(a) : false; }
    
    static M    get(const M& o, const M& prop);
    static void set(const M& o, const M& prop, const M& value);

    template <typename T>
    static M method(T& obj, const struct str &name, const Vector<M> &args);

    template <typename T>
    T* get() const {
        //static id* type_check = typeof(T);
        //assert(type_check == a->type);
        return (T*)a;
    }

    template <typename T>
    operator T&() const {
        if (a->type == typeof(Pointer)) { /// can handle non-A inheritance
            assert(v_pointer->type == typeof(T)); /// inheritance check not supported here since it requires an alias protocol on the class
            return *(T*)v_pointer->ptr;
        }
        if (a->type->traits & traits::value) {
            Value<int> *v = v_i32;
            id* v_type = v_i32->value_type;
            void *ptr = &v->value;
            assert(v_type == typeof(T));
            return *(T*)ptr;
        } else {
            /// inheritance check (requires A-class)
            bool inherits_a = inherits<A, T>();
            assert(inherits_a);

            id *type = a->type;
            id *T_type = typeof(T);
            while (type) {
                if (type == T_type)
                    break;
                type = type->parent;
            }
            assert(type);
            return *(T*)a;
        }
    }

    ion::symbol symbol() const {
        if (a && a->type == typeof(String))
            return a_str    ? ion::symbol(a_str->chars)   : ion::symbol(null);
        else if (a && a->type == typeof(symbol))
            return a_symbol ? ion::symbol(a_symbol->name) : ion::symbol(null);
        return null;
    }

    String *to_string() {
        return a->to_string();
    }
};

template <typename M>
prop::prop(symbol name, const M &member) : key(new M(name)), member_addr((size_t)&member), type(typeof(M)), is_method(false) { }

template <typename M, typename CL>
prop::prop(symbol name, const M &member, const CL* inst) : 
    key(new M(name)), member_addr((size_t)&member), type(signatureof(CL, M, member_addr, sizeof(member))), is_method(true) { }

template <typename M>
M &prop::member_ref(void *M) { return *(M *)handle_t(&cstr(M)[offset]); }

template <typename T>
int Value<T>::compare(const struct M &ref) const {
    Value &b = *(Value*)ref.a;
    return value - b.value;
}

template <typename T> struct is_vector : false_type {};

struct iList:A {
    protected:
    int count;
    public:
    iList(id *type) : A(type) { count = 0; }
    virtual void remove(int index, int amount = 1) = 0;
    //virtual void push(const M &v) = 0; -- these abstract classes arent very useful; some of the list require M, some are T type; you cannot do that here.
    //virtual M pop() = 0;
    virtual void clear() = 0;
};

#include <mx/iterators.hpp>

template <typename T>
struct Vector:iList {
    friend vector<T>;

    private:
    T  *elements;
    int alloc;
    int count;

    public:
    Vector(int reserve = 32) : iList(typeof(Vector)) {
        alloc = 0;
        count = 0;
        elements = null;
        resize(reserve);
    }

    Vector(std::initializer_list<T> list) : Vector() {
        for (auto i: list)
            push(i);
    }

    T &get(int index) const {
        return elements[index];
    }

    void remove(int index, int amount = 1) {
        assert(count >= (index + amount));

        int w = index;
        for (int i = index + amount; i < count; i++)
            elements[w++] = elements[i];

        while (w < count)
            elements[w++].~T();

        count -= amount;
    }

    void push(const T &v) {
        if (alloc == count)
            resize(alloc << 1);
        
        new (&elements[count++]) T(v);
    }

    T pop() {
        assert(count > 0);
        T res = elements[count];
        elements[--count].~T();
        return res;
    }
    
    void operator += (const T &v) {
        push(v);
    }

    void clear() {
        int w = 0;
        while (w < count)
            elements[w++].~T();
        count = 0;
    }

    void resize(int sz) {
        if (sz == alloc) return;
        assert(sz >= count);
        alloc = sz;
        T *n = (T*)calloc(alloc, sizeof(T));
        for (int i = 0; i < count; i++)
            new (&n[i]) T(*(const T*)&elements[i]);
        free(elements);
        elements = n;
    }

    u64 hash() {
        bool is_prim = type->traits & traits::primitive;
        if (A::h) return A::h;
        u64 h = OFFSET_BASIS;
        for (int i = 0; i < count; i++) {
            h *= FNV_PRIME;
            h ^= is_prim ? fnv1a_hash(&elements[i], type->base_sz) : ((A*)&elements[i])->hash();
        }
        return A::h = h;
    }

    int len() const { return count; }

    iterator<T> begin() const { return iterator<T>{elements, 0};     }
    iterator<T>   end() const { return iterator<T>{elements, (size_t)count}; }

    explicit operator T*() const {
        return elements;
    }

    explicit operator bool() const { return count > 0; }
};

template <typename T> struct is_vector<Vector<T>> : true_type { };


struct List:iList {
    Item* first;
    Item* last;
    List() : iList(null) {
        first = null;
        last  = null;
        count = 0;
    }
    //template <typename T>
    List(std::initializer_list<M> args) : List() {
        for (auto i: args)
            push(i);
    }
    void clear() {
        while (last)
            remove(last);
    }
    void remove(Item *i) {
        Item *inext = i->next;
        Item *iprev = i->prev;
        if (inext) inext->prev = iprev;
        if (iprev) iprev->next = inext;
        if (first == i) first  = inext;
        if (last  == i) last   = iprev;
        delete i;
        count--;
    }
    void remove(int index, int amount = 1) {
        int f = 0;
        bool found = false;
        for (Item *i = first; i; i = i->next) {
            if (index == f++) {
                for (int ii = 0; ii < amount; ii++) {
                    assert(i);
                    Item *inext = i->next;
                    remove(i);
                    i = inext;
                }
                found = true;
                break;
            }
        }
        assert(found);
    }
    Item *push(const M &v) {
        Item *i = new Item(v);
        i->h = ((M&)v).hash();
        if (last) {
            last->next = i;
            i->prev    = last;
            last       = i;
        } else
            first      = (last = i);
        count++;
        return i;
    }
    M pop() {
        assert(last);
        M res = last->element;
        Item *ilast = last;
        last  = last->prev;
        if (!last)
            first = 0;
        delete ilast;
        return res;
    }

    void operator += (const M &v) {
        push(v);
    }
    operator bool() const { return count > 0; }

    template <typename T>
    list_iterable<T> elements() const { return list_iterable<T> { first, last }; }

    template <typename T>
    list_iterable_hash<T> elements(u64 hash) const { return list_iterable_hash<T>    { first, last, hash }; }

    list_iterable_items      items()         const { return list_iterable_items      { first, last }; }
    list_iterable_items_hash items(u64 hash) const { return list_iterable_items_hash { first, last, hash }; }
};

/// never use directly, like all A-classes
struct Field:A {
    M key;
    M value; /// these are more debuggable than simple A* pointers
    public:
    Field() : A(typeof(Field)) { }
    Field(const M& k, const M& v) : Field() {
        key   = k;
        value = v;
    }
    u64 hash() {
        if (A::h) return A::h;
        return A::h = key.hash();
    }
    String *to_string() override;
    operator bool() const { return key.a != null; }
};

template <typename T>
struct lambda;

template <typename R, typename... Args>
struct lambda<R(Args...)>:A {
    using fdata = std::function<R(Args...)>;
    fdata *lfn;

    ~lambda() {
        delete lfn;
    }
};

template <typename> struct is_lambda : false_type { };

template <typename T>
struct Lambda;

template <typename R, typename... Args>
struct Lambda<R(Args...)> {
    using ltype = lambda<R(Args...)>;
    ltype *data;
    Lambda(A* mem) : data(mem ? mem->hold() : null) { assert(!data || data->type == typeof(lambda<R(Args...)>)); }
    Lambda(ltype* mem) : Lambda((A*)mem) { }
    Lambda(const Lambda &b) : data(b.data->hold()) { }
    Lambda() : data(new lambda<R(Args...)>()) { data->type = typeof(lambda<R(Args...)>); }
    template <typename F>
    Lambda(F&& fn);
    Lambda &operator=(const Lambda &b) {
        if (data != b.data) {
            if (data) data->drop();
            data = b.data ? b.data->hold() : null;
        }
        return *this;
    }
    R operator()(Args... args) const { return (*data->lfn)(std::forward<Args>(args)...); }
    operator bool() const { return data && data->lfn && *data->lfn; }

};

template <typename T> struct is_lambda<Lambda<T>> : true_type { };

template <typename R, typename... Args>
template <typename F>
Lambda<R(Args...)>::Lambda(F&& fn) {
    if constexpr (ion::inherits<Lambda, F>() || is_lambda2<std::remove_reference_t<F>>::value) {
        data = fn.data->hold();
    } else {
        if constexpr (std::is_invocable_r_v<R, F, Args...>) {
            data = new lambda<R(Args...)>();
            data->lfn = new lambda<R(Args...)>::fdata(std::forward<F>(fn));
        } else {
            static_assert("F type is not a functor");
        }
    }
    data->type == typeof(lambda<R(Args...)>);
}

struct str {
    A_decl(str, String)
    operator         cstr() const { return        cstr(*data); }
    ion::symbol    symbol() const { return ion::symbol(*data); }
    operator       double() const { return      double(*data); }
    operator          i64() const { return         i64(*data); }
    char &operator[](int i) const { return (*data)[i]; }
};

struct field {
    A_decl(field, Field)
};

struct list {
    A_decl(list,  List)

    List(std::initializer_list<M> args) : A_forward(List, args)
};

struct Item {
    A_decl(Item, item)
};

template <typename T>
struct vector {
    private:
    struct avector<T>* data;
    public:
    using intern = avector<T>;

    vector()                : data(new avector<T>())             A_type(avector<T>)
    vector(avector<T>* data) : data((avector<T>*)data)            A_type(avector<T>)
    vector(const M& ref)    : data((avector<T>*)(ref.a->hold())) A_type(avector<T>)
    vector(M& ref)          : vector((const M&)ref)             { }
    vector(const vector &b) : data((avector<T>*)b.data->hold())  { }
    vector(vector &b)       : vector((vector &)b)               { }
    vector(int size)        : data(new avector<T>(size))         A_type(avector<T>)

    vector(std::initializer_list<T> args) : vector(args.size()) {
        for (auto a: args)
            data->push(a);
    }

    //template <typename... Arg>
    //vector(Arg&&... arg) : data(new avector<T>(std::forward<Arg>(arg)...)) A_type(avector<T>)

    vector& operator += (const T& v) { (*data) += v;     return *this; }
    vector& operator -= (int  index) { (*data) -= index; return *this; }

    iterator<T> begin() const { return data->begin(); }
    iterator<T>   end() const { return data->end(); }

    operator T*() const {
        return data->elements;
    }

    operator bool() const { return data->len() > 0; }
    operator M() const { return M((A*)data->hold()); }
    
    avector<T>* operator->() const { return data; }
    operator avector<T>*() const { return (avector<T>*)data->hold(); }
    vector &operator=(const vector &b) {
        if (data != b.data) {
            data->drop();
            data = b.data->hold();
        }
        return *this;
    }

    ~vector() {
        data->drop();
    }

    T &operator[](int index) const {
        assert(index >= 0 && index < data->len());
        return data->elements[index];
    }
};

/// type is not set on these primitive structs; we dont construct them ourselves, and the macro generally does this
/// the exception is here, in the M generics. for all uses of A-type, we set their type
M::M(const u64    &i) : v_u64(new Value<u64>(i))    { a->type = typeof(Value<u64>); }
M::M(const i64    &i) : v_i64(new Value<i64>(i))    { a->type = typeof(Value<i64>); }
M::M(const u32    &i) : v_u32(new Value<u32>(i))    { a->type = typeof(Value<u32>); }
M::M(const i32    &i) : v_i32(new Value<i32>(i))    { a->type = typeof(Value<i32>); }
M::M(const u16    &i) : v_u16(new Value<u16>(i))    { a->type = typeof(Value<u16>); }
M::M(const i16    &i) : v_i16(new Value<i16>(i))    { a->type = typeof(Value<i16>); }
M::M(const u8     &i) : v_u8 (new Value<u8 >(i))    { a->type = typeof(Value<u8>); }
M::M(const i8     &i) : v_i8 (new Value<i8 >(i))    { a->type = typeof(Value<i8>); }
M::M(const char   &i) : v_u8 (new Value<u8>((u8)i)) { a->type = typeof(Value<u8>); }
M::M(ion::symbol sym) : a_str(new String(sym)) { a->type = typeof(String);    }

struct Hashmap:A {
    private:
    list *h_fields;
    list *list;
    int    count;
    int    sz;
    public:

    int len() const {
        return count;
    }

    static u64 hash_value(M &key) {
        return key.hash();
    }

    static u64 hash_index(M &key, int mod) {
        return hash_value(key) % mod;
    }

    list &list_for_key(u64 k) {
        assert(sz > 0 && k < sz);
        return h_fields[k];
    }

    void push(const Field &f) {
        u64 k = hash_index(f->key, sz); // if sz is 1 for non-hash use-cases, that would be a reduction
        list &b = list_for_key(k);
        f->h = f->hash();
        item *i = b.push(f);
        i->peer = list->push(f);
        i->peer->hold();
        count++;
    }
    
    ~Hashmap() {
        delete h_fields;
        delete list;
    }

    Hashmap(int sz = 32);
    //Hashmap(std::initializer_list<Field> a) : Hashmap(a.size() > 0 ? a.size() : 32) {
    //    for (auto &v: a)
    //        push(v);
    //}

    template<typename... Fields>
    Hashmap(const Field& first, Fields&&... fields) : Hashmap(32) {
        push(first);
        // expand pack and push each Field
        (push(fields), ...);
    }

    item     *item(const M &key, list **list = null, u64 *phash = null);
    Field   *fetch(const M &key); /// we use Field here, because fetch is internal to hash and we store Field; any user who accepts a return value can contain it in a User type
    M        &value(const M &key);
    void        set(const M &key, const M &value);

    template <typename V>
    V &get(const M &k);

    M      lookup(const M &k);
    bool contains(const M& key);
    bool   remove(const M &key);

    M &operator[](const M &key);

    operator bool() { return *list; }

    template <typename T>
    list_iterable<T> elements() const { return list->elements<T>(); }

    template <typename T>
    list_iterable_hash<T> elements(u64 hash) const { return list->elements<T>(hash); }

    list_iterable_items      items()         const { return list->items(); }
    list_iterable_items_hash items(u64 hash) const { return list->items(hash); }
};

struct hashmap {
    A_decl(hashmap, Hashmap)

    template<typename... Fields>
    hashmap(const Field& first, Fields&&... fields) : data(new Hashmap(first, std::forward<Fields>(fields)...)) { }

    void set(const M& key, const M& val) {
        data->set(key, val);
    }

    M &operator[](const M &key) { return (*data)[key]; }

    M &operator[](i64 key) { return (*data)[key]; }
    M &operator[](i32 key) { return (*data)[key]; }
};

struct logger {
    inline static Lambda<void(M)> service;

    protected:
    static void _print(const str &st, const vector<M> &ar) {
        static std::mutex mtx;
        mtx.lock();
        str msg = st->format(ar);
        if (service) service(msg);
        fputs(msg->chars, stdout);
        fputs("\n", stdout);
        fflush(stdout);
        mtx.unlock();
    }

    public:
    inline void log(const M& msg, const vector<M> &ar = {}) {
        _print(((M&)msg).to_string(), ar);
    }

    void test(const bool cond, const M& str = {}, const vector<M> &ar = {}) {
        #ifndef NDEBUG
        if (!cond) {
            _print(((M&)str).to_string(), ar);
            exit(1);
        }
        #endif
    }

    static str input() {
        return String::input(1024);
    }

    inline void fault(const M& msg, const vector<M>& ar = { }) { _print(((M&)msg).to_string(), ar); brexit(); }
    inline void error(const M& msg, const vector<M>& ar = { }) { _print(((M&)msg).to_string(), ar); brexit(); }
    inline void print(const M& msg, const vector<M>& ar = { }) { _print(((M&)msg).to_string(), ar); }
    inline void debug(const M& msg, const vector<M>& ar = { }) { _print(((M&)msg).to_string(), ar); }
};

extern logger console;

struct symbols {
    Hash   by_name  { };
    Hash   by_value { };
    List   list;
};

void register_type(id *type, const std::string &sig, sz_t sz, u64 traits);

template <typename TT, typename CL>
id *id::for_type(void *MPTR, size_t MPTR_SZ) {
    id::init();
    using T = pure_type<TT>;

    static id* type; if (type) return type; /// assert the name and traits are set

    /// static identity: make sure we only proceed to pure type's definition; otherwise we are redefining id*    multiple times
    /// side effect is one more call
    if constexpr (!identical<T, TT>()) return for_type<T>(); 

    type       = new id();
    memset(type, 0, sizeof(id));
    bool is_A  = ion::inherits<A, T>();
    u64 traits = (is_primitive<T> ()  ? traits::primitive : 0) |
                 (is_integral <T> ()  ? traits::integral  : 0) |
                 (is_realistic<T> ()  ? traits::realistic : 0) | // if references radioshack catalog
                 (is_array    <T> ()  ? traits::array     : 0) |
                 (is_lambda2  <T> ()  ? traits::lambda    : 0) |
                 (is_map      <T> ()  ? traits::map       : 0) |
                 (is_value    <T> ()  ? traits::value     : 0) |
                 (has_etype<T>::value ? traits::mx_enum   : 0) |
                 (is_A                ? traits::mx_obj    : 0);

    if constexpr (has_intern<T>())
        type->intern = id::for_type<typename T::intern>(MPTR, MPTR_SZ);
    
    if constexpr (std::is_member_function_pointer<T>::value) {
        using R = typename member_fn<T>::r_type;
        type->method = new method_info {
            .args      = (id**)calloc(member_fn<T>::n_args, sizeof(id*)),
            .r_type    = typeof(R),
            .arg_count = member_fn<T>::n_args
        };

        #undef member_init
        #define member_init(...) \
            CL *obj = (CL*)inst; \
            R (CL::*member_func)(__VA_ARGS__); \
            assert(sizeof(member_func) == MPTR_SZ); \
            memcpy((void*)&member_func, (void*)MPTR_MEM, MPTR_SZ); \
            M result;

        #undef member_call
        #define member_call(...) \
            if constexpr (identical<R, void>()) \
                (obj->*member_func)(__VA_ARGS__); \
            else \
                result = (obj->*member_func)(__VA_ARGS__); \
            return result.a ? result.a->hold() : (A*)null;
        
        u8 MPTR_MEM[1024];
        memcpy(MPTR_MEM, MPTR, MPTR_SZ);
        if constexpr (member_fn<T>::n_args == 0) {
            type->method->call = [MPTR_MEM, MPTR_SZ](void* inst, M * args, int n_args) -> A* {
                member_init()
                member_call()
            };
        }
        else if constexpr (member_fn<T>::n_args == 1) {
            using A0 = typename member_fn<T>::argument<0>::type;
            type->method->args[0] = typeof(A0);
            type->method->call = [MPTR_MEM, MPTR_SZ](void* inst, M * args, int n_args) -> A* {
                A0 a0 = M(args[0]);
                member_init(A0)
                member_call(a0)
            };
        }
        else if constexpr (member_fn<T>::n_args == 2) {
            using A0 = typename member_fn<T>::argument<0>::type;
            using A1 = typename member_fn<T>::argument<1>::type;
            type->method->args[0] = typeof(A0);
            type->method->args[1] = typeof(A1);
            type->method->call = [MPTR_MEM, MPTR_SZ](void* inst, M * args, int n_args) -> A* {
                A0 a0 = M(args[0]);
                A1 a1 = M(args[1]);
                member_init(A0, A1)
                member_call(a0, a1)
            };
        }
        else if constexpr (member_fn<T>::n_args == 3) {
            using A0 = typename member_fn<T>::argument<0>::type;
            using A1 = typename member_fn<T>::argument<1>::type;
            using A2 = typename member_fn<T>::argument<2>::type;
            type->method->args[0] = typeof(A0);
            type->method->args[1] = typeof(A1);
            type->method->args[2] = typeof(A2);
            type->method->call = [MPTR_MEM, MPTR_SZ](void* inst, M * args, int n_args) -> A* {
                A0 a0 = M(args[0]);
                A1 a1 = M(args[1]);
                A2 a2 = M(args[2]);
                member_init(A0, A1, A2)
                member_call(a0, a1, a2)
            };
        }
        else if constexpr (member_fn<T>::n_args == 4) {
            using A0 = typename member_fn<T>::argument<0>::type;
            using A1 = typename member_fn<T>::argument<1>::type;
            using A2 = typename member_fn<T>::argument<2>::type;
            using A3 = typename member_fn<T>::argument<3>::type;
            type->method->args[0] = typeof(A0);
            type->method->args[1] = typeof(A1);
            type->method->args[2] = typeof(A2);
            type->method->args[3] = typeof(A3);
            type->method->call = [MPTR_MEM, MPTR_SZ](void* inst, M * args, int n_args) -> A* {
                A0 a0 = M(args[0]);
                A1 a1 = M(args[1]);
                A2 a2 = M(args[2]);
                A3 a3 = M(args[3]);
                member_init(A0, A1, A2, A3)
                member_call(a0, a1, a2, a3)
            };
        }
        else {
            static_assert("implement more args");
        }
    } else if constexpr (!is_lambda2<T>() && !inherits<A, T>()) {
        using K = intern_of<T>;

        if constexpr (std::is_default_constructible<T>::value)
            type->f.ctr      = [](void *a) -> void* { if (a) new (a) T(); else a = (void*)new T(); return a; };
        
        if constexpr (has_ctr_str<K>::value)
            type->f.ctr_str  = [](void* a, String *v) -> void* { str sv((A*)v); if (a) new (a) T(sv); else a = (void*)new T(sv); return a; };
        
        if constexpr (!std::is_trivially_destructible<T>::value)
            type->f.dtr      = [](void* a) -> void { ((T*)a) -> ~T(); };
        
        if constexpr (has_constructor<T, A*>::value)
            type->f.ctr_mem  = [](void *a, A* mem) -> void* { if (a) new (a) T(mem); else a = (void*)new T(mem); return a; };
        
        if constexpr (std::is_copy_constructible<T>::value)
            type->f.ctr_cp   = [](void* a, void* b) -> void* { if (a) new (a) T(*(const T*)b); else a = (void*)new T(*(const T*)b); return a; };

        if constexpr (has_bool<T>)
            type->f.boolean  = [](void* a) -> bool { return bool(*(T*)a); };

        if constexpr (has_to_string<T>())
            type->f.to_str   = [](void* a) -> String* { return ((T*)a)->to_string(); };

        if constexpr (has_hash<T>())
            type->f.hash     = [](void* a) -> u64 { return ((T*)a)->hash(); };

        if constexpr (has_compare<T>())
            type->f.compare  = [](void* a, void* b) -> int { return ((T*)a)->compare(*(T*)b); };

        if constexpr (is_convertible<T, M>())
            type->f.m_ref    = [](void* a) -> M* { return new M(M::pointer((T*)a, false)); };

        /// mix is useful on trivial types, like vec2/3/4 rgba/8/32f
        if constexpr (has_mix<T>::value)
            type->f.mix      = [](void* a, void *b, void *c, double f) -> void {
                *(T*)c = ((T*)a)->mix(*(T*)b, f);
            };
    
        if constexpr (has_etype<T>::value) {
            id*    etype = typeof(typename T::etype);
            etype->ref = type; ///
            type->ref = etype;
            etype->traits |= traits::enum_primitive;
        }
    }

    if constexpr (ion::inherits<A, T>()) {
        if constexpr (has_parent<T>())
            type->parent = typeof(typename T::parent);
    }

    register_type(type, __PRETTY_FUNCTION__, sizeof(T), traits);

    if constexpr (!is_lambda2<T>()) {
        if constexpr ((inherits<A, T>() || !has_intern<T>()) && registered_instance_meta<T>()) {
            static T *def = new T();
            List mlist = def->meta();
            type->meta = new hashmap(16);
            for (M &element: mlist->elements<M>()) {
                prop &pr = element;
                pr.offset = pr.is_method ? 0 : (size_t(pr.member_addr) - size_t(def));
                pr.parent_type = type;
                /// obtain default value for this member
                if (!pr.is_method) {
                    u8 *prop_def = &(((u8*)def)[pr.offset]);
                    if (pr.type->f.ctr_cp)
                        pr.init_value = pr.type->f.ctr_cp(null, prop_def);
                }
                (*type->meta)->set(*pr.key, element);
            }
            delete def;
        }
    }

    return (id*)type;
}

M symbolize(cstr cs, id* type = typeof(String), i32 id = 0);

struct E:Value<i32> {
    static int convert(M raw, ion::symbol S, id* type) {
        initialize(0, S, type);
        M sym;
        if (raw.type() == typeof(symbol))
            return raw.a_symbol->id;
        if (raw.type() == typeof(String)) {
            str raw_str = raw;
            /// in json, the enum can sometimes be given in "23124" form; no enum begins with a numeric so we can handle this
            if (raw_str[0] == '-' || isdigit(raw_str[0])) {
                i32 id   = (i32)i64(raw_str);
                sym      = type->symbols->by_value[id];
            } else {
                sym      = type->symbols->by_name[raw];
            }
            if (!sym) {
                /// if lookup fails, compare by the number of chars given
                for (symbol &f: type->symbols->list->elements<symbol>()) { /// changing this from str to a Field (so we may have the id on symbols)
                    if (raw_str->len() != f.len)
                        continue;
                    if (matches((ion::symbol)f.name, (ion::symbol)raw_str, raw_str->len()))
                        return f.id;
                }
            }
        } else if (raw.type() == typeof(i32) || raw.type() == type->ref) { // make sure ref is set
            i32   id = i32(raw);
            sym      = type->symbols->by_value[id];
        } else if (raw.type() == typeof(i64)) {
            i32   id = i32(i64(raw));
            sym      = type->symbols->by_value[id];
        }
        if (!sym) {
            printf("symbol: %s, raw: %s\n", S, (ion::symbol)raw);
            fflush(stdout);
            return 0;
        }
        return sym.a_symbol->id;
    }

    static int initialize(int v, ion::symbol names, id* type) {
        if (type->secondary_init) return v;
        type->secondary_init = true;
        str snames = str(names);
        arr     *sp = snames->split(new String(", "));
        int       c = sp->len();
        i32    next = 0;
        String   *e = new String("=");
        int       i = 0;
        for (String &s: *sp) {
            num idx = s.index_of((String*)e->hold());
            if (idx >= 0) {
                str sym = s.mid(0, idx);
                sym = sym->trim();
                str val = s.mid(idx + 1);
                val = val->trim();
                symbolize(sym, type, i32(i64(val)));
            } else {
                str sym = s.trim();
                symbolize(sym, type, i32(next));
            }
            next = i + 1;
        };
        e->drop();
        return v;
    }

    E(int value) : Value<i32>(value) {
    }

    static bool matches(ion::symbol a, ion::symbol b, int len) {
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

    ion::symbol symbol() {
        assert(type == typeof(Value<i32>));
        i32 v = value;
        assert(type->symbols);
        M mem_symbol = type->symbols->by_value[v];
        if (!mem_symbol) printf("symbol: mem is null for value %d\n", (int)v);
        assert(mem_symbol);
        return (ion::symbol)mem_symbol;
    }

    String *to_string() override { return type->symbols->by_value[i32(value)].a_str; }
};

template <typename T>
M M::method(T& obj, const str &name, const vector<M> &args) {
    id* type = typeof(T);
    if (type->intern)
        type = type->intern;
    Field *f = (*type->meta)->fetch(name);
    assert(f);
    prop &pr = f->value;
    method_info *method = pr.type->method;

    M result;
    if (method) {
        int n_args = args->len();
        assert(n_args == method->arg_count);

        M *mem_args = (M*)calloc(n_args, sizeof(M));
        for (int a = 0; a < n_args; a++) {
            M  &src = args[a];
            M  &dst = mem_args[a];
            id *atype = method->args[a];
            if (src.a->type != atype) {
                M   msrc(src.a->hold());
                String *st = msrc.to_string();
                M   conv = M::from_string(st->chars, atype);
                st->drop();
                dst = M(conv.a->hold());
            } else
                dst = src;
        }
        result = method->call((void*)&obj, mem_args, n_args);
        for (int a = 0; a < n_args; a++)
            mem_args[a].a->drop();
        free(mem_args);
    }
    return result;
}

enums(EnumTest, undefined,
    undefined, one, two, three, four)

template <typename T, const size_t SZ>
struct Buffer:A {
    T      values[SZ];
    size_t count;
    Buffer(id* type = typeof(Buffer)) : A(type), count(0) { }
    Buffer(std::initializer_list<T> args) : Buffer() {
        assert(args.size() < SZ);
        for (auto a: args)
            values[count++] = a;
    }
};

struct Size:Buffer<num, 16> {
    using base = Buffer<num, 16>;

    Size(num        sz = 0);
    Size(null_t           );
    Size(size_t         sz);
    Size(i8             sz);
    Size(u8             sz);
    Size(i16            sz);
    Size(u16            sz);
    Size(i32            sz);
    Size(u32            sz);

    Size(std::initializer_list<num> args);

    size_t    x() const;
    size_t    y() const;
    size_t    z() const;
    size_t    w() const;
    num    area() const;
    size_t dims() const;

    void assert_index(const Size &b) const;

    bool operator==(size_t b) const;
    bool operator!=(size_t b) const;
    bool operator==(Size &sb) const;
    bool operator!=(Size &sb) const;
    
    void   operator++(int);
    void   operator--(int);
    Size  &operator+=(size_t sz);
    Size  &operator-=(size_t sz);

    num &operator[](size_t index) const;

    Size &zero();

    template <typename T>
    size_t operator()(T *addr, const Size &index) {
        size_t i = index_value(index);
        return addr[i];
    }
    
             operator num() const;
    explicit operator  i8() const;
    explicit operator  u8() const;
    explicit operator i16() const;
    explicit operator u16() const;
    explicit operator i32() const;
    explicit operator u32() const;
  //explicit operator i64() const;
    explicit operator u64() const;
    explicit operator r32() const;
    explicit operator r64() const;

    num &operator[](num i);

    Size &operator=(i8   i);
    Size &operator=(u8   i);
    Size &operator=(i16  i);
    Size &operator=(u16  i);
    Size &operator=(i32  i);
    Size &operator=(u32  i);
    Size &operator=(i64  i);
    Size &operator=(u64  i);

    Size &operator=(const Size b);

    size_t index_value(const Size &index) const;
};

struct str;
struct array;

#undef min
#undef max

struct math {
    template <typename T>
    static inline T min (T a, T b)                             { return a <= b ? a : b; }
    template <typename T> static inline T max  (T a, T b)      { return a >= b ? a : b; }
    template <typename T> static inline T clamp(T v, T m, T x) { return v >= x ? x : x < m ? m : v; }
    template <typename T> static inline T round(T v)           { return std::round(v);  }
};

struct attachment {
    const char    *id;
    void          *data;
    func<void()>   release;
};

using mutex = std::mutex;

template <typename T>
T *defaults() {
    static T def_instance;
    return  &def_instance;
}

static constexpr uint64_t FNV_PRIME    = 0x100000001b3;
static constexpr uint64_t OFFSET_BASIS = 0xcbf29ce484222325;

template <typename T>
using fun = std::function<T>;

using properties = List;

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

/// must cache by cstr, and id; ideally support any sort of id range
/// symbols 
using symbol_djb2  = hashmap; // <u64, memory*>;
using symbol_ids   = hashmap; // <i64, memory*>;
using     prop_map = hashmap; // <symbol, prop*>;

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

template <typename T, typename = void>
struct has_hash : std::false_type {};
template <typename T>
struct has_hash<T, std::void_t<decltype(&T::hash)>> : std::true_type {};

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
template <typename T, bool isMemberFunctionPointer = std::is_member_function_pointer<T>::value>
struct pure_type_ {
    using type = typename std::remove_pointer<
        typename std::remove_reference<
            typename std::remove_const<T>::type
        >::type
    >::type;
};

template <typename T>
struct pure_type_<T, true> {
    using type = T;  // Just pass the type through unchanged
};

template <typename T>
using pure_type = typename pure_type_<T>::type;

// template to extract information from a member function pointer
template<typename T>
struct member_fn;

template<typename R, typename C, typename... Args>
struct member_fn<R(C::*)(Args...)> {
    using r_type = R;
    static constexpr std::size_t n_args = sizeof...(Args); // Number of arguments

    template<std::size_t N>
    struct argument {
        static_assert(N < n_args, "Error: invalid parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};

using fn_t = func<void()>;

struct rand {
    struct seq {
        enum seeding {
            Machine,
            Seeded
        };
        seq(i64 seed, seq::seeding t = Seeded) {
            if (t == Machine) {
                std::random_device r;
                e = std::default_random_engine(r());
            } else
                e.seed(u32(seed)); // windows doesnt have the same seeding 
        }
        std::default_random_engine e;
        i64 iter = 0;
    };

    static inline seq global = seq(0, seq::Machine);

    static std::default_random_engine& global_engine() {
        return global.e;
    }

    static void seed(i64 seed) {
        global = seq(seed);
        global.e.seed(u32(seed)); // todo: seed with 64b, doesnt work on windows to be 32b seed
    }

    template <typename T>
    static T uniform(T from, T to, seq& s = global) {
        real r = std::uniform_real_distribution<real>(real(from), real(to))(s.e);
        return   std::is_integral<T>() ? T(math::round(r)) : T(r);
    }

    static bool coin(seq& s = global) { return uniform(0.0, 1.0) >= 0.5; }
};

using arg = field;
//using ax  = Array<arg>;

struct io:mx {
    struct Source {
        type_t           type; /// types are the stream identifier
        lambda<void(mx)> fn;   /// 'sink' of sort
    };

    struct M {
        array sources   { typeof(Source), 1 };
        array types_out { typeof(type_t), 1 };
        lambda<void()> shutdown;
        ~M() {
            /// stream shutdown
            if (shutdown)
                shutdown();
        }

        void emit(mx data) {
            type_t type = data.type();
            for (Source &src: sources.elements<Source>()) {
                if (src.type == type)
                    src.fn(data);
            }
        }
        
        bool attach(Source &src) {
            bool valid = types_out.index_of<type_t>([&](Source &a) -> bool {
                return a.type == src.type;
            }) >= 0;
            assert(valid);
            sources += src;
            return valid;
        }
    };
    
    mx_basic(io)

    io(const Array<type_t> &types_out, const lambda<void()> &shutdown) : io() {
        data->types_out = types_out;
        data->shutdown  = shutdown;
    }

    bool operator+=(const Source &src) {
        return data->attach((Source &)src);
    }
};

// todo: was this needed for var/json?
//external(arg);

using ichar = int;

/// UTF8 :: Bjoern Hoehrmann <bjoern@hoehrmann.de>
/// https://bjoern.hoehrmann.de/utf-8/decoder/dfa/
struct utf {
    static inline int decode(u8* cs, lambda<void(u32)> fn = {}) {
        if (!cs) return 0;

        static const u8 utable[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
            7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
            8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
            0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
            0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
            0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
            1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
            1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
            1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
        };

        u32    code;
        u32    state = 0;
        size_t ln    = 0;
        ///
        auto     dc    = [&](u32 input) {
            u32      u = utable[input];
            code	   = (state != 1) ? (input & 0x3fu) | (code << 6) : (0xff >> u) & input;
            state	   = utable[256 + state * 16 + u];
            return state;
        };

        for (u8 *cs0 = cs; *cs0; ++cs0)
            if (!dc(*cs0)) {
                if (fn) fn(code);
                ln++;
            }

        return (state != 0) ? -1 : int(ln);
    }

    static inline int len(uint8_t* cs) {
        int l = decode(cs, null);
        return math::max<int>(0, l);
    }
};

using args = map;

template <typename T>
struct uniques:mx {
    struct M {
        std::unordered_set<T> uset;
    };

    uniques(std::initializer_list<T> l) : uniques() {
        data->uset = l;
    }

    bool set(const T &v) {
        std::pair<typename std::unordered_set<T>::iterator, bool> i = data->uset.insert(v);
        return bool(i.second);
    }

    bool unset(const T &v) {
        size_t count = data->uset.erase(v);
        return count == 1;
    }

    bool contains(const T &v) {
        return data->uset.find(v) != data->uset.end();
    }

    inline bool operator[](const T &v) const {
        return contains(v);
    }

    mx_basic(uniques)
};

template <typename T>
class has_count {
    using one = char;
    struct two { char x[2]; };
    template <typename C> static one test( decltype(C::count) ) ;
    template <typename C> static two test(...);    
public:
    enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename T>
struct states:mx {
    struct fdata {
        type_t type;
        u64    bits;
    };

    /// get the bitmask (1 << value); 0 = 1bit set
    inline static u64 to_flag(i64 v) {
        return (v < 0) ? ((u64(1) << u64(63)) >> u64(-v)) : (u64(1) << u64(v));
    }

    mx_object(states, mx, fdata);

    /// initial states with initial list of values
    inline states(std::initializer_list<T> args) : states() {
        for (auto  v:args) {
            i64 ord = i64(v); /// from an enum type (best to support sign)
            u64 fla = to_flag(ord);
            data->bits |= fla;
        }
    }

    memory *String() const {
        if (!data->bits)
            return memory::symbol("null");

        if constexpr (has_count<T>::value) {
            const size_t sz = 32 * (T::count + 1);
            char format[sz];
            ///
            doubly &symbols = T::symbols();
            ///
            u64    v     = data->bits;
            size_t c_id  = 0;
            size_t c_len = 0;
            bool   first = true;
            ///
            for (memory *sym: symbols.elements<memory*>()) {
                if (v & 1 && c_id == sym->id) {
                    char *name = (char*)sym->origin;
                    if (!first)
                        c_len += snprintf(&format[c_len], sz - c_len, ",");
                    else
                        first  = false;
                    c_len += snprintf(&format[c_len], sz - c_len, "%s", name);   
                }
                v = v >> 1;
                c_id++;
            }
            return memory::cstring(format);
        } else {
            assert(false);
            return null;
        }
    }

    void clear(const T &v) {
        u64 f = to_flag((i64)i32(v));
        data->bits &= ~f;
    }

    void set(const T &v) {
        u64 f = to_flag((i64)i32(v)); /// this was reading u64 from an int, segfaulting
        data->bits |= f;
    }

    template <typename ET>
    inline bool operator[](const ET &varg) const {
        u64 f = 0;
        if constexpr (identical<ET, std::initializer_list<T>>()) {
            for (auto &v:varg)
                f |= to_flag(u32(v));
        } else if constexpr (identical<ET, symbol>()) { /// support states["enum-name"] = true;  this reduces code
            T v = T(varg);
            f = to_flag(i32(v.value));
        } else
            f = to_flag(i32(varg));
        
        return (data->bits & f) == f;
    }

    operator memory*() { return ion::hold(mem); }
};

#ifdef WIN32
#define PATH_MAX 4096
#define getcwd _getcwd
#endif

// just a mere lexical user of cwd
struct dir {
    static str cwd() {
        static char buf[PATH_MAX + 1]; /// larger of POSIX & windows with null char
        getcwd(buf, sizeof(buf));
        return str(buf);
    }
    str prev;
     ///
     dir(str p = null) : prev(p ? cwd() : null) {
        chdir(p.cs());
    }
    ~dir() { if (prev) chdir(prev.cs()); }
};

static void brexit() {
    exit(1);
}

/// use char as base.
struct Path:A {
    inline static std::error_code ec;

    using Fn = lambda<void(Path*)>;

    enums(option, recursion,
         recursion, no_sym_links, no_hidden, use_git_ignores);

    enums(op, none,
         none, deleted, modified, created);

    static Path* cwd();

    fs::path p;
    String* to_string();

    Path(str      s);
    Path(symbol  cs);

    template <typename T> T     read(symbol subpath = null) const;
    template <typename T> bool write(T input) const;

    bool get_modified_date(struct tm *res);
    str  get_modified_string();

    /// utility for knowing if you are trying to go up beyond a relative dir
    /// without any dir analysis is more reduced
    static bool backwards(cstr cs);

    fs::path      *pdata() const;
    str        mime_type();
    str             stem() const;
    bool      remove_all() const;
    bool          remove() const;
    bool       is_hidden() const;
    bool          exists() const;
    bool          is_dir() const;
    bool        make_dir() const;
    Path* remove_filename();
    bool    has_filename() const;
    Path*            link() const;
    bool         is_file() const;
    char             *cs() const;
    str             ext () const;
    str             ext4() const;
    Path*            file() const;
    bool     copy(Path* to) const;
    
    Path* &assert_file();

    /// create an alias; if successful return its location
    Path*  link(Path* alias) const;

    /// operators
    operator        bool()         const;
    operator         str()         const;
    Path*         parent()         const;
    Path*operator / (Path*       s) const;
    Path*operator / (symbol     s) const;
    Path*operator / (const str& s) const;
    Path*relative   (Path*    from) const;
    bool  operator==(Path&      b) const;
    bool  operator!=(Path&      b) const;
    bool  operator==(symbol     b) const;
    bool  operator!=(symbol     b) const;
    operator str();
    operator cstr() const;

    /// methods
    static Path *uid(path b);
    static Path *format(str t, array args);
    int64_t modified_at() const;
    bool read(size_t bs, lambda<void(symbol, size_t)> fn);
    bool append(const array &bytes);
    bool same_as  (path b) const; /// visual studio code should use this
    void resources(array exts, states<option> states, Fn fn);
    array matching(array exts);
};

using path_t = path;

/// these are utc epoch-based
void wait_until(u64 to);
void wait_until(i64 to);
i64 millis();
u64 microseconds();

struct base64 {
    static umap<size_t, size_t> &enc_map() {
        static umap<size_t, size_t> mm;
        static bool init;
        if (!init) {
            init = true;
            /// --------------------------------------------------------
            for (size_t i = 0; i < 26; i++) mm[i]          = size_t('A') + size_t(i);
            for (size_t i = 0; i < 26; i++) mm[26 + i]     = size_t('a') + size_t(i);
            for (size_t i = 0; i < 10; i++) mm[26 * 2 + i] = size_t('0') + size_t(i);
            /// --------------------------------------------------------
            mm[26 * 2 + 10 + 0] = size_t('+');
            mm[26 * 2 + 10 + 1] = size_t('/');
            /// --------------------------------------------------------
        }
        return mm;
    }

    static umap<size_t, size_t> &dec_map() {
        static umap<size_t, size_t> m;
        static bool init;
        if (!init) {
            init = true;
            for (int i = 0; i < 26; i++)
                m['A' + i] = i;
            for (int i = 0; i < 26; i++)
                m['a' + i] = 26 + i;
            for (int i = 0; i < 10; i++)
                m['0' + i] = 26 * 2 + i;
            m['+'] = 26 * 2 + 10 + 0;
            m['/'] = 26 * 2 + 10 + 1;
        }
        return m;
    }

    static str encode(symbol data, size_t len) {
        umap<size_t, size_t> &enc = enc_map();
        size_t p_len = ((len + 2) / 3) * 4;
        str encoded(p_len + 1);

        u8    *e = (u8 *)encoded.data;
        size_t b = 0;
        
        for (size_t  i = 0; i < len; i += 3) {
            *(e++)     = u8(enc[data[i] >> 2]);
            ///
            if (i + 1 <= len) *(e++) = u8(enc[((data[i]     << 4) & 0x3f) | data[i + 1] >> 4]);
            if (i + 2 <= len) *(e++) = u8(enc[((data[i + 1] << 2) & 0x3f) | data[i + 2] >> 6]);
            if (i + 3 <= len) *(e++) = u8(enc[  data[i + 2]       & 0x3f]);
            ///
            b += math::min(size_t(4), len - i + 1);
        }
        for (size_t i = b; i < p_len; i++) {
            *(e++) = '=';
            b++;
        }
        *e = 0;
        encoded.mem->count = b; /// str allocs atleast 1 more than size given above
        return encoded;
    }

    static array decode(symbol b64, size_t b64_len) {
        assert(b64_len % 4 == 0);
        /// --------------------------------------
        umap<size_t, size_t> &dec = dec_map();
        size_t alloc_sz = b64_len / 4 * 3;
        array out(typeof(u8), alloc_sz + 1);
        u8     *o = (u8*)out.data<u8>();
        size_t  n = 0;
        size_t  e = 0;
        /// --------------------------------------
        for (size_t ii = 0; ii < b64_len; ii += 4) {
            size_t a[4], w[4];
            for (int i = 0; i < 4; i++) {
                bool is_e = a[i] == '=';
                char   ch = b64[ii + i];
                a[i]      = ch;
                w[i]      = is_e ? 0 : dec[ch];
                if (a[i] == '=')
                    e++;
            }
            u8  b0 = u8(w[0]) << 2 | u8(w[1]) >> 4;
            u8  b1 = u8(w[1]) << 4 | u8(w[2]) >> 2;
            u8  b2 = u8(w[2]) << 6 | u8(w[3]) >> 0;
            if (e <= 2) o[n++] = b0;
            if (e <= 1) o[n++] = b1;
            if (e <= 0) o[n++] = b2;
        }
        assert(n + e == alloc_sz);
        o[n] = 0;
        out.set_size(n);
        return out;
    }
};

/// var should inherit from M
struct var:mx {
protected:
    void push(mx v);
    mx  &last();

    static mx parse_obj(cstr *start, type_t type);
    static mx parse_arr(cstr *start, type_t type);
    static void skip_alpha(cstr *start);
    static mx parse_value(cstr *start, type_t type, mx field);
    static str parse_numeric(cstr * cursor, bool &floaty);
    static mx parse_quoted(cstr *cursor, type_t type);
    static char ws(cstr *cursor);

    public:

    operator std::string();
    mx *get(str key);
    static mx parse(cstr js, type_t type);
    str stringify() const;

    /// default constructor constructs map
    var();
    var(mx b);
    var(map m);
    var(memory* mem) : mx(mem) { }

    template <typename T>
    var(const T b) : mx(alloc(&b)) { }

    var operator+(const var &b) const {
        type_t at =   mem->data_type();
        type_t bt = b.mem->data_type();
        if (at == typeof(i64)    && bt == typeof(i64))    return    i64((mx&)*this)        + i64((mx&)b);
        if (at == typeof(double) && bt == typeof(i64))    return double(double((mx&)*this) + i64((mx&)b));
        if (at == typeof(i64)    && bt == typeof(double)) return double(   i64((mx&)*this) + double((mx&)b));
        if (at == typeof(double) && bt == typeof(double)) return double(double((mx&)*this) + double((mx&)b));
        if (at == typeof(char)   && bt == typeof(char))   return str(     str((mx&)*this)  +    str((mx&)b));
        assert(false);
        return var();
    }

    var operator-(const var &b) const {
        type_t at =   mem->data_type();
        type_t bt = b.mem->data_type();
        if (at == typeof(i64)    && bt == typeof(i64))    return    i64((mx&)*this)        - i64((mx&)b);
        if (at == typeof(double) && bt == typeof(i64))    return double(double((mx&)*this) - i64((mx&)b));
        if (at == typeof(i64)    && bt == typeof(double)) return double(   i64((mx&)*this) - double((mx&)b));
        if (at == typeof(double) && bt == typeof(double)) return double(double((mx&)*this) - double((mx&)b));
        assert(false);
        return var();
    }

    var operator*(const var &b) const {
        type_t at =   mem->data_type();
        type_t bt = b.mem->data_type();
        if (at == typeof(i64)    && bt == typeof(i64))    return    i64((mx&)*this)        * i64((mx&)b);
        if (at == typeof(double) && bt == typeof(i64))    return double(double((mx&)*this) * i64((mx&)b));
        if (at == typeof(i64)    && bt == typeof(double)) return double(   i64((mx&)*this) * double((mx&)b));
        if (at == typeof(double) && bt == typeof(double)) return double(double((mx&)*this) * double((mx&)b));
        assert(false);
        return var();
    }

    var operator/(const var &b) const {
        type_t at =   mem->data_type();
        type_t bt = b.mem->data_type();
        if (at == typeof(i64)    && bt == typeof(i64))    return    i64((mx&)*this)        / i64((mx&)b);
        if (at == typeof(double) && bt == typeof(i64))    return double(double((mx&)*this) / i64((mx&)b));
        if (at == typeof(i64)    && bt == typeof(double)) return double(   i64((mx&)*this) / double((mx&)b));
        if (at == typeof(double) && bt == typeof(double)) return double(double((mx&)*this) / double((mx&)b));
        assert(false);
        return var();
    }

    var operator&&(const var &b) const {
        if (bool((mx&)*this) && bool((mx&)b))
            return true;
        return false;
    }

    var operator||(const var &b) const {
        if (bool((mx&)*this)) return *this;
        if (bool((mx&)    b)) return b;
        return var();
    }

    var operator^(const var &b) const {
        bool ba = bool((mx&)*this);
        bool bb = bool((mx&)b);
        if ((ba ^ bb) == false) return var();
        if (ba) return *this;
        return b;
    }

    // abstract between array and map based on the type
    template <typename KT>
    var &operator[](KT key) {
        /// if key is numeric, this must be an array
        type_t kt        = typeof(KT);
        type_t data_type = type();

        if constexpr (identical<KT, char*>() || identical<KT, const char*>() || identical<KT, str>()) {
            /// if key is something else just pass the mx to map
            map::mdata *ptr = mx::get<map::mdata>(0);
            assert(ptr);
            mx skey = mx(key);
            return *(var*)&(*ptr)[skey];
        } else if (kt->traits & traits::integral) {    
            assert(data_type == typeof(array) || data_type == typeof(mx)); // array or mx
            mx *ptr = mx::get<mx>(0);
            assert(ptr);
            size_t ix;
                 if (kt == typeof(i32)) ix = size_t(int(key));
            else if (kt == typeof(u32)) ix = size_t(int(key));
            else if (kt == typeof(i64)) ix = size_t(int(key));
            else if (kt == typeof(u64)) ix = size_t(int(key));
            else {
                console.fault("weird integral type given for indexing var: {0}\n", array { mx((symbol)kt->name) });
                ix = 0;
            }
            return *(var*)&ptr[ix];
        } else if (kt == typeof(mx)) {
            map::mdata *ptr = mx::mem->get<map::mdata>(0);
            assert(ptr);
            mx skey = mx(key);
            return *(var*)&(*ptr)[skey];
        }
        console.fault("incorrect indexing type provided to var");
        return *this;
    }

    static mx json(mx i, type_t type);

    memory *string();

    operator str();

    explicit operator cstr();

    operator i64();

    operator real();

    operator bool();

    ldata::literable<field> fields() const { // replaces items
        map iter;
        if (mem->type->traits & traits::map)
            iter = mem->hold();
        return iter.fields();
    }

    template <typename T>
    iterable<T> elements() const { // replaces list
        array iter;
        if (mem->type->traits & traits::array)
            iter = mem->hold();
        return iter.elements<T>();
    }
};

bool         chdir(std::string c);

template <typename T>
bool path::write(T input) const {
    fs::path *pdata = this->pdata();
    if constexpr (is_array<T>()) {
        std::ofstream f(*pdata, std::ios::out | std::ios::binary);
        if (input.len() > 0)
            f.write((symbol)input.data, input.data_type()->base_sz * input.len());
    } else if constexpr (identical<T, var>()) {
        str    v  = input.stringify(); /// needs to escape strings
        std::ofstream f(*pdata, std::ios::out | std::ios::binary);
        if (v.len() > 0)
            f.write((symbol)v.data, v.len());
    } else if constexpr (identical<T,str>()) {
        std::ofstream f(*pdata, std::ios::out | std::ios::binary);
        if (input.len() > 0)
            f.write((symbol)input.cs(), input.len());
    } else {
        console.log("path conversion to {0} is not implemented. surely it wont take long.", { mx(typeof(T)->name) });
        return false;
    }
    return true;
}

/// use-case: any kind of file [Joey]
template <typename T>
T path::read(symbol subpath) const {
    const bool nullable = std::is_constructible<T, std::nullptr_t>::value;
    const bool integral = !has_int_conversion<T>::value || std::is_integral<T>::value;
    
    if (!fs::is_regular_file(*pdata())) {
        if constexpr (nullable)
            return null;
        else if constexpr (integral)
            return 0;
        else
            return T();
    }

    if constexpr (identical<T, array>()) {
        std::ifstream input(data->p, std::ios::binary);
        std::vector<char> buffer(std::istreambuf_iterator<char>(input), { });
        array res(typeof(u8), buffer.size());
        memcpy(res.data<u8>(), (u8*)buffer.data(), buffer.size());
        res.set_size(buffer.size());
    } else {
        std::ifstream fs;
        fs.open(*pdata());
        std::ostringstream sstr;
        sstr << fs.rdbuf();
        fs.close();
        std::string st = sstr.str();

        /// for json, we handle the subpath
        //dir d(subpath);

        if constexpr (identical<T, str>()) {
            return str((cstr )st.c_str(), int(st.length()));
        } else if constexpr (ion::inherits<var, T>()) {
            return var::parse(cstr(st.c_str()), null);
        } else if constexpr (ion::inherits<mx, T>()) {
            /// if its a user-class, we can use var's schema option
            return var::parse(cstr(st.c_str()), typeof(T));
        } else {
            console.fault("not implemented");
            if constexpr (nullable)
                return null;
            else if constexpr (integral)
                return 0;
            else
                return T();
        }
    }
}

}