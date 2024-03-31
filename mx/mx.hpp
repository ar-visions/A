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
//#include <condition_variable>
//#include <future>
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
#include <array>
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
// typedef i64             ssize_t; // ssize_t is typically predefined in sys/types.h
typedef i64                num;
typedef void*              handle_t;
typedef size_t             sz_t;

struct none { };

/// everybody has to do this, because you cant just break anywhere
/// this lets us define where we put these so we can find them later
#define debug_break() do { int test = 0; } while(0);

struct idata;
struct mx;
struct memory;

using raw_t = void*;
using type_t = idata *;

void*     calloc64(size_t count, size_t size);
void        free64(void* ptr);
memory*       hold(memory *mem);
memory*       drop(memory *mem);
memory* mem_symbol(symbol cs, type_t ty, i64 id);
void*   mem_origin(memory *mem);
//memory*    cstring(cstr cs, size_t len, size_t reserve, bool is_constant);
memory*    cstring(cstr cs, size_t len = UINT64_MAX, size_t reserve = 0, bool sym = false);


template <typename T> T *mdata(memory *mem, size_t index);

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


template <typename T, typename = std::void_t<>>
struct has_mix : std::false_type {};

template <typename T>
struct has_mix<T, std::void_t<decltype(std::declval<T>().mix(std::declval<T&>(), 1.0))>> : std::true_type {};


template <typename T, typename = void> struct registered_instance_to_string : false_type { };
template <typename T>                  struct registered_instance_to_string<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().to_string()), memory *>>> : true_type { };


//template <typename> struct is_registered : false_type { };
template <typename> struct is_external   : false_type { };
template <typename> struct is_opaque     : false_type { };
template <typename> struct is_singleton  : false_type { };


template <typename T>
constexpr bool is_mx();

//int snprintf(char *str, size_t size, const char *format, ...);

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

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

/// useful single statement error and print with fprintf stderr formatting, auto new-line?
#define errorf(TXT, ...) do { fprintf(stderr, TXT "\n", __VA_ARGS__); exit(1); } while (0);

constexpr int num_occurances(const char* cs, char c) {
    return cs[0] ? (cs[0] == c) + num_occurances(cs + 1, c) : 0; 
}

#define num_args(...) (ion::num_occurances(#__VA_ARGS__, ',') + 1)
#define str_args(...) (str(#__VA_ARGS__))

#define enums(C,D,...)\
    struct C:ex {\
        enum etype { __VA_ARGS__ };\
        enum etype&    value;\
        inline static const type_t intern_t = typeof(etype);\
        static memory* lookup(symbol sym) { return typeof(C)->lookup(sym); }\
        static memory* lookup(i64     id) { return typeof(C)->lookup(id);  }\
        static doubly<memory*> &symbols() { return typeof(C)->symbols->list; }\
        inline static const int count = num_args(__VA_ARGS__);\
        inline static const str raw   = str_args(__VA_ARGS__);\
        ion::symbol symbol() {\
            memory *mem = typeof(C)->lookup(i64(value));\
            if (!mem) printf("symbol: mem is null for value %d\n", (int)value);\
            assert(mem);\
            return (char*)mem->origin;\
        }\
        str name() { return (char*)symbol(); }\
        struct memory *to_string() { return typeof(C)->lookup(i64(value)); }\
        C(enum etype t = etype::D):ex(initialize(this,             t, (ion::symbol)raw.cs(), typeof(C)), this), value(ref<enum etype>()) { }\
        C(size_t     t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(ref<enum etype>()) { }\
        C(int        t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(ref<enum etype>()) { }\
        C(str sraw):C(ex::convert(sraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
        C(mx  mraw):C(ex::convert(mraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
        C(ion::symbol sym):C(ex::convert(sym, (ion::symbol)C::raw.cs(), (C*)null)) { }\
        C(memory* mem):C(mx(mem)) { }\
        inline  operator etype() { return value; }\
        C&      operator=  (const C b)  { return (C&)assign_mx(*this, b); }\
        bool    operator== (enum etype v) { return value == v; }\
        bool    operator== (ion::symbol v) {\
            if (!mem && !v)\
                return true;\
            memory *m = lookup(v);\
            return (int)m->id == (int)value;\
        }\
        bool    operator!= (enum etype v) { return value != v; }\
        bool    operator>  (C &b)       { return value >  b.value; }\
        bool    operator<  (C &b)       { return value <  b.value; }\
        bool    operator>= (C &b)       { return value >= b.value; }\
        bool    operator<= (C &b)       { return value <= b.value; }\
        explicit operator int()         { return int(value); }\
        explicit operator i64()         { return i64(value); }\
        operator str()         { return symbol(); }\
    };\

#define enums_declare(C,D,W,...)\
    struct W##Wrapper;\
    struct C:ex {\
        enum etype { __VA_ARGS__ };\
        enum etype&    value;\
        inline static const type_t intern_t = typeof(etype);\
        static memory* lookup(symbol sym) { return typeof(C)->lookup(sym); }\
        static memory* lookup(i64     id) { return typeof(C)->lookup(id);  }\
        static doubly<memory*> &symbols() { return typeof(C)->symbols->list; }\
        inline static const int count = num_args(__VA_ARGS__);\
        inline static const str raw   = str_args(__VA_ARGS__);\
        ion::symbol symbol();\
        str name();\
        memory *to_string();\
        C(enum etype t = etype::D);\
        C(size_t     t);\
        C(int        t);\
        C(str sraw);\
        C(mx  mraw);\
        C(ion::symbol sym);\
        C(memory* mem);\
        operator etype();\
        C&      operator=  (const C b);\
        bool    operator== (enum etype v);\
        bool    operator== (ion::symbol v);\
        bool    operator!= (enum etype v);\
        bool    operator>  (C &b);\
        bool    operator<  (C &b);\
        bool    operator>= (C &b);\
        bool    operator<= (C &b);\
        explicit operator int();\
        explicit operator i64();\
        operator str();\
        C(const W##Wrapper &r);\
        W##Wrapper convert();\
    };\

#define enums_define(C, W)\
struct W##Wrapper {\
    W value;\
    W##Wrapper(W v) : value(v) { }\
    operator W() {\
        return value;\
    }\
};\
ion::symbol C::symbol() {\
    memory *mem = typeof(C)->lookup(i64(value));\
    if (!mem) printf("symbol: mem is null for value %d\n", (int)value);\
    assert(mem);\
    return (char*)mem->origin;\
}\
str C::name() { return (char*)symbol(); }\
memory *C::to_string() { return typeof(C)->lookup(i64(value)); }\
C::C(enum etype t)           :ex(initialize(this,             t, (ion::symbol)raw.cs(), typeof(C)), this), value(ref<enum etype>()) { }\
C::C(size_t     t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(ref<enum etype>()) { }\
C::C(int        t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(ref<enum etype>()) { }\
C::C(str sraw):C(ex::convert(sraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
C::C(mx  mraw):C(ex::convert(mraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
C::C(ion::symbol sym):C(ex::convert(sym, (ion::symbol)C::raw.cs(), (C*)null)) { }\
C::C(memory* mem):C(mx(mem)) { }\
C::operator etype() { return value; }\
C&      C::operator=  (const C b)  { return (C&)assign_mx(*this, b); }\
bool    C::operator== (enum etype v) { return value == v; }\
bool    C::operator== (ion::symbol v) {\
    if (!mem && !v)\
        return true;\
    memory *m = lookup(v);\
    return (int)m->id == (int)value;\
}\
bool    C::operator!= (enum etype v) { return value != v; }\
bool    C::operator>  (C &b)       { return value >  b.value; }\
bool    C::operator<  (C &b)       { return value <  b.value; }\
bool    C::operator>= (C &b)       { return value >= b.value; }\
bool    C::operator<= (C &b)       { return value <= b.value; }\
C::operator int()         { return int(value); }\
C::operator i64()         { return i64(value); }\
C::operator str()         { return symbol(); }\
C::C(const W##Wrapper &r):C((enum etype)(int)r.value) { }\
W##Wrapper C::convert() {\
    return W##Wrapper((W)(int)value);\
}\

#ifdef WIN32
void usleep(__int64 usec);
#endif

#define infinite_loop() { for (;;) { usleep(1000); } }

#define typeof(T)   ident::for_type<T>()

#ifdef __cplusplus
#    define decl(x)   decltype(x)
#else
#    define decl(x) __typeof__(x)
#endif

#define type_assert(CODE_ASSERT, TYPE_ASSERT) \
    do {\
        const auto   v = CODE_ASSERT;\
        const type_t t = typeof(decl(v));\
        static_assert(decl(v) != typeof(TYPE));\
        if (t != typeof(TYPE)) {\
            faultf("(type-assert) %s:%d: :: %s\n", __FILE__, __LINE__, xstr_0(TYPE_ASSERT));\
        }\
        if (!bool(v)) {\
            faultf("(assert) %s:%d: :: %s\n", __FILE__, __LINE__, xstr_0(CODE_ASSERT));\
        }\
    } while (0);

#define interops(C) \
    template <typename X>\
    operator X &() {\
        return (X &)mx::data<typename C::intern>();\
    }\


template <typename T, typename = std::void_t<>> struct has_bool_op: false_type {};
template <typename T> struct has_bool_op<T, std::void_t<decltype(static_cast<bool>(std::declval<T&>()))>> : true_type {};
template <typename T> constexpr bool has_bool = has_bool_op<T>::value;

/// primitive to/from string here
memory *_to_string(cstr data);
size_t  _hash(cstr data, size_t count);

template <typename T>
typename std::enable_if<std::is_same<T, double>::value
                     || std::is_same<T, float>::value
                     || std::is_same<T, i64>::value
                     || std::is_same<T, u64>::value
                     || std::is_same<T, i32>::value
                     || std::is_same<T, u32>::value
                     || std::is_same<T, i16>::value
                     || std::is_same<T, u16>::value,
                     T>::type* _to_string(T* v) {
    std::string s = std::to_string(*v);
    memory   *mem = cstring((cstr)s.c_str(), s.length(), 0, false);
    return mem;
}

template <typename T>
typename std::enable_if<std::is_same<T, double>::value
                     || std::is_same<T, float>::value
                     || std::is_same<T, bool>::value
                     || std::is_same<T, i64>::value
                     || std::is_same<T, u64>::value
                     || std::is_same<T, i32>::value
                     || std::is_same<T, u32>::value
                     || std::is_same<T, i16>::value
                     || std::is_same<T, u16>::value,
                     T>::type* _from_string(T* type, cstr data) {
    if constexpr (std::is_same_v<T, bool>) {
        std::string s = std::string(data);
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return new bool(s == "true" || s == "1" || s == "tru" || s == "yes");
    }
    else if constexpr (std::is_floating_point<T>::value)
        return new T(T(std::stod(data)));
    else
        return new T(T(std::stoi(data)));
}

template<typename T, typename = void>
struct has_intern : false_type { };

template<typename T>
struct has_intern<T, std::void_t<typename T::intern>> : true_type { };

//template<typename T, typename = void>
//struct is_lambda : false_type { };
//template<typename T>
//struct is_lambda<T, std::void_t<typename T::fdata>> : true_type { };

#define mx_declare(C, B, D) \
    using parent_class  = B;\
    using context_class = C;\
    static type_t intern_t;\
    C(const D& data);\
    C(memory*  mem);\
    C(D*  data);\
    C(mx o);\
    C();\
    D *data;\
    D  &operator *();\
    D  *operator->();\
    explicit operator D *();\
             operator D &();\
    C       &operator=(const C &b);

#define mx_implement(C, B, D) \
    type_t C::intern_t  = typeof(D);\
    C::C(memory*   mem) : B(mem), data(mdata<D>(mem, 0)) { }\
    C::C(mx          o) : C(o.mem->hold()) { }\
    C::C()              : C(mx::alloc<C>()) { }\
    C::C(D  *data) : C(mx::wrap<D>(data, 1)) { }\
    C::C(const D  &data) : C(mx::alloc<C>((void*)&data)) { }\
    D  &C::operator *() { return *data; }\
    D *C::operator->() { return  data; }\
    C::operator D   *() { return  data; }\
    C::operator D   &() { return *data; }\
    C &C::operator=(const C &b) { mem->type->functions->assign((none*)null, (none*)this, (none*)&b); return *this; }

//
// C(const C& b) : B(b.mem->hold()), data(mx::data<D>()) { }\

#define mx_object(C, B, D) \
    using parent_class   = B;\
    using context_class  = C;\
    using intern         = D;\
    static const inline type_t intern_t  = typeof(D);\
    intern*    data;\
    C(memory*   mem) : B(mem), data(mx::data<D>()) { }\
    C(const C& b)    : C(b.mem->hold()) { }\
    C(intern*  data) : C(mx::wrap <C>(raw_t(data), 1)) { }\
    C(const intern& memb) : B(mx::alloc<C>((void*)&memb)), data(mx::data<D>()) { }\
    C(mx          o) : C(o.mem->hold()) { }\
    C()              : C(mx::alloc<C>()) { }\
    intern    &operator *() { return *data; }\
    intern    *operator->() { return  data; }\
    explicit operator intern *() { return  data; }\
    operator     intern &() { return *data; }\
    C      &operator=(const C &b) {\
        mx::drop();\
        mx::mem = b.mem->hold();\
        data = (intern*)mx::mem->data<intern>(0);\
        return *this;\
    }

#define mx_basic(C) mx_object(C, mx, M)

/// these objects dont contain a data member for access
#define mx_object_0(C, B, D) \
    using parent_class   = B;\
    using context_class  = C;\
    using intern         = D;\
    static const inline type_t intern_t = typeof(D);\
    C(memory*   mem) : B(mem) { }\
    C(mx          o) : C(o.mem->hold()) { }\
    C()              : C(mx::alloc<C>(null, 0, 1)) { }\

template <class T, class Enable = void>
struct is_defined {
    static constexpr bool value = false;
};

template <class T>
struct is_defined<T, std::enable_if_t<(sizeof(T) > 0)>> {
    static constexpr bool value = true;
};

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

template<typename...>
using void_t = void;

template <typename T, typename = void>
struct has_multiply_double : false_type {};

template <typename T>
struct has_multiply_double<T, void_t<decltype(std::declval<T>() * std::declval<double>())>> : true_type {};

template <typename T, typename = void>
struct has_addition : false_type {};

template <typename T>
struct has_addition<T, void_t<decltype(std::declval<T>() + std::declval<T>())>> : std::is_same<decltype(std::declval<T>() + std::declval<T>()), T>::type {};

template <typename T, typename = void>
struct has_multiply : false_type {};

template <typename T>
struct has_multiply<T, void_t<decltype(std::declval<T>() * std::declval<T>())>> : std::is_same<decltype(std::declval<T>() * std::declval<T>()), T>::type {};

template <typename T, typename = void>
struct has_multiply_scalar : false_type {};

template <typename T>
struct has_multiply_scalar<T, void_t<decltype(std::declval<T>() * std::declval<float>())>> : std::is_same<decltype(std::declval<T>() * std::declval<float>()), T>::type {};

template <typename T>
constexpr bool transitionable() {
    return has_multiply_double<T>::value && has_addition<T>::value;
}

static inline void yield() {
#ifdef _WIN32
    SwitchToThread(); // turns out you can. lol.
#else
    sched_yield();
#endif
}

template <typename T> using initial = std::initializer_list<T>;

template <typename T>
struct item {
    struct item *next;
    struct item *prev;
    T            data;

    /// return the wrapped item memory, just 3 doors down.
    static item<T> *container(T& i) {
        return (item<T> *)(
            (symbol)&i - sizeof(struct item*) * 2
        );
    }
};

/// iterator unique for doubly
template <typename T>
struct liter {
    item<T>* cur;
    ///
    liter& operator++  () { cur = cur->next; return *this;          }
    liter& operator--  () { cur = cur->prev; return *this;          }

    T& operator *  ()                  const { return cur->data;     }
       operator T& ()                  const { return cur->data;     }

    inline bool operator==  (const liter& b) const { return cur == b.cur; }
    inline bool operator!=  (const liter& b) const { return cur != b.cur; }
};


/// iterator
/*
template <typename T>
struct iter {
    T      *start;
    size_t  index;

    iter       &operator++()       { index++; return *this; }
    iter       &operator--()       { index--; return *this; }
    T&         operator * () const { return start[index];   }
    bool operator==(iter &b) const { return start == b.start && index == b.index; }
    bool operator!=(iter &b) const { return start != b.start || index != b.index; }
};
*/

template <typename T>
struct iter {
    T*      start;
    size_t  index;

    iter(T* s, size_t i) : start(s), index(i) {}  // Added constructor for initialization

    iter& operator++()       { index++; return *this; }
    iter  operator++(int)    { iter tmp = *this; ++(*this); return tmp; }  // Post-increment
    iter& operator--()       { index--; return *this; }
    iter  operator--(int)    { iter tmp = *this; --(*this); return tmp; }  // Post-decrement
    T&    operator* () const { return start[index]; }

    bool operator==(const iter& other) const { return start == other.start && index == other.index; } // made 'other' const
    bool operator!=(const iter& other) const { return !(*this == other); } // simplified
};


template <typename K, typename V>
struct pair {
    V value;
    K key;
    ///
    inline pair() { }
    inline pair(K k, V v) : value(v), key(k)  { }
};

/// doubly does not require memory* or type
template <typename T>
struct doubly {
    /// might be good not to reference ldata elsewhere
    struct ldata {
        size_t   refs = 1;
        size_t   icount;
        item<T> *ifirst, *ilast;

        ~ldata() {
            item<T>* d = ifirst;
            while   (d) {
                item<T>* dn = d->next;
                delete   d;
                d      = dn;
            }
            icount = 0;
            ifirst = ilast = 0;
        }

        operator  bool() const { return ifirst != null; }
        bool operator!() const { return ifirst == null; }

        T shift() {
            assert(ilast);
            ///
            item<T>    *i = ifirst;
            bool set_last = ilast == ifirst;
            ifirst        = ifirst->next;
            ///
            if (!ifirst) {
                ilast = null;
            } else
                ifirst->prev = 0;
            ///
            icount--;
            T data = i->data;
            delete i;
            return data;
        }

        /// push by value, return its new instance
        T &push(T v) {
            item<T> *plast = ilast;
            ilast = new item<T> { null, ilast, v };
            ///
            (!ifirst) ? 
            ( ifirst      = ilast) : 
            ( plast->next = ilast);
            ///
            icount++;
            return ilast->data;
        }

        /// push and return default instance
        T &push() {
            item<T> *plast = ilast;
                ilast = new item<T> { null, ilast }; /// default-construction on data
            ///
            (!ifirst) ? 
                (ifirst      = ilast) : 
                (  plast->next = ilast);
            ///
            icount++;
            return ilast->data;
        }

        /// 
        item<T> *get(num index) const {
            if (index == icount)
                return ilast;
            else if (index == 0)
                return ifirst;
            item<T> *i;
            if (index < 0) { /// if i is negative, its from the end.
                i = ilast;
                while (++index <  0 && i)
                    i = i->prev;
                assert(index == 0); // (negative-count) length mismatch
            } else { /// otherwise 0 is positive and thats an end of it.
                i = ifirst;
                while (--index >= 0 && i)
                    i = i->next;
                assert(index == -1); // (positive-count) length mismatch
            }
            return i;
        }

        /// no need for insert before AND after when you can give it the count of the list
        item<T> *insert(num before, T &data) {
            item<T> *i;
            if (before == icount) {
                i = new item<T> { null, ilast, data };
                if (ilast)
                    ilast->next = i;
                ilast = i;
                if (icount == 0)
                    ifirst = i;
            } else {
                item<T> *s = get(before);
                i = new item<T> { s, s->prev, data };
                if (s->prev) {
                    s->prev->next = i;
                } else {
                    ifirst = i;
                }
                s->prev = i;
            }
            icount++;
            return i;
        }

        size_t    len() { return icount; }
        size_t length() { return icount; }

        bool remove(item<T> *i) {
            if (i) {
                if (i->next)    i->next->prev = i->prev;
                if (i->prev)    i->prev->next = i->next;
                if (ifirst == i) ifirst   = i->next;
                if (ilast  == i) ilast    = i->prev;
                --icount;
                delete i;
                return true;
            }
            return false;
        }

        bool remove(num index) {
            item<T> *i = get(index);
            return remove(i);
        }

        bool remove(num index, num count) {
            item<T> *i = get(index);
            for (int v = 0; v < count; v++) {
                if (!remove(i))
                    return false;
                i = i->next;
            }
            return count > 0;
        }

        void clear() {
            while (icount)
                remove(-1);
        }

        size_t             count() const { return icount;       }
        T                 &first() const { return ifirst->data; }
        T                  &last() const { return ilast->data;  }
        ///
        void          pop(T *prev = null) { assert(icount); if (prev) *prev = last();  remove(-1);     }
        void        shift(T *prev = null) { assert(icount); if (prev) *prev = first(); remove(int(0)); }
        ///
        T                  pop_v()       { assert(icount); T cp =  last(); remove((num)-1); return cp; }
        T                shift_v()       { assert(icount); T cp = first(); remove((num) 0); return cp; }
        T   &operator[]   (num ix) const { assert(ix >= 0 && ix < num(icount)); return get(ix)->data; }
        liter<T>           begin() const { return { ifirst }; }
        liter<T>             end() const { return { null  }; }
        T   &operator+=    (T   v)       { return push  (v); }
        bool operator-=    (num i)       { return remove(i); }
    } *data;

    doubly(ldata *data) : data(data) {
        data->refs++;
    }
    
    doubly() : doubly(new ldata()) { }
    
    doubly(initial<T> a) : doubly() {
        for (auto &v: a)
            data->push(v);
    }

    operator bool() const { return bool(*data); }
    
    ldata &operator *() { return *data; }
    ldata *operator->() { return  data; }

    inline T   &operator[]   (num ix) const { return (*data)[ix];         }
    inline liter<T>           begin() const { return (*data).begin();     }
	inline liter<T>             end() const { return (*data).end();       }
    inline T   &operator+=    (T   v)       { return (*data) += v;        }
    inline bool operator-=    (num i)       { return (*data) -= i;        }

    size_t len() { return data->count(); }

    inline doubly<T>& operator=(const doubly<T> &b) {
             this -> ~doubly( ); /// destruct this
        new (this)    doubly(b); /// copy construct into this, from b; lets us reset refs
        return *this;
    }

     doubly(const doubly &ref) : doubly(ref.data) {
        if (data)
            data->refs++;
     }

    ~doubly() {
        if (data && --data->refs == 0) {
            delete data;
        }
    }
};

/// a simple hash K -> V impl; used by types so it does not use types itsself
template <typename K, typename V>
struct hmap {
  //using hmap   = ion::hmap <K,V>;
    using pair   = ion::pair <K,V>;
    using bucket = typename doubly<pair>::ldata; /// no reason i should have to put typename. theres no conflict on data.

    struct hmdata {
        int     refs = 1;
        bucket *h_pairs;
        size_t  sz;

        /// i'll give you a million quid, or, This Bucket.
        bucket &operator[](u64 k) {
            if (!h_pairs)
                 h_pairs = (bucket*)calloc64(sz, sizeof(bucket));
            return h_pairs[k];
        }
        
        ~hmdata() {
            for (size_t  i = 0; i < sz; i++) h_pairs[i]. ~ bucket(); // data destructs but does not construct and thats safe to do
            free64(h_pairs);
        }
    };

    hmdata *data;

    hmap(hmdata *data) : data(data) {
        data->refs++; /// a bit different than other cases
    }
    hmap(size_t   sz = 0) : hmap(new hmdata()) { data->sz = sz; }
    hmap(initial<pair> a) : hmap(a.size()) { for (auto &v: a) push(v); }

    hmap& operator=(hmap &a) {
        if (data && --data->refs == 0) {
            delete data;
        }
        data = a.data;
        data->refs++;
        return *this;
    }

    inline pair* shared_lookup(K key); 
    
    V* lookup(K input, u64 *pk = null, bucket **b = null) const;
    V &operator[](K key);

    inline size_t    len() { return data->sz; }
    inline operator bool() { return data->sz > 0; }
};

template <typename>
struct is_hmap : false_type {};

template <typename K, typename V>
struct is_hmap<hmap<K, V>> : true_type {};

template <typename T>
struct is_doubly : false_type {};

template <typename T>
struct is_doubly<doubly<T>> : true_type {};

struct prop;
using prop_map = hmap<ion::symbol, prop*>;

struct symbol_data {
    hmap<u64, memory*> djb2 { 32 };
    hmap<i64, memory*> ids  { 32 };
    doubly<memory*>    list { };
};


static inline void sleep(u64 u) {
    #ifdef _WIN32
        Sleep(u);
    #else
        usleep(useconds_t(u) * 1000);
    #endif
}

struct traits {
    enum bit {
        primitive = 1,
        integral  = 2,
        realistic = 4,
        singleton = 8,
        lambda    = 16,
        array     = 32,
        map       = 64,
        mx        = 128,
        mx_obj    = 256,
        init      = 512,
        mx_enum   = 1024,
        enum_primitive = 2048,
        opaque         = 4096 /// may simply check for sizeof() in completeness check, unless we want complete types as opaque too which i dont see point of
    };
};

///
template <typename T> using func    = std::function<T>;


//template <typename T> using lambda  = std::function<T>;

template <typename K, typename V> using umap = std::unordered_map<K,V>;
namespace fs  = std::filesystem;

template <typename T, typename B>
                      T mix(T a, T b, B f) { return a * (B(1.0) - f) + b * f; }
template <typename T> T radians(T degrees) { return degrees * static_cast<T>(0.01745329251994329576923690768489); }
template <typename T> T degrees(T radians) { return radians * static_cast<T>(57.295779513082320876798154814105); }

template <> struct is_singleton<std::nullptr_t> : true_type { };

template <typename A, typename B, typename = void>
struct has_operator : false_type { };
template <typename A, typename B>
struct has_operator <A, B, decltype((void)(void (A::*)(B))& A::operator())> : true_type { };

template<typename, typename = void> constexpr bool type_complete = false;
template<typename T>                constexpr bool type_complete <T, std::void_t<decltype(sizeof(T))>> = true;

template <typename T, typename = void>
struct has_convert : false_type {};
template <typename T>
struct has_convert<T, std::void_t<decltype(std::declval<T>().convert((memory*)nullptr))>> : true_type {};

template <typename T, typename = void>
struct has_compare : false_type {};
template <typename T>
struct has_compare<T, std::void_t<decltype(std::declval<T>().compare((T &)*(T*)nullptr))>> : true_type {};


template <typename T> struct is_array : false_type {};
template <typename T> struct is_map   : false_type {};

bool chdir(std::string c);

struct ident;

template <typename T, const size_t SZ>
struct buffer {
    T      values[SZ];
    size_t count;
};

struct size:buffer<num, 16> {
    using base = buffer<num, 16>;

    inline size(num        sz = 0) { memset(values, 0, sizeof(values)); values[0] = sz; count = 1; }
    inline size(null_t           ) : size(num(0))  { }
    inline size(size_t         sz) : size(num(sz)) { }
    inline size(i8             sz) : size(num(sz)) { }
    inline size(u8             sz) : size(num(sz)) { }
    inline size(i16            sz) : size(num(sz)) { }
    inline size(u16            sz) : size(num(sz)) { }
    inline size(i32            sz) : size(num(sz)) { }
    inline size(u32            sz) : size(num(sz)) { }
    inline size(initial<num> args) : base() {
        size_t i = 0;
        for (auto &v: args)
            values[i++] = v;
        count = args.size();
    }

    size_t    x() const { return values[0];    }
    size_t    y() const { return values[1];    }
    size_t    z() const { return values[2];    }
    size_t    w() const { return values[3];    }
    num    area() const {
        num v = (count < 1) ? 0 : 1;
        for (size_t i = 0; i < count; i++)
            v *= values[i];
        return v;
    }
    size_t dims() const { return count; }

    void assert_index(const size &b) const {
        assert(count == b.count);
        for (size_t i = 0; i < count; i++)
            assert(b.values[i] >= 0 && values[i] > b.values[i]);
    }

    bool operator==(size_t b) const { return  area() ==  b; }
    bool operator!=(size_t b) const { return  area() !=  b; }
    bool operator==(size &sb) const { return count == sb.count && memcmp(values, sb.values, count * sizeof(num)) == 0; }
    bool operator!=(size &sb) const { return !operator==(sb); }
    
    void   operator++(int)          { values[count - 1] ++; }
    void   operator--(int)          { values[count - 1] --; }
    size  &operator+=(size_t sz)    { values[count - 1] += sz; return *this; }
    size  &operator-=(size_t sz)    { values[count - 1] -= sz; return *this; }

    num &operator[](size_t index) const {
        return (num&)values[index];
    }

    size &zero() { memset(values, 0, sizeof(values)); return *this; }

    template <typename T>
    size_t operator()(T *addr, const size &index) {
        size_t i = index_value(index);
        return addr[i];
    }
    
             operator num() const { return     area();  }
    explicit operator  i8() const { return  i8(area()); }
    explicit operator  u8() const { return  u8(area()); }
    explicit operator i16() const { return i16(area()); }
    explicit operator u16() const { return u16(area()); }
    explicit operator i32() const { return i32(area()); }
    explicit operator u32() const { return u32(area()); }
  //explicit operator i64() const { return i64(area()); }
    explicit operator u64() const { return u64(area()); }
    explicit operator r32() const { return r32(area()); }
    explicit operator r64() const { return r64(area()); }

    inline num &operator[](num i) { return values[i]; }

    size &operator=(i8   i) { *this = size(i); return *this; }
    size &operator=(u8   i) { *this = size(i); return *this; }
    size &operator=(i16  i) { *this = size(i); return *this; }
    size &operator=(u16  i) { *this = size(i); return *this; }
    size &operator=(i32  i) { *this = size(i); return *this; }
    size &operator=(u32  i) { *this = size(i); return *this; }
    size &operator=(i64  i) { *this = size(i); return *this; }
    size &operator=(u64  i) { *this = size(i64(i)); return *this; }

    size &operator=(const size b);

    size_t index_value(const size &index) const {
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
};

template <typename T>
memory *talloc(size_t count = 1, size_t reserve = 0);

struct hash { u64 value; hash(u64 v) : value(v) { } operator u64() { return value; } };

template <typename T>
u64 hash_value(T &key);

template <typename T>
u64 hash_index(T &key, size_t mod);

/// a field can be single param, value only resorting and that reduces code in many cases
/// to remove pair is a better idea if we want to reduce the template arg confusion away
template <typename V, typename K=mx>
struct field:pair<K, V> {
    inline field()         : pair<K,V>()     { }
    inline field(K k, V v) : pair<K,V>(k, v) { }
    operator V &() { return pair<K, V>::value; }
};

/// string with precision of float/double
template <typename T> std::string string_from_real(T a_value, int n = 6) {
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

struct context_bind {
    type_t        ctx;
    type_t        data;
    size_t        base_sz;
    size_t        offset;
};

struct alloc_schema {
    size_t        bind_count;  /// bind-count
    size_t        total_bytes; /// total-allocation-size
    context_bind *composition; /// vector-of-pairs (context and its data struct; better to store the data along with it!)
    context_bind *bind; // this is a pointer to the top bind
};

struct str;
/// mx-mock-type
struct ident {
    memory *mem;

    template <typename T>
    static idata* for_type();

    ident(memory* mem) : mem(mem) { }
};

template <typename T>
using fun = std::function<T>;

/// these should be vectorized but its not worth it for now
template <typename T> using   SetMemoryFn =          fun<void(T*, memory*)>;  /// a (inst), b (memory)
template <typename T> using         AddFn =          fun<void(const T&, const T&, T&)>;
template <typename T> using         MulFn =          fun<void(const T&, const T&, T&)>;
template <typename T> using         DivFn =          fun<void(const T&, const T&, T&)>;
template <typename T> using        DivSFn =          fun<void(const T&, const float, T&)>;
template <typename T> using        MulSFn =          fun<void(const T&, const float, T&)>;
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
template <typename T> using    SetValueFn =          fun<void(T*, memory*, memory*)>;  /// dst, src

using PushFn = void(*)(void*, void*); /// array<T>* dst, T* src
//using SetValueFn = void(*)(void*, memory*, memory*); /// map origin, key, value

template <typename T>
struct ops {
        //FreeFn<T> free;
    SetMemoryFn<T> set_memory;
          AddFn<T> add;
          MulFn<T> mul;
         MulSFn<T> mul_scalar;
      CompareFn<T> compare;
      BooleanFn<T> boolean;
          MixFn<T> mix;
     ToStringFn<T> to_string;
   FromStringFn<T> from_string;
         CopyFn<T> copy;
     DestructFn<T> destruct;
    ConstructFn<T> construct;
         InitFn<T> init;
       VAllocFn<T> valloc;
          NewFn<T> alloc_new;
          DelFn<T> del;
       AssignFn<T> assign;
         HashFn<T> hash;
         PushFn    push; /// only for array<T>; we needed a way to allocate array<T> and at runtime push with pointers
         SetValueFn<T> set_value;
         ProcessFn<T> process;

         void     *meta;
};

struct symbol_data;

template<typename T>
struct array;
struct str;

using GenericLambda = std::function<mx(void*, array<str>&)>;

///
struct idata {
    void            *src;     /// the source.
    cstr             name;
    size_t           base_sz; /// a types base sz is without regards to pointer state
    size_t           traits;
    bool             pointer; /// allocate enough space for a pointer to be stored
    doubly<prop>    *meta;    /// doubly<prop>
    prop_map        *meta_map; // prop_map
    alloc_schema    *schema;  /// primitives dont need this one -- used by array and map for type aware contents, also any mx-derrived object containing data will populate this polymorphically
    ops<none>       *functions;
    symbol_data     *symbols;
    memory          *singleton;
    idata           *ref; /// if you are given a primitive enum, you can find the schema and symbols on ref (see: to_string)
    idata           *parent; 
    bool             secondary_init; /// used by enums but flag can be used elsewhere.  could use 'flags' too
    GenericLambda   *generic_lambda;

    size_t size();
    memory *lookup(symbol sym);
    memory *lookup(i64 id);

    idata *meta_lookup() {
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

#undef min
#undef max

struct math {
    template <typename T>
    static inline T min (T a, T b)                             { return a <= b ? a : b; }
    template <typename T> static inline T max  (T a, T b)      { return a >= b ? a : b; }
    template <typename T> static inline T clamp(T v, T m, T x) { return v >= x ? x : x < m ? m : v; }
    template <typename T> static inline T round(T v)           { return std::round(v);  }
};

memory *mem_symbol(symbol cs, type_t ty = typeof(char), i64 id = 0);

static inline u64 djb2(cstr str) {
    u64     h = 5381;
    u64     v;
    while ((v = *str++))
            h = h * 33 + v;
    return  h;
}

template <typename T>
static inline u64 djb2(T *str, size_t count) {
    u64     h = 5381;
    for (size_t i = 0; i < count; i++)
        h = h * 33 + u64(str[i]);
    return  h;
}

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

using properties = doubly<prop>;

template <typename T> using MetaFn = properties(*)(T*);

/// must cache by cstr, and id; ideally support any sort of id range
/// symbols 
using symbol_djb2  = hmap<u64, memory*>;
using symbol_ids   = hmap<i64, memory*>;
using     prop_map = hmap<symbol, prop*>;

struct context_bind;



/// type-api-check
/// typeof usage need not apply registration, registration is about ops on the object not its identification; that only requires a name)
template <typename T, typename = void> struct registered             : false_type { };

/// granular-checks
template <typename T, typename = void> struct registered_assign      : false_type { };
template <typename T, typename = void> struct registered_compare     : false_type { };
template <typename T, typename = void> struct registered_bool        : false_type { };
template <typename T, typename = void> struct registered_copy        : false_type { };
template <typename T, typename = void> struct registered_to_string   : false_type { };
template <typename T, typename = void> struct registered_from_string : false_type { };
template <typename T, typename = void> struct registered_init        : false_type { };
template <typename T, typename = void> struct registered_destruct    : false_type { };
template <typename T, typename = void> struct registered_meta        : false_type { };
template <typename T, typename = void> struct registered_hash        : false_type { };
template <typename T, typename = void> struct registered_construct   : false_type { };

template <typename T, typename = void> struct external_assign      : false_type { };
template <typename T, typename = void> struct external_compare     : false_type { };
template <typename T, typename = void> struct external_bool        : false_type { };
template <typename T, typename = void> struct external_copy        : false_type { };
template <typename T, typename = void> struct external_to_string   : false_type { };
template <typename T, typename = void> struct external_from_string : false_type { };
template <typename T, typename = void> struct external_init        : false_type { };
template <typename T, typename = void> struct external_destruct    : false_type { };
template <typename T, typename = void> struct external_meta        : false_type { };
template <typename T, typename = void> struct external_hash        : false_type { };
template <typename T, typename = void> struct external_construct   : false_type { };

template <typename T, typename = void> struct registered_instance_meta : false_type { };


/// we need two sets because data types can be registered in templates.  you dont want to hard code all of the types you use
/// you also dont want to limit yourself to your own data types, so we have the idea of external

template <typename T> struct registered            <T, std::enable_if_t<std::is_same_v<decltype(T::_new        (std::declval<T*>  (), std::declval<T*>  ())), T*>>>       : true_type { };
template <typename T> struct registered_assign     <T, std::enable_if_t<std::is_same_v<decltype(T::_assign     (std::declval<T*>  (), std::declval<T*>  (), std::declval<T*>())), void>>> : true_type { };
template <typename T> struct registered_compare    <T, std::enable_if_t<std::is_same_v<decltype(T::_compare    (std::declval<T*>  (), std::declval<T*>  (), std::declval<T*>())), int>>>  : true_type { };
template <typename T> struct registered_bool       <T, std::enable_if_t<std::is_same_v<decltype(T::_boolean    (std::declval<T*>  (), std::declval<T*>  ())), bool>>>                     : true_type { };
template <typename T> struct registered_copy       <T, std::enable_if_t<std::is_same_v<decltype(T::_copy       (std::declval<T*>  (), std::declval<T*>  (),  std::declval<T*>(), size_t(0))), void>>> : true_type { };
template <typename T> struct registered_construct  <T, std::enable_if_t<std::is_same_v<decltype(T::_construct  (std::declval<T*>  (), std::declval<T*>  (), size_t(0))), void>>>          : true_type { };

/// this does not need the double argument because the user-implemented function does not have the prototype
template <typename T> struct registered_to_string  <T, std::enable_if_t<std::is_same_v<decltype(T::_to_string  (std::declval<T*>  ())), memory*>>> : true_type { };

template <typename T> struct registered_from_string<T, std::enable_if_t<std::is_same_v<decltype(T::_from_string(std::declval<T*>  (), std::declval<cstr>())), T*>>>                       : true_type { };
template <typename T> struct registered_init       <T, std::enable_if_t<std::is_same_v<decltype(T::_init       (std::declval<T*>  (), std::declval<T*>  ())), void>>>                     : true_type { };
template <typename T> struct registered_destruct   <T, std::enable_if_t<std::is_same_v<decltype(T::_destruct   (std::declval<T*>  (), std::declval<T*>  ())), void>>>                     : true_type { };
template <typename T> struct registered_meta       <T, std::enable_if_t<std::is_same_v<decltype(T::meta        (std::declval<T*>  (), std::declval<T*>  ())), doubly<prop>>>>             : true_type { };
template <typename T> struct registered_hash       <T, std::enable_if_t<std::is_same_v<decltype(T::hash        (std::declval<T*>  (), size_t(0))), size_t>>>        : true_type { };

/// externals need two args (first unused); its so we are explicit about its definition
template <typename T> struct external_assign       <T, std::enable_if_t<std::is_same_v<decltype(_assign        (std::declval<T*>(), std::declval<T*>(), std::declval<T*>())), void>>> : true_type { };
template <typename T> struct external_compare      <T, std::enable_if_t<std::is_same_v<decltype(_compare       (std::declval<T*>(), std::declval<T*>(), std::declval<T*>())), int>>>  : true_type { };
template <typename T> struct external_bool         <T, std::enable_if_t<std::is_same_v<decltype(_boolean       (std::declval<T*>(), std::declval<T*>())), bool>>>                     : true_type { };
template <typename T> struct external_copy         <T, std::enable_if_t<std::is_same_v<decltype(_copy          (std::declval<T*>(), std::declval<T*>(), std::declval<T*>(), size_t(0))), void>>> : true_type { };
template <typename T> struct external_construct    <T, std::enable_if_t<std::is_same_v<decltype(_construct     (std::declval<T*>(), std::declval<T*>(), size_t(0))), void>>>          : true_type { };

/// user must implement this one (see mx.cpp:_to_string of char*)
template <typename T> struct external_to_string    <T, std::enable_if_t<std::is_same_v<decltype(_to_string     (std::declval<T*>())), memory*>>> : true_type { };

template <typename T> struct external_from_string  <T, std::enable_if_t<std::is_same_v<decltype(_from_string   (std::declval<T*>(), std::declval<cstr>())), T*>>> : true_type { };
template <typename T> struct external_init         <T, std::enable_if_t<std::is_same_v<decltype(_init          (std::declval<T*>(), std::declval<T*>())), void>>>                     : true_type { };
template <typename T> struct external_destruct     <T, std::enable_if_t<std::is_same_v<decltype(_destruct      (std::declval<T*>(), std::declval<T*>())), void>>>                     : true_type { };
template <typename T> struct external_meta         <T, std::enable_if_t<std::is_same_v<decltype(_meta          (std::declval<T*>(), std::declval<T*>())), doubly<prop>>>>             : true_type { };
template <typename T> struct external_hash         <T, std::enable_if_t<std::is_same_v<decltype(_hash          (std::declval<T*>(), size_t(0))), size_t>>>  : true_type { };

template <typename T>
struct registered_instance_meta<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().meta()), doubly<prop>>>> : true_type { };


template <typename T>
static ops<T> *ftable();

void *mem_origin(memory *mem);

template <typename T>
T* mdata(raw_t origin, size_t index, type_t ctx);

i64 integer_value(memory *mem);

template <typename T>
T real_value(memory *mem) {
    cstr c = mdata<char>(mem, 0);
    while (isalpha(char(*c)))
        c++;
    return T(strtod(c, null));
}

struct util {
    static cstr copy(symbol cs) {
        size_t   sz = strlen(cs) + 1;
        cstr result = cstr(malloc(sz));
        memcpy(result, cs, sz - 1);
        result[sz - 1] = 0;
        return result;
    }
};

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
                bind.data    = T::intern_t;
                if (!bind.data) {
                    /// we avoid doing this on array<T> because that breaks schema population;
                    /// without having intern_t it does not work on gcc, the types are seen as opaque even when its initialized in module
                    bind.data = identical<typename T::intern, none>() ? null : typeof(typename T::intern);
                }
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

/// scan schema info from struct from design-time info, then populate and sum up the total bytes along with the primary binding
template <typename T>
void schema_populate(idata *type, T *p) {
    type->schema         = (alloc_schema*)calloc64(1, sizeof(alloc_schema));
    alloc_schema &schema = *type->schema;
    schema.bind_count    = schema_info(&schema, 0, (T*)null, (T*)null, type);
    schema.composition   = (context_bind*)calloc64(schema.bind_count, sizeof(context_bind));
    schema_info(&schema, 0, (T*)null, (T*)null, type);
    size_t offset = 0;
    for (size_t i = 0; i < schema.bind_count; i++) {
        context_bind &bind = schema.composition[i];
        bind.offset        = offset;
        offset            += bind.base_sz;
    }
    schema.total_bytes = offset;
    schema.bind = &schema.composition[schema.bind_count - 1];
}

template <typename T>
u64 hash_value(T &key) {
    if constexpr (is_primitive<T>()) {
        return u64(key);
    } else if constexpr (identical<T, cstr>() || identical<T, symbol>()) {
        return djb2(cstr(key));
    } else if constexpr (is_convertible<T, hash>()) {
        return hash(key);
    } else if constexpr (ion::inherits<mx, T>() || is_mx<T>()) {
        ops<T> *fn = (ops<T>*)key.mx::mem->type->functions;
        return fn->hash(&key, key.mem->count);
    } else {
        return 0;
    }
}

template <typename T>
u64 hash_index(T &key, size_t mod) {
    return hash_value(key) % mod;
}

constexpr bool is_debug() {
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
}

struct prop;

template <typename T>
inline bool vequals(T* b, size_t c, T v) {
    for (size_t i = 0; i < c; i++)
        if (b[i] != v)
            return false;
    return true;
}

template <typename T>
struct has_init {
    template <typename U, void (U::*)()> struct SFINAE {};
    template <typename U> static char test(SFINAE<U, &U::init>*);
    template <typename U> static int test(...);
    static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

struct attachment {
    const char    *id;
    void          *data;
    func<void()>   release;
};

using mutex = std::mutex;

struct memory {
    enum attr { constant = 1 };
    size_t              refs;
    u64                 attrs;
    type_t              com; /// if not null, this is the com type
    type_t              type;
    size_t              count, reserve;
    size               *shape; // if null, then, its just 1 dim, of count.  otherwise this is the 'shape' of the data and strides through according
    doubly<attachment> *atts;
    size_t              id;
    bool                managed; /// origin is allocated by us
    raw_t               origin;

    static memory *raw_alloc(type_t type, size_t sz, size_t count, size_t res);
    static int raw_alloc_count();

    static memory *    alloc(type_t type, size_t count, size_t reserve, raw_t src);
           void   *  realloc(size_t res,  bool fill_default);

    /// destruct data and set count to 0
    void clear();

    void drop();
    attachment *attach(symbol id, void *data, func<void()> release);
    attachment *find_attachment(symbol id);

    static inline const size_t autolen = UINT64_MAX;

    static inline memory *wrap(type_t type, void *origin, size_t count = 1, bool managed = true) {
        memory*     mem = raw_alloc(type, sizeof(idata), count, count);
        mem->origin     = origin;
        mem->managed    = managed;
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
    memory *hold();
    memory *weak();
    
    /// now it has a schema with types to each data structure, their offset in allocation, and the total allocation bytes
    /// also supports the primative store, non-mx
    template <typename T>
    T *data(size_t index) const;

    raw_t typed_data(type_t data_type, size_t index) const;

    template <typename T>
    T &ref() const { return *data<T>(0); }
};

template <typename T>
memory *talloc(size_t count, size_t reserve) {
    return memory::alloc(typeof(T), count, reserve, (T*)null);
}

template <typename T>
class has_string {
    typedef char one;
    struct two { char x[2]; };
    template <typename C> static one test( decltype(&C::string) ) ;
    template <typename C> static two test(...);    
public:
    enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

i64 integer_value(memory *mem);

struct str;

template <typename T>
T *defaults() {
    static T def_instance;
    return  &def_instance;
}

template <typename T> T* mdata(memory *mem, size_t index) { return mem ? mem->data<T>(index) : null; }

template <typename T>
inline T &assign_mx(T &a, const T &b) {
    typeof(T)->functions->assign((none*)null, (none*)&a, (none*)&b);
    return a;
}

using fn_t = func<void()>;

struct rand {
    struct seq {
        enum seeding {
            Machine,
            Seeded
        };
        inline seq(i64 seed, seq::seeding t = Seeded) {
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

struct size;

template<typename, typename = void>
struct has_process : std::false_type {};

template<typename T>
struct has_process<T, std::void_t<decltype(std::declval<T>().process(std::declval<memory*>()))>> : std::true_type {};

struct mx {
    memory *mem = null;
    using parent_class  = none;
    using context_class = none;
    using intern        = none;
    static const inline type_t intern_t = null;

    void *realloc(size_t reserve, bool fill_default);

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
        memory *mem = wrap((void*)m, count, (T*)null);
        mem->managed = false;
        return mem;
    }

    /// T is context, not always the data type.  sometimes it is for simple cases, but not in context -> data 
    template <typename T>
    static inline memory *alloc(void *cp, size_t count = 1, size_t reserve = 1, T *ph = null) {
        return memory::alloc(typeof(T), count, reserve, raw_t(cp));
    }

    template <typename T>
    static inline memory *alloc(T *cp = null) {
        return memory::alloc(typeof(T), 1, 1, raw_t(cp));
    }

    template <typename DP>
    inline mx(DP **dptr) {
        mx::alloc((DP*)null);
        *dptr = (DP*)mem->origin;
    }

    mx(u8 *bytes, size_t len) : mem(memory::alloc(typeof(u8), len, len, (void*)bytes)) { }

    operator std::string() {
        return std::string((symbol)mem->origin);
    }
    
    ///
    inline mx(std::string s) : mem(memory:: string(s)) { }

    template <typename T>
    static memory *import(T *addr, size_t count, size_t reserve, bool managed) {
        memory*     mem = alloc<T>(count, reserve);
        mem->managed    = managed;
        mem->origin     = addr;
        return mem;
    }

    /// must move lambda code away from lambda table, and into constructor code gen
    template <typename T>
    memory *copy(T *src, size_t count, size_t reserve) {
        memory*     mem = (memory*)calloc64(1, sizeof(memory));
        mem->count      = count;
        mem->reserve    = reserve; //math::max(count, reserve);
        mem->refs       = 1;
        mem->type       = typeof(T);
        mem->managed    = true;
        mem->origin     = (T*)calloc64(math::max(count, reserve), sizeof(T));
        ///
        if constexpr (is_primitive<T>()) {
            memcpy(mem->origin, src, sizeof(T) * count);
        } else {
            for (size_t i = 0; i < count; i++)
                new (&((T*)mem->origin)[i]) T(&src[i]);
        }
        return mem;
    }

    inline memory *hold() const { return mem->hold(); }
    inline memory *weak() const { return mem->weak(); }

    template <typename T>
    inline T use() const {
        return T(mem->hold());
    }

    inline size  *shape() const { return mem->shape;  }
    inline void    drop() const {
        if (mem)
            mem->drop();
    }


    inline attachment *find_attachment(symbol id) {
        return mem->find_attachment(id);
    }

    inline attachment *attach(symbol id, void *data, func<void()> release) {
        return mem->attach(id, data, release);
    }

    bool is_const() const { return mem->attrs & memory::constant; }

    prop *lookup(cstr cs) const { return lookup(mx(mem_symbol(cs))); }

    prop *lookup(mx key) const {
        if (key.is_const() && mem->type->schema) {
            doubly<prop> *meta = (doubly<prop> *)mem->type->schema->bind->data->meta;
            if (meta)
                for (prop &p: *meta)
                    if (key.mem == p.key) 
                        return &p;
        }
        return null;
    }

    template <typename T>
    inline T *get(size_t index) const { return mem->data<T>(index); }

    template <typename T>
    inline T *origin() const { return (T*)mem->origin; }

    void set_size(sz_t sz) {
        assert(mem->reserve >= sz);
        mem->count = sz;
    }

    /// gets a wrapped mx value; supports primitive/struct/context
    mx get_meta(cstr cs) const { return get_meta(mx(mem_symbol(cs))); }
    mx get_meta(mx  key) const {
        prop *p = lookup(key);
        assert(p);
        
        void *src = p->member_pointer(mem->origin);
        assert(src);

        void *dst = p->type->functions->alloc_new();
        bool is_mx = p->type->traits & traits::mx_obj;
        if (!is_mx)
            p->type->functions->assign(null, (none*)dst, (none*)src); /// copy primitives and structs
        else
            p->type->functions->set_memory((none*)dst, ((mx*)src)->mem); /// quick assignment for mx

        return memory::wrap(p->type, dst, 1);
    }

    void set_meta(cstr cs, mx value) { set_meta(mx(mem_symbol(cs)), value); }
    void set_meta(mx key,  mx value) {
        prop *p = lookup(key);
        assert(p);

        /// this one shouldnt perform any type conversion yet
        assert((p->type == value.type() || (p->type->traits & traits::mx_enum)) || 
               (p->type->schema && p->type->schema->bind->data == value.type()));
        
        /// property must exist
        void *src = p->member_pointer(mem->origin); //
        assert(src);

        bool is_mx = p->type->traits & traits::mx_obj;
        if (!is_mx)
            p->type->functions->assign(null, (none*)src, (none*)value.mem->origin); /// copy primitives and structs
        else
            p->type->functions->set_memory((none*)src, value.mem); /// quick assignment for mx
    }

    inline ~mx() { if (mem) mem->drop(); }
    
    /// interop with shared; needs just base type functionality for lambda
    inline mx(null_t = null): mem(alloc<null_t>()) { }
    inline mx(memory *mem)    : mem(mem->hold()) { }
    inline mx(symbol ccs, type_t type = typeof(char)) : mx(mem_symbol(ccs, type)) { }
    
    mx(  cstr  cs, type_t type = typeof(char)) : mx(memory::stringify(cs, memory::autolen, 0, false, type)) { }
    
    inline mx(const mx     & b) :  mx( b.mem ?  b.mem->hold() : null) { }

    //template <typename T>
    //mx(T& ref, bool cp1) : mx(memory::alloc(typeof(T), 1, 0, cp1 ? &ref : (T*)null), ctx) { }

    template <typename T> inline T *data() const { return  mem->data<T>(0); }
    template <typename T> inline T &ref () const { return *mem->data<T>(0); }
    

    inline mx(bool   v) : mem(copy(&v, 1, 1)) { }
    inline mx(u8     v) : mem(copy(&v, 1, 1)) { }
    inline mx(i8     v) : mem(copy(&v, 1, 1)) { }
    inline mx(u16    v) : mem(copy(&v, 1, 1)) { }
    inline mx(i16    v) : mem(copy(&v, 1, 1)) { }
    inline mx(u32    v) : mem(copy(&v, 1, 1)) { }
    inline mx(i32    v) : mem(copy(&v, 1, 1)) { }
    inline mx(u64    v) : mem(copy(&v, 1, 1)) { }
    inline mx(i64    v) : mem(copy(&v, 1, 1)) { }
    inline mx(r32    v) : mem(copy(&v, 1, 1)) { }
    inline mx(r64    v) : mem(copy(&v, 1, 1)) { }

    /// construct element of type with optional copy
    mx(type_t type, void *src = null) : mem(memory::alloc(type, 1, 0, src)) { }

    template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    inline mx(E v) : mem(alloc(&v)) { }

    inline size_t   byte_len() const {
        type_t type = mem->type;
        size_t t    = (type->schema && type->schema->total_bytes) ? type->schema->total_bytes : type->base_sz;
        return count() * t;
    }

    memory    *copy(size_t res = 0) const { return mem->copy((res == 0 && mem->type == typeof(char)) ? (mem->count + 1) : res); }
    memory *quantum()               const { return (mem->refs == 1) ? mem : mem->copy(); }

    bool is_string() const {
        return mem->type == typeof(char) || (mem->type->schema && mem->type->schema->bind->data == typeof(char));
    }

    void set(memory *m) {
        if (m != mem) {
            if (mem) mem->drop();
            mem = m ?  m->hold() : null;
        }
    }

    template <typename T>
    T &set(T v) {
        memory *nm = alloc(v);
        set(nm);
        return ref<T>();
    }

    size_t count() const { return mem ? mem->count : 0;    }
    type_t  type() const { return mem ? mem->type  : null; }

    memory  *to_string() const;

    inline bool operator==(mx &b) const {
        if (mem == b.mem)
            return true;
    
        if (mem && type() == b.type() && mem->count == b.mem->count) {
            type_t ty = mem->type;
            size_t cn = mem->count;
            if (ty->traits & traits::primitive)
                return memcmp(mem->origin, b.mem->origin, ty->base_sz * cn) == 0;
            else if (ty->functions && ty->functions->compare)
                assert(false);
            //    return ty->functions->compare(raw_t(0), mem->origin, b.mem->origin); /// must pass this in; dont change to memory* type, deprecate those
        }
        return false;
    }

    inline bool operator!=(mx &b)    const { return !operator==(b); }
    
    inline bool operator==(symbol b) const {
        if (mem && mem->type == typeof(char)) {
            if (mem->attrs & memory::attr::constant) {
                return mem == mem_symbol(b);
            } else {
                return strcmp(b, mem->data<char>(0)) == 0;
            }
        }
        return false;
    }
    inline bool operator!=(symbol b) const { return !operator==(b); }

    mx &operator=(const mx &b) {
        mx &a = *this;
        if (a.mem != b.mem) {
            if (b.mem) b.mem->hold();
            if (a.mem) a.mem->drop();
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
            if (ty->schema && ty->schema->bind->data->functions->boolean) {
                BooleanFn<none> data = ty->schema->bind->data->functions->boolean;
                return mem->origin && data((none*)null, (none*)mem->origin);
            }
            
            return mem->count > 0;
        } else
            return false;
    }
    
    inline bool operator!() const { return !(operator bool()); }

    ///
    explicit operator   i8()      { return mem->ref<i8>();  }
    explicit operator   u8()      { return mem->ref<u8>();  }
    explicit operator  i16()      { return mem->ref<i16>(); }
    explicit operator  u16()      { return mem->ref<u16>(); }
    explicit operator  i32()      { return mem->ref<i32>(); }
    explicit operator  u32()      { return mem->ref<u32>(); }
    explicit operator  i64()      { return mem->ref<i64>(); }
    explicit operator  u64()      { return mem->ref<u64>(); }
    explicit operator  r32()      { return mem->ref<r32>(); }
    explicit operator  r64()      { return mem->ref<r64>(); }
    explicit operator  memory*()  { return mem->hold(); } /// trace its uses
    explicit operator  symbol()   { return (ion::symbol)mem->origin; }
};

template <typename T>
constexpr bool is_mx() {
    return identical<T, mx>();
}

template<typename T>
struct lambda_traits : lambda_traits<decltype(&T::operator())> {};

template<typename C, typename R, typename... Args>
struct lambda_traits<R(C::*)(Args...) const> {
    using return_type = R;
    using arg_types = std::tuple<Args...>;
};

template <typename T>
struct lambda;

/// lambda needs to be based on a LGeneric or something?

template <typename R, typename... Args>
struct lambda<R(Args...)>:mx {
    using fdata = std::function<R(Args...)>;
    
    /// just so we can set it in construction
    struct container {
        fdata *fn;
        ~container() {
            delete fn;
        }
    };
    
    mx_object(lambda, mx, container);
    
    template <typename F>
    lambda(F&& fn);

    template <typename CL, typename F>
    lambda(CL* cl, F fn);

    R operator()(Args... args) const {
        return (*data->fn)(std::forward<Args>(args)...);
    }
    
    operator bool() const {
        return data && data->fn && *data->fn;
    }
};

template <typename T> struct is_lambda<lambda<T>> : true_type  { };

template <typename R, typename... Args>
template <typename F>
lambda<R(Args...)>::lambda(F&& fn) : mx() {
    if constexpr (ion::inherits<mx, F>() || is_lambda<std::remove_reference_t<F>>::value) {
        mx::mem = fn.mem->hold();
        data    = (container*)mem->origin;
    } else {
        if constexpr (std::is_invocable_r_v<R, F, Args...>) {
            mx::mem  = mx::alloc<lambda>();
            data     = (container*)mem->origin;
            data->fn = new fdata(std::forward<F>(fn));
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
    constexpr size_t n_args = std::tuple_size_v<args_t>;
    //if constexpr (n_args == 1) { /// remove test.
        //data->fn = new fdata(std::bind(cl, fn, std::placeholders::_1));
    data->fn = new fdata([=](Args... args) { return (cl->*fn)(std::forward<Args>(args)...); });
    //}
}

mx call(mx lambda, array<str> args);

template <typename T>
inline void vset(T *data, u8 bv, size_t c) {
    memset((void*)data, int(bv), sizeof(T) * c);
}

template <typename T>
inline void vset(T *data, T v, size_t c) {
    for (size_t i = 0; i < c; i++)
        data[i] = v;
}

template<typename, typename = void>
struct has_push : std::false_type {};

template<typename T>
struct has_push<T, std::void_t<decltype(T::pushv(std::declval<T*>(), std::declval<memory*>()))>> : std::true_type {};

template <typename T>
struct array:mx {
protected:
    size_t alloc_size() const {
        size_t usz = size_t(mem->count);
        size_t ual = size_t(mem->reserve);
        return ual == 0 ? usz : ual;
    }

public:
    static inline type_t element_type() { return typeof(T); }

    //mx_object(array, mx, T);
    using parent_class   = mx;
    using context_class  = array;
    using intern         = T;
    //inline static const type_t intern_t = typeof(T);
    T*    data;

    static void pushv(array<T> *a, memory *m_item) {
        if constexpr (is_convertible<memory*, T>()) {
            T item(m_item->hold());
            a->push(item);
        } else {
            T &item = *(T*)m_item->origin;
            a->push(item);
        }
    }

    static array<T> read_file(symbol filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("failed to open file!");

        size_t byte_sz = (size_t) file.tellg();
        array<T> result(byte_sz / sizeof(T), byte_sz / sizeof(T));
        file.seekg(0);
        file.read((char*)result.data, byte_sz);
        file.close();
        return result;
    }

    array(memory*   mem) : mx(mem), data(mx::data<T>()) { }
    array(mx          o) : array(o.mem->hold()) { }
    array()              : array(mx::alloc<array>(null, 0, 1)) { }

    /// immutable
    array<T> unshift(T &v) const {
        array<T> res { mem->count + 1 };
        res += v;
        for (size_t i = 0; i < mem->count; i++)
            res += data[i];
        return res;
    }

    /// immutable 
    array<T> shift() const {
        if (mem->count <= 1)
            return {};
        array<T> res { mem->count - 1 };
        for (size_t i = 1; i < mem->count; i++)
            res += data[i];
        return res;
    }
    /// likely need to do a constexpr to check if its able to concat
    /// in the condition its not it would be best to return default
    /// and not implement by other means
    T join(T s = T()) const {
        T v = data[0];
        for (size_t i = 1; i < mem->count; i++)
            v += data[i];
        return v;
    }

    array<T> reverse() const {
        array<T> res;
        for (size_t i = 0; i < mem->count; i++)
            res += data[mem->count - 1 - i];
        return res;
    }

    array<T> slice(size_t start, size_t end) const {
        size_t count = end - start;
        assert(count <= mem->count);
        array<T> res;
        for (size_t i = start; i < start + count; i++) {
            res += data[i];
        }
        return res;
    }

    array<T> every(lambda<bool(T&)> fn) const {
        array<T> res;
        for (T &v: *this) {
            if (fn(v))
                res.push(v);
        }
        return res;
    }

    array<T> some(lambda<bool(T&)> fn) const {
        bool res = false;
        for (T &v: *this) {
            if (fn(v)) {
                res = true;
                break;
            }
        }
        return res;
    }

    //intern    *operator &() { return  data; } -- dont ever do these!
    //operator     intern *() { return  data; }

    template <typename B>
    array      &operator=(const array<B> &b) {\
        mx::drop();\
        mx::mem = b.mem->hold();\
        assert(typeof(B) == typeof(T));
        data = (intern*)mx::mem->data<intern>(0);\
        return *this;\
    }\

    /// push an element, return its reference
    T &push(const T &v) {
        size_t csz = mem->count;
        if (csz >= alloc_size())
            data = (T*)mem->realloc(csz + 1, false);
        new (&data[csz]) T(v);
        mem->count++;
        return data[csz];
    }

    T &push() {
        size_t csz = mem->count;
        if (csz >= alloc_size())
            data = (T*)mem->realloc(csz + 1, false);
        new (&data[csz]) T();
        mem->count++;
        return data[csz];
    }

    void push(T *pv, size_t push_count) {
        if (!push_count) return; 
        ///
        size_t usz = size_t(mem->count);
        size_t rsv = size_t(mem->reserve);

        if (usz + push_count > rsv)
            data = (T*)mem->realloc(usz + push_count, false);
        
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

    void pop(T *prev = null) {
        size_t i = size_t(mem->count);
        assert(i > 0);
        if (prev)
        *prev = data[i];
        if constexpr (!is_primitive<T>())
            data[i]. ~T();
        --mem->count;
    }

    ///
    template <typename S>
    memory *convert_elements(array<S> &a_src) {
        constexpr bool can_convert = std::is_convertible<S, T>::value;
        if constexpr (can_convert) {
            memory* src = a_src.mem;
            if (!src) return null;
            memory* dst;
            if constexpr (identical<T, S>()) {
                dst = src->hold();
            } else {
                type_t ty = typeof(T);
                size_t sz = a_src.len();
                dst       = memory::alloc(ty, sz, sz, (T*)null);
                T*      b = dst->data<T>(0);
                S*      a = src->data<S>(0);
                for (size_t i = 0; i < sz; i++)
                    new (&b[i]) T(a[i]);
            }
            return dst;
        } else {
            printf("cannot convert elements on array\n");
            return null;
        }
    }

    static inline type_t data_type() { return typeof(T); }
    
    static array<T> empty() { return array<T>(size_t(1)); }

    array(initial<T> args) : array() { //  size_t(args.size()) -- doesnt work with the pre-alloc; find out why.
        for (auto &v:args) {
            T &element = (T &)v;
            push(element);
        }
    }

    inline void reserve_size(size sz) {
        if (mem->reserve < sz)
            data = (T*)mem->realloc(sz, true);
    }

    void set_size(size_t sz) {
        mem->count = sz;
    }

    ///
    size_t count_of(T v) {
        size_t c = 0;
        for (size_t i = 0; i < mem->count; i++)
            if (data[i] == v)
                c++;
        return c;
    }

    ///
    T* elements() const { return data; }

    ///
    int index_of(const T &v) const {
        for (size_t i = 0; i < mem->count; i++) {
            if (data[i] == (T&)v)
                return int(i);
        }
        return -1;
    }

    int index_of(lambda<bool(T&)> v) const {
        for (size_t i = 0; i < mem->count; i++) {
            if (v(data[i]))
                return int(i);
        }
        return -1;
    }

    bool contains(const T &v) const {
        return index_of(v) != -1;
    }

    ///
    template <typename R>
    R select_first(lambda<R(T&)> qf) const {
        for (size_t i = 0; i < mem->count; i++) {
            R r = qf(data[i]);
            if (r)
                return r;
        }
        if constexpr (is_numeric<R>())
            return R(0);
        else
            return R();
    }

    template <typename R>
    array<R> map(lambda<R(T&)> qf) const {
        array<R> res(mem->count);
        ///
        for (size_t i = 0; i < mem->count; i++) {
            R r = qf(data[i]);
            res += r;
        }
        return res;
    }

    template <typename R>
    array<R> flat_map(lambda<R(T&)> qf) const {
        array<R> res(mem->count);
        ///
        for (size_t i = 0; i < mem->count; i++) {
            array<R> r = qf(data[i]);
            for (R &ri: r)
                res += ri;
        }
        return res;
    }

    ///
    template <typename R>
    R select(lambda<R(T&)> qf) const {
        array<R> res(mem->count);
        ///
        for (size_t i = 0; i < mem->count; i++) {
            R r = qf(data[i]);
            if (r)
                res += r;
        }
        return res;
    }

    /// quick-sort
    array<T> sort(lambda<int(T &a, T &b)> cmp) {
        /// create reference list of identical size as given, pointing to the respective index
        size_t sz = len();
        T **refs = (T **)calloc64(len(), sizeof(T*));
        for (size_t i = 0; i < sz; i++)
            refs[i] = &data[i];

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
        array<T> result { ln };
        for (size_t i = 0; i < ln; i++) {
            T &r = *refs[i];
            result.push(r);
        }

        //free(refs);
        return result;
    }
    
    inline size     shape() const { return mem->shape ? *mem->shape : size(mem->count); }
    inline size_t     len() const { return mem->count; }
    inline size_t  length() const { return mem->count; }
    inline size_t reserve() const { return mem->reserve; }

    array(size al, size sz = size(0)) : 
            array(talloc<T>(sz, al)) {
        if (al.count > 1)
            mem->shape = new size(al); /// allocate a shape if there are more than 1 dims
    }

    /// constructor for allocating with a space to fill (and construct; this allocation does not run the constructor (if non-primitive) until append)
    /// indexing outside of shape space does cause error
    array(size_t  reserve) : array(talloc<T>(0, size_t(reserve))) { }
    array(   u32  reserve) : array(talloc<T>(0, size_t(reserve))) { }
    array(   i32  reserve) : array(talloc<T>(0, size_t(reserve))) { }
    array(   i64  reserve) : array(talloc<T>(0, size_t(reserve))) { }

    inline T &push_default()  { return push();  }

    /// important that this should always assign or
    /// copy construct the elements in, as supposed to  a simple reference
    array<T> mid(int start, int len = -1) const {
        /// make sure we dont stride out of bounds
        int ilen = int(this->len());
        assert(abs(start) < ilen);
        if (start < 0) start = ilen + start;
        size_t cp_count = len < 0 ? math::max(0, (ilen - start) + len) : len; /// 0 is a value to keep as 0 len, auto starts at -1 (+ -1 from remaining len)
        assert(start + cp_count <= ilen);

        array<T> result(cp_count);
        for (size_t i = 0; i < cp_count; i++)
            result.push(data[start + i]);
        ///
        return result;
    }

    /// get has more logic in the indexing than the delim.  wouldnt think of messing with delim logic but this is useful in general
    inline T &get(num i) const {
        if (i < 0) {
            i += num(mem->count);
            assert(i >= 0);
        }
        assert(i >= 0 && i < num(mem->count));
        return data[i];
    }


    iter<T>  begin() const { return { (T *)data, size_t(0)    }; }
	iter<T>    end() const { return { (T *)data, size_t(mem->count) }; }
    T&       first() const { return data[0];        }
	T&        last() const { return data[mem->count - 1]; }
    
    void realloc(size s_to) {
        data = mx::realloc(s_to, false);
    }

    operator  bool() const { return mx::mem && mem->count > 0; }
    bool operator!() const { return !(operator bool()); }

    bool operator==(array b) const {
        if (mx::mem == b.mem) return true;
        if (mx::mem && b.mem) {
            if (mem->count != b.mem->count) return false;
            for (size_t i = 0; i < mem->count; i++)
                if (!(data[i] == b.data[i])) return false;
        }
        return true;
    }

    /// not-equals
    bool operator!=(array b) const { return !(operator==(b)); }
    
    inline T &operator[](size index) const {
        if (index.count == 1) {
            size_t i = size_t(index.values[0]);
            assert(i >= 0 && i < mem->count);
            /// make sure this is within valid count range
            /// want to make sure all arrays even with primitive values do this
            return (T &)data[i];
        } else {
            assert(mem->shape);
            mem->shape->assert_index(index);
            size_t i = mem->shape->index_value(index);
            return (T &)data[i];
        }
    }

    inline T &operator[](size_t index) { return data[index]; }

    template <typename IX>
    inline T &operator[](IX index) {
        if constexpr (identical<IX, size>()) {
            mem->shape->assert_index(index);
            size_t i = mem->shape->index_value(index);
            return (T &)data[i];
        } else if constexpr (std::is_enum<IX>() || is_integral<IX>()) { /// just dont bitch at people.  people index by so many types.. most type casts are crap and noise as result
            size_t i = size_t(index);
            return (T &)data[i];
        } else {
            /// now this is where you bitch...
            assert(!"index type must be size or integral type");
            return (T &)data[0];
        }
    }

    inline T &operator+=(const T &v) { 
        return push(v);
    }

    /// clearing a list reallocates to 1, destructing previous
    void clear() {
        for (int i = 0; i < mem->count; i++) {
            data[i] . ~T();
        }
        mem->count = 0;
    }

    /// destructing a list means it keeps the size
    void destruct() {
        for (int i = 0; i < mem->count; i++) {
            data[i] . ~T();
        }
        mem->count = 0;
    }
};

template <typename T>
T convert_str(const str& s) {
    return T(s);
}

// this can be converted to mx
template <>
str convert_str<str>(const str& s);

template <typename Lambda, std::size_t... Is>
auto invokeImpl(
        const Lambda& lambda,
        const std::index_sequence<Is...>&,
        const array<str>& args)
{
    using traits = lambda_traits<Lambda>;
    return lambda(convert_str<std::tuple_element_t<Is, typename traits::arg_types>>(args[Is])...);
}

template <typename Lambda>
auto invoke(const Lambda& lambda, const array<str>& args) {
    using traits = lambda_traits<Lambda>;
    constexpr size_t numArgs = std::tuple_size_v<typename traits::arg_types>;

    if (args.len() != numArgs) {
        throw std::runtime_error("Incorrect number of arguments.");
    }
    /// 
    return invokeImpl(lambda, std::make_index_sequence<numArgs>{}, args);
}


/// stream accept types going out, and accept types going in
/// we just put them in a list of accepted.  its good to declare that for something as robust as streaming
/// Streamers will use an array of these; I dont believe its a good idea to 'name' each stream though; type identity is fine
/// 
struct io:mx {
    struct Source {
        type_t           type; /// types are the stream identifier
        lambda<void(mx)> fn;   /// 'sink' of sort
    };

    struct M {
        array<Source> sources;
        array<type_t> types_out;
        lambda<void()> shutdown;
        ~M() {
            /// stream shutdown
            if (shutdown)
                shutdown();
        }

        void emit(mx data) {
            type_t type = data.type();
            for (Source &src: sources) {
                if (src.type == type)
                    src.fn(data);
            }
        }
        
        bool attach(Source &src) {
            bool valid = types_out.index_of([&](Source &a) -> bool {
                return a.type == src.type;
            }) >= 0;
            assert(valid);
            sources += src;
            return valid;
        }
    };
    
    mx_basic(io)

    io(array<type_t> types_out, lambda<void()> shutdown) : io() {
        data->types_out = types_out;
        data->shutdown  = shutdown;
    }

    bool operator+=(Source src) {
        return data->attach(src);
    }
};

using arg = pair<mx, mx>;
using ax  = array<arg>;

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
        memory **psym = null;
        if (raw.type() == typeof(char)) {
            char  *d = &raw.ref<char>();
            /// in json, the enum can sometimes be given in "23124" form; no enum begins with a numeric so we can handle this
            if (d[0] == '-' || isdigit(*d)) {
                std::string str = (symbol)d;
                int num = std::stoi(str);
                psym = type->symbols->ids.lookup((i64)num);
            } else {
                u64 hash = djb2(d);
                psym     = type->symbols->djb2.lookup(hash);
            }
            if (!psym) {
                /// if lookup fails, compare by the number of chars given
                for (memory *mem: type->symbols->list) {
                    if (raw.mem->count > mem->count)
                        continue;
                    if (matches((symbol)mem->origin, (symbol)d, raw.mem->count))
                        return (typename C::etype)mem->id;
                }
            }
        } else if (raw.type() == typeof(int)) {
            i64   id = i64(raw.ref<int>());
            psym     = type->symbols->ids.lookup(id);
        } else if (raw.type() == typeof(i64)) {
            i64   id = raw.ref<i64>();
            psym     = type->symbols->ids.lookup(id);
        } else if (raw.type() == typeof(typename C::etype)) {
            i64   id = raw.ref<typename C::etype>();
            psym     = type->symbols->ids.lookup(id);
        }
        if (!psym) {
            printf("symbol: %s, raw: %s\n", S, (char*)raw.mem->origin);
            fflush(stdout);
            throw C();
        }
        return (typename C::etype)((*psym)->id);
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
	static utf16 join(array<utf16> &src, utf16 j);

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
    inline static const type_t intern_t = typeof(char);
    using char_t = unsigned short;

    cstr data;

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

    /// static methods
    static str combine(const str sa, const str sb);

    /// skip to next non-whitespace
    static char &non_wspace(cstr cs);

    str(memory        *mem);
    str(null_t = null);
    str(char            ch);
    str(size_t          sz);
    
    //str(std::ifstream  &in) : str((memory*)null) { }
    str(std::ifstream &in);

    static void code_to_utf8(int code, char *output);

    static str from_code(int code);

    static str from_integer(i64 i);

    str(float          f32, int n = 6);
    str(double         f64, int n = 6);

    /// no more symbol usage in str. thats garbage
    str(symbol cs, size_t len = memory::autolen, size_t rs = 0);
    str(cstr   cs, size_t len = memory::autolen, size_t rs = 0);
    str(std::string s);
    str(const str &s);

    cstr cs() const;

    /// tested.
    str expr(lambda<str(str)> fn) const;

    /// format is a user of expr
    str format(array<mx> args) const;

    /// just using cs here, for how i typically use it you could cache the strings
    static str format(symbol cs, array<mx> args);

    operator fs::path() const;
    
    void        clear() const;

    bool        contains   (array<str>   a) const;
    str         operator+  (symbol       s) const;
    bool        operator<  (const str    b) const;
    bool        operator>  (const str    b) const;
    bool        operator<  (symbol       b) const;
    bool        operator>  (symbol       b) const;
    bool        operator<= (const str    b) const;
    bool        operator>= (const str    b) const;
    bool        operator<= (symbol       b) const;
    bool        operator>= (symbol       b) const;
  //bool        operator== (std::string  b) const;
  //bool        operator!= (std::string  b) const;
    bool        operator== (str          b) const;
    bool        operator!= (str          b) const;
    bool        operator== (symbol       b) const;
    bool        operator!= (symbol       b) const;
    char&		operator[] (size_t       i) const;
    int         operator[] (str          b) const;
                operator             bool() const;
    bool        operator!()                 const;
    str         operator+    (const str sb) const;
    str        &operator+=   (str        b);

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
    
    int index_of_first(array<str> elements, int *str_index) const;

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
    array<str> split(L delim) const {
        array<str>  result;
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
                    str  mm = mid(start, int(end - start));
                    result += mm;
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

    array<str> split(symbol s) const;
    array<str> split();

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

using astr = array<str>;

template <typename C, typename E>
E ex::initialize(C *p, E v, symbol names, type_t ty) {
    /// names should support normal enum syntax like abc = 2, abc2 = 4, abc3, abc4; we can infer what C++ does to its values
    /// get this value from raw.origin (symbol) instead of the S
    if (ty->secondary_init) return v;
    ty->secondary_init = true;
    array<str>   sp = str((cstr)names).split(", ");
    int           c = (int)sp.len();
    i64        next = 0;

    for (int i = 0; i < c; i++) {
        num idx = sp[i].index_of(str("="));
        if (idx >= 0) {
            str sym = sp[i].mid(0, idx).trim();
            str val = sp[i].mid(idx + 1).trim();
            mem_symbol(sym.data, ty, val.integer_value());
        } else {
            mem_symbol(sp[i].data, ty, i64(next));
        }
        next = i + 1;
    };
    return v;
}

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
    inline fmt(str templ, array<mx> args) : str(templ.format(args)) { }
    fmt():str() { }

    operator memory*() { return hold(); }
};

/// cstr operator overload
inline str operator+(cstr cs, str rhs) { return str(cs) + rhs; }

template <typename V>
struct map:mx {
    static inline type_t value_type() { return typeof(V); }
    inline static const size_t default_hash_size = 64;
    using hmap = ion::hmap<mx, field<V>*>;

    using ValueType = V;

    struct mdata {
        doubly<field<V>>  fields;
        hmap           *hash_map;

        /// boolean operator for key
        bool operator()(mx key) {
            if (hash_map)
                return hash_map->lookup(key);
            else {
                for (field<V> &f:fields)
                    if (f.key.mem == key.mem)
                        return true;
            }
            return false; 
        }

        template <typename K>
        inline V &operator[](K k) {
            if constexpr (ion::inherits<mx, K>()) {
                return fetch(k).value;
            } else {
                mx io_key = mx(k);
                return fetch(io_key).value;
            }
        }

        field<V> &first()  { return fields->first(); }
        field<V> & last()  { return fields-> last(); }
        size_t    count()  { return fields->len();   }
        size_t      len()  { return fields->len();   }

        template <typename R>
        array<R> map(lambda<R(field<V>&)> fn) {
            array<R> r { fields->len() };
            for (field<V> &f:fields)
                r += fn(f);
            return r;
        }

        size_t    count(mx k) {
            type_t tk = k.type();
            memory *b = k.mem;
            if (!(k.mem->attrs & memory::constant) && (tk->traits & traits::primitive)) {
                size_t res = 0;
                for (field<V> &f:fields) {
                    memory *a = f.key.mem;
                    assert(a->type->size() == b->type->size());
                    if (a == b || (a->count == b->count && memcmp(a->origin, b->origin, a->count * a->type->size()) == 0)) // think of new name instead of type_* ; worse yet is handle base types in type
                        res++;
                }
                return res;
            } else {
                size_t res = 0;
                for (field<V> &f:fields)
                    if (f.key.mem == b)
                        res++;
                return res;
            }
        }

        size_t count(cstr cs) {
            size_t res = 0;
            for (field<V> &f:fields)
                if (strcmp(symbol(f.key.mem->origin), symbol(cs)) == 0)
                    res++;
            return res;
        }

        field<V> *lookup(mx &k) const {
            if (hash_map) {
                field<V> **ppf = hash_map->lookup(k);
                return  ppf ? *ppf : null;
            } else {
                for (field<V> & f : fields)
                    if (f.key == k)
                        return &f;
            }
            return null;
        }

        V* get(mx key) const {
            field<V> *f = lookup(key);
            return f ? &f->value : null;
        }

        bool remove(field<V> &f) {
            item<field<V>> *i = item<field<V>>::container(f); // the container on field<V> is item<field<V>>, a negative offset
            fields->remove(i);
            return true;
        }

        bool remove(mx &k) {
            field<V> *f = lookup(k);
            return f ? remove(*f) : false;
        }

        template <typename K>
        array<K> keys(K *t = null) {
            array<K> res { fields->len() };
            for (field<V> &f:fields) {
                K k = K(f.key.hold());
                res.push(k);
            }
            return res;
        }

        array<V> values() {
            array<V> res { fields->len() };
            for (field<V> &f:fields)
                res.push(f.value);
            return res;
        }

        field<V> &fetch(mx &k) {
            field<V> *f = lookup(k);
            if (f) return *f;
            ///
            fields += { k, V() };
            field<V> &last = fields->last();

            if (hash_map)
              (*hash_map)[k] = &last;

            return last;
        }

        operator bool() { return ((hash_map && hash_map->len() > 0) || (fields->len() > 0)); }
    };

    /// an init for type would be useful; then we could fill out more on the type
    static void set_value(map<V> *m, memory *key, memory *m_item) {
        if constexpr (is_convertible<memory*, V>()) {
            mx mkey = key->hold();
            assert(m_item->type == typeof(V));

            if constexpr (ion::inherits<mx, V>()) {
                /// hold memory
                (*m)->fetch(mkey).value = m_item->hold();
            } else {
                /// copy V, a non-mx value
                (*m)->fetch(mkey).value = *(V*)m_item->origin;
            }
        }
    }

    static int defaults(map<V> &def) {
        for (field<V> &f: def) {
            str name  = f.key.hold();
            str value = f.value.mem->type->functions->to_string(f.value.mem->origin);
            printf("%s: default (%s)", name.cs(), value.cs());
        }
        return 1;
    }

    static map<V> parse(int argc, cstr *argv, map<V> &def) {
        map<V> iargs = map<V>();
        ///
        for (int ai = 0; ai < argc; ai++) {
            cstr ps = argv[ai];
            ///
            if (ps[0] == '-') {
                bool   is_single = ps[1] != '-';
                mx key {
                    cstr(&ps[is_single ? 1 : 2]), typeof(char)
                };
                field<mx>* found;
                if (is_single) {
                    for (field<mx> &df: def) {
                        symbol s = symbol(df.key);
                        if (ps[1] == s[0]) {
                            found = &df;
                            break;
                        }
                    }
                } else found = def->lookup(key);
                ///
                if (found) {
                    str     aval = str(argv[ai + 1]);
                    type_t  type = found->value.type();
                    mx mstr = type->functions->from_string((none*)null, (cstr)aval.mem->origin);
                    iargs[key] = mstr;
                } else {
                    printf("arg not found: %s\n", str(key.mem).data);
                    return {};
                }
            }
        }
        ///
        /// return results in order of defaults, with default value given
        map<V> res = map<V>();
        for(field<V> &df:def.data->fields) {
            field<V> *ov = iargs->lookup(df.key);
            res.data->fields += { df.key, ov ? ov->value : df.value };
        }
        return res;
    }

    template <typename K>
    array<K> keys() {
        return data->keys((K*)null); /// cannot pass K without a phony default arg.
    }

    array<V> values() {
        return data->values();
    }

    size_t len() { return data->fields->len(); }
    
    void print();

    /// props bypass dependency with this operator returning a list of field, which is supported by mx (map would not be)
    operator doubly<field<V>> &() { return data->fields; }
    bool       operator()(mx key) { return (*data)(key); }

    bool contains(mx key) { return (*data)(key); }
    
    operator               bool() { return mx::mem && *data; }

    template <typename K>  K &get(K k) const { return lookup(k)->value; }
    inline liter<field<V>>     begin() const { return data->fields->begin(); }
    inline liter<field<V>>       end() const { return data->fields->end();   }

    mx_object(map, mx, mdata);

    /// when a size is specified to map, it engages hash map mode
    map(size sz) : map() {
        data->fields = doubly<field<V>>();
        if (sz) data->hash_map = new hmap(sz);
    }

    map(size_t sz) : map() {
        data->fields = doubly<field<V>>();
        if (sz) data->hash_map = new hmap(sz);
    }

    map(initial<field<V>> args) : map(size(0)) {
        for(auto &f:args) data->fields += f;
    }

    map(array<field<V>> arr) : map(size(default_hash_size)) {
        for(field<V> &f:arr)  data->fields += f;
    }

    map(doubly<field<V>> li) : map(size(default_hash_size)) {
        for(field<V> &f:li)   data->fields += f;
    }
    
    /// pass through key operator
    template <typename K>
    inline V &operator[](K k) { return (*data)[k]; }
};

using args = map<mx>;

template <typename T> struct is_array<array<T>> : true_type  {};
template <typename T> struct is_map  <map  <T>> : true_type  {};

template <typename T>
struct assigner {
    protected:
    T val;
    lambda<void(bool&)> &on_assign;
    public:

    ///
    inline assigner(T v, lambda<void(bool&)> &fn) : val(v), on_assign(fn) { }

    ///
    inline operator T() { return val; }

    /// the cycle must end [/picard-shoots-picard]
    inline void operator=(T n) {
        val = n; /// probably wont set this
        on_assign(val);
    }
};

template <typename T>
struct uniques:mx {
    struct M {
        std::unordered_set<T> uset;
    };

    uniques(std::initializer_list<T> l) : uniques() {
        data->uset = l;
    }

    bool set(T v) {
        std::pair<typename std::unordered_set<T>::iterator, bool> i = data->uset.insert(v);
        return bool(i.second);
    }

    bool unset(T v) {
        size_t count = data->uset.erase(v);
        return count == 1;
    }

    bool contains(T v) {
        return data->uset.find(v) != data->uset.end();
    }

    /// bit ridiculous, but why not have the syntax
    inline assigner<bool> operator[](T v) const {
        lambda<void(bool&)> fn {
            [&](bool &from_assign) -> void {
                if (from_assign)
                    data->uset.insert(v);
                else
                    data->uset.erase(v);
            }
        };
        return { v, fn };
    }

    mx_basic(uniques)
};

using Map = map<mx>;

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
    inline states(initial<T> args) : states() {
        for (auto  v:args) {
            i64 ord = i64(v); /// from an enum type (best to support sign)
            u64 fla = to_flag(ord);
            data->bits |= fla;
        }
    }

    memory *string() const {
        if (!data->bits)
            return memory::symbol("null");

        if constexpr (has_count<T>::value) {
            const size_t sz = 32 * (T::count + 1);
            char format[sz];
            ///
            doubly<memory*> &symbols = T::symbols();
            ///
            u64    v     = data->bits;
            size_t c_id  = 0;
            size_t c_len = 0;
            bool   first = true;
            ///
            for (memory *sym:symbols) {
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

    void clear(T v) {
        u64 f = to_flag((i64)i32(v));
        data->bits &= ~f;
    }

    void set(T v) {
        u64 f = to_flag((i64)i32(v)); /// this was reading u64 from an int, segfaulting
        data->bits |= f;
    }

    template <typename ET>
    inline assigner<bool> operator[](ET varg) const {
        u64 f = 0;
        if constexpr (identical<ET, initial<T>>()) {
            for (auto &v:varg)
                f |= to_flag(u32(v));
        } else if constexpr (identical<ET, symbol>()) { /// support states["enum-name"] = true;  this reduces code
            T v = T(varg);
            f = to_flag(i32(v.value));
        } else
            f = to_flag(i32(varg));
        
        lambda<void(bool&)> fn {
            [this, f](bool &from_assign) -> void {
                if (from_assign)
                    data->bits |=  f;
                else
                    data->bits &= ~f;
            }
        };
        return { bool((data->bits & f) == f), fn };
    }

    operator memory*() { return hold(); }
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

/// console interface for printing, it could get input as well
/// print has no topic, so one must be able to redirect all logging.  no reason not to have a global here
struct logger {
    enum option {
        err,
        append
    };

    inline static type_t l_type = typeof(lambda<void(mx)>);
    inline static lambda<void(mx)> service;

    protected:
    static void _print(const str &st, const array<mx> &ar, const states<option> opts) {
        static std::mutex mtx;
        mtx.lock();
        str msg = st.format(ar);
        if (service)
            service(msg);
        fputs(msg.data, stdout);
        fputs("\n", stdout);
        fflush(stdout);
        mtx.unlock();
    }


    public:
    inline void log(mx m, array<mx> ar = {}) {
        str st = m.to_string();
        _print(st, ar, { });
    }

    void test(const bool cond, mx templ = {}, array<mx> ar = {}) {
        #ifndef NDEBUG
        if (!cond) {
            str st = templ.count() ? templ.to_string() : null;
            _print(st, ar, { });
            exit(1);
        }
        #endif
    }

    static str input() {
        str      s  = str(size_t(1024)); /// strip integer conversions from constructors here in favor of converting to reserve alone.  that is simple.
        fgets(s.data, 1024, stdin);
        s.mem->count = strlen(s.data);
        return s;
    }

    inline void fault(mx m, array<mx> ar = array<mx> { }) { str s = m.to_string(); _print(s, ar, { err }); brexit(); }
    inline void error(mx m, array<mx> ar = array<mx> { }) { str s = m.to_string(); _print(s, ar, { err }); brexit(); }
    inline void print(mx m, array<mx> ar = array<mx> { }) { str s = m.to_string(); _print(s, ar, { append }); }
    inline void debug(mx m, array<mx> ar = array<mx> { }) { str s = m.to_string(); _print(s, ar, { append }); }
};

/// define a logger for global use; 'console' can certainly be used as delegate in mx or node, for added context
extern logger console;

struct basic_string {
    cstr cs;
    int  len;
};

/// use char as base.
struct path:mx {
    inline static std::error_code ec;

    using Fn = lambda<void(path)>;

    enums(option, recursion,
         recursion, no_sym_links, no_hidden, use_git_ignores);

    enums(op, none,
         none, deleted, modified, created);

    static path cwd() {
        return dir::cwd();
    }

    struct M {
        fs::path p;
        memory* to_string() {
            static std::string str;
            str = p.string();
            return memory::string(str);
        }
    };
    mx_basic(path);

    path(str      s) : path() {
        data->p = fs::path((symbol)s.cs());
    }

    path(symbol  cs) : path() {
        data->p = fs::path(cs);
    }

    template <typename T> T     read(symbol subpath = null) const;
    template <typename T> bool write(T input) const;

    bool get_modified_date(struct tm *res);
    str  get_modified_string();

    /// utility for knowing if you are trying to go up beyond a relative dir
    /// without any dir analysis is more reduced
    static bool backwards(cstr cs) {
        cstr v = cs;
        if (v[0] == '/') v++;
        double b = 0;
        ///
        for (; v[0];) {
            if (v[0] == '.' && v[1] == '.') {
                if (v[2] != 0 && v[2] != '/')
                    return true; /// this is backwards enough
                v++;
                b--;
            } else if (v[0] != '/') {
                // double slash is same dir, we 'ignore' that
                b++;
                do {
                    v++;
                } while (v[0] && v[0] != '/');

                if (v[0] == '/')
                    v++;
            }
            if (v[0])
                v++;
        }
        return b < 0;
    }

    fs::path *pdata() const {
        return &data->p;
    }

    str        mime_type();
    str             stem() const { return !pdata()->empty() ? str(pdata()->stem().string()) : str(null);    }
    bool      remove_all() const { std::error_code ec;          return fs::remove_all(*pdata(), ec); }
    bool          remove() const { std::error_code ec;          return fs::remove(*pdata(), ec); }
    bool       is_hidden() const { auto st = pdata()->stem().string(); return st.length() > 0 && st[0] == '.'; }
    bool          exists() const {                              return fs::exists(*pdata()); }
    bool          is_dir() const {                              return fs::is_directory(*pdata()); }
    bool        make_dir() const { std::error_code ec;          return !pdata()->empty() ? fs::create_directories(*pdata(), ec) : false; }
    path remove_filename()       {
        fs::path p = pdata()->remove_filename();
        return path(p.string().c_str());
    }
    bool    has_filename() const { return pdata()->has_filename(); }
    path            link() const { return fs::is_symlink(*pdata()) ? path(pdata()->string().c_str()) : path(); }
    bool         is_file() const { return !fs::is_directory(*pdata()) && fs::is_regular_file(*pdata()); }
    char             *cs() const {
        static std::string static_thing;
        static_thing = pdata()->string();
        return (char*)static_thing.c_str();
    }
    operator cstr() const {
        return cstr(cs());
    }

    str         ext () const { return str(pdata()->extension().string()); }
    str         ext4() const { return pdata()->extension().string(); }
    path        file() const { return fs::is_regular_file(*pdata()) ? path(cs()) : path(); }
    bool copy(path to) const {
        assert(!pdata()->empty());
        assert(exists());
        if (!to.exists())
            (to.has_filename() ?
                to.remove_filename() : to).make_dir();

        std::error_code ec;
        path p = to.is_dir() ? to / path(str(pdata()->filename().string())) : to;
        fs::copy(*pdata(), *p.pdata(), ec);
        return ec.value() == 0;
    }
    
    path &assert_file() {
        assert(fs::is_regular_file(*pdata()) && exists());
        return *this;
    }

    /// create an alias; if successful return its location
    path link(path alias) const {
        fs::path &ap = *pdata();
        fs::path &bp = *alias.pdata();
        if (ap.empty() || bp.empty())
            return {};
        std::error_code ec;
        is_dir() ? fs::create_directory_symlink(ap, bp) : fs::create_symlink(ap, bp, ec);
        return fs::exists(bp) ? alias : path();
    }

    /// operators
    operator        bool()         const {
        return cs()[0];
    }
    operator         str()         const { return str(cs()); }
    path          parent()         const {
        std::string s = pdata()->parent_path().string();
        return  s.c_str();
    }
    
    path operator / (path       s) const { return path((*pdata() / *s.pdata()).string().c_str()); }
    path operator / (symbol     s) const { return path((*pdata() /  s).string().c_str()); }
    path operator / (const str& s) const { return path((*pdata() / fs::path(s.data)).string().c_str()); }
    path relative   (path    from) const { return path(fs::relative(*pdata(), *from.pdata()).string().c_str()); }
    
    bool  operator==(path&      b) const { return  *pdata() == *b.pdata(); }
    bool  operator!=(path&      b) const { return !(operator==(b)); }
    
    bool  operator==(symbol     b) const { return *pdata() == b; }
    bool  operator!=(symbol     b) const { return !(operator==(b)); }

    ///
    static path uid(path b) {
        auto rand = [](num i) -> str { return rand::uniform('a', 'z'); };
        fs::path p { };
        do { p = fs::path(str::fill(6, rand)); }
        while (fs::exists(*b.pdata() / p));
        std::string s = p.string();
        return  s.c_str();
    }

    static path  format(str t, array<mx> args) {
        return t.format(args);
    }

    int64_t modified_at() const {
        using namespace std::chrono_literals;
        std::string ps = pdata()->string();
        auto        wt = fs::last_write_time(*pdata());
        const auto  ms = wt.time_since_epoch().count(); // offset needed atm
        return int64_t(ms);
    }

    bool read(size_t bs, lambda<void(symbol, size_t)> fn) {
        cstr  buf = new char[bs];
        fs::path *pdata = this->pdata();
        try {
            std::error_code ec;
            size_t rsize = fs::file_size(*pdata, ec);
            /// dont throw in the towel. no-exceptions.
            if (!ec)
                return false;
            std::ifstream f(*pdata);
            for (num i = 0, n = (rsize / bs) + (rsize % bs != 0); i < n; i++) {
                size_t sz = i == (n - 1) ? rsize - (rsize / bs * bs) : bs;
                fn((symbol)buf, sz);
            }
        } catch (std::ofstream::failure e) {
            std::cerr << "read failure: " << pdata->string();
        }
        delete[] buf;
        return true;
    }

    bool append(array<uint8_t> bytes) {
        fs::path *pdata = this->pdata();
        try {
            size_t sz = bytes.len();
            std::ofstream f(*pdata, std::ios::out | std::ios::binary | std::ios::app);
            if (sz)
                f.write((symbol)bytes.data, sz);
        } catch (std::ofstream::failure e) {
            std::cerr << "read failure on resource: " << pdata->string() << std::endl;
        }
        return true;
    }

    /// visual studio code should use this
    bool same_as  (path b) const { std::error_code ec; return fs::equivalent(*pdata(), *b.pdata(), ec); }

    void resources(array<str> exts, states<option> states, Fn fn) {
        fs::path *pdata     = this->pdata();
        bool use_gitignore	= states[ option::use_git_ignores ];
        bool recursive		= states[ option::recursion       ];
        bool no_hidden		= states[ option::no_hidden       ];
        array<str> ignore   = states[ option::use_git_ignores ] ? path((*pdata / ".gitignore").string().c_str()).read<str>().split("\n") : array<str>();
        lambda<void(path)> res;
        map<mx>    fetched_dir;  /// this is temp and map needs a hash lambdas pronto
        fs::path   parent   = *pdata; /// parent relative to the git-ignore index; there may be multiple of these things.

        ///
        res = [&](path a) {
            auto fn_filter = [&](ion::path p) -> bool {
                str      ext = p.ext4();
                bool proceed = false;
                /// proceed if the extension is matching one, or no extensions are given
                if (!exts) {
                    proceed = !no_hidden || !is_hidden();
                } else
                    for (size_t i = 0; i < exts.len(); i++) {
                        const str &e = (const str &)exts[i];
                        if (ext == e) {
                            proceed = !no_hidden || !is_hidden();
                            break;
                        }
                    }

                /// proceed if proceeding, and either not using git ignore,
                /// or the file passes the git ignore collection of patterns
                
                if (proceed && use_gitignore) {
                    path    pp = path(parent.string().c_str());
                    path   rel = pp.relative(p); // original parent, not resource parent
                    str   srel = rel;
                    ///
                    for (str& i: ignore)
                        if (i && srel.has_prefix(i)) {
                            proceed = false;
                            break;
                        }
                }
                
                /// call lambda for resource
                if (proceed) {
                    fn(p);
                    return true;
                }
                return false;
            };

            /// directory of resources
            if (a.is_dir()) {
                if (!no_hidden || !a.is_hidden())
                    for (fs::directory_entry e: fs::directory_iterator(*a.pdata())) {
                        std::string str = e.path().string();
                        path p  = str.c_str();
                        path li = p.link();
                        //if (li)
                        //    continue;
                        path pp = li ? li : p;
                        if (recursive && pp.is_dir()) {
                            if (fetched_dir.lookup(mx(pp)))
                                return;
                            fetched_dir[pp] = mx(true);
                            res(pp);
                        }
                        ///
                        if (pp.is_file())
                            fn_filter(pp);
                    }
            } /// single resource
            else if (a.exists())
                fn_filter(a);
        };
        std::string str = pdata->string();
        symbol sym = str.c_str();
        return res(sym);
    }

    array<path> matching(array<str> exts) {
        auto ret = array<path>();
        resources(exts, { }, [&](path p) { ret += p; });
        return ret;
    }

    operator str() { return str(cs(), memory::autolen); }
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

    static array<u8> decode(symbol b64, size_t b64_len) {
        assert(b64_len % 4 == 0);
        /// --------------------------------------
        umap<size_t, size_t> &dec = dec_map();
        size_t alloc_sz = b64_len / 4 * 3;
        array<u8> out(size_t(alloc_sz + 1));
        u8     *o = out.data;
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

struct var:mx {
protected:
    void push(mx v);
    mx  &last();

    static mx parse_obj(cstr *start, type_t type);

    /// no longer will store compacted data
    static mx parse_arr(cstr *start, type_t type);

    static void skip_alpha(cstr *start);

    static mx parse_value(cstr *start, type_t type);

    static str parse_numeric(cstr * cursor, bool &floaty);

    /// \\ = \ ... \x = \x
    static mx parse_quoted(cstr *cursor, type_t type);

    static char ws(cstr *cursor);

    public:

    operator std::string();

    mx *get(str key);

    /// called from path::read<var>()
    static mx parse(cstr js, type_t type);

    str stringify() const;

    map<mx> items();

    array<mx> list();

    /// default constructor constructs map
    var();
    var(mx b);
    var(map<mx> m);

    template <typename T>
    var(const T b) : mx(alloc(&b)) { }

    // abstract between array and map based on the type
    template <typename KT>
    var &operator[](KT key) {
        /// if key is numeric, this must be an array
        type_t kt        = typeof(KT);
        type_t data_type = type();

        if constexpr (identical<KT, char*>() || identical<KT, const char*>() || identical<KT, str>()) {
            /// if key is something else just pass the mx to map
            map<mx>::mdata &dref = mx::mem->ref<map<mx>::mdata>();
            assert(&dref);
            mx skey = mx(key);
            return *(var*)&dref[skey];
        } else if (kt->traits & traits::integral) {    
            assert(data_type == typeof(array<mx>) || data_type == typeof(mx)); // array<mx> or mx
            mx *dref = mx::mem->data<mx>(0);
            assert(dref);
            size_t ix;
                 if (kt == typeof(i32)) ix = size_t(int(key));
            else if (kt == typeof(u32)) ix = size_t(int(key));
            else if (kt == typeof(i64)) ix = size_t(int(key));
            else if (kt == typeof(u64)) ix = size_t(int(key));
            else {
                console.fault("weird integral type given for indexing var: {0}\n", array<mx> { mx((symbol)kt->name) });
                ix = 0;
            }
            return *(var*)&dref[ix];
        } else if (kt == typeof(mx)) {
            /// not able to pass a integer via mx
            map<mx>::mdata &dref = mx::mem->ref<map<mx>::mdata>();
            assert(&dref);
            mx skey = mx(key);
            return *(var*)&dref[skey];
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

    liter<field<var>> begin() const;
    liter<field<var>>   end() const;
};

using FnFuture = lambda<void(mx)>;


template <typename T>
struct sp:mx {
    mx_object(sp, mx, T);
};

bool         chdir(std::string c);
memory* mem_symbol(ion::symbol cs, type_t ty, i64 id);
void *  mem_origin(memory *mem);
memory *   cstring(cstr cs, size_t len, size_t reserve, bool is_constant);

template <typename K, typename V>
pair<K,V> *hmap<K, V>::shared_lookup(K input) {
    pair* p = lookup(input);
    return p;
}

struct types {
    static inline hmap<ion::symbol, type_t> *type_map;
    static idata *lookup(str &);
    static void hash_type(type_t);
};

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

template <typename T>
idata *ident::for_type() {
    static idata *type;

    /// get string name of class (works on win, mac and linux)
    static auto    parse_fn = [&](std::string cn) -> cstr {
        std::string      st = is_win() ? "<"  : "T =";
        std::string      en = is_win() ? ">(" : "]";
        num		         p  = cn.find(st) + st.length();
        num              ln = cn.find(en, p) - p;
        std::string      nm = cn.substr(p, ln);
        auto             sp = nm.find(' ');
        std::string  s_name = (sp != std::string::npos) ? nm.substr(sp + 1) : nm;
        num ns = s_name.find("::");
        if (ns >= 0 && s_name.substr(0, ns) != "std")
            s_name = s_name.substr(ns + 2);
        cstr  name = util::copy(s_name.c_str());
        return name;
    };

    if (!type) {
        /// this check was to hide lambdas to prevent lambdas being generated on lambdas
        if constexpr (std::is_function_v<T>) { /// i dont believe we need this case now.
            memory         *mem = memory::raw_alloc(null, sizeof(idata), 1, 1);
            type                = (idata*)mem_origin(mem);
            type->src           = type;
            type->base_sz       = sizeof(T);
            type->name          = parse_fn(__PRETTY_FUNCTION__);
        } else if constexpr (!type_complete<T> || is_opaque<T>()) {
            /// minimal processing on 'opaque'; certain std design-time calls blow up the vulkan types
            memory         *mem = memory::raw_alloc(null, sizeof(idata), 1, 1);
            type                = (idata*)mem_origin(mem);
            type->src           = type;
            type->base_sz       = sizeof(T*);
            type->traits        = traits::opaque;
            type->name          = parse_fn(__PRETTY_FUNCTION__);
        } else if constexpr (std::is_const    <T>::value) {
            return ident::for_type<std::remove_const_t    <T>>();
        } else if constexpr (std::is_reference<T>::value) {
            return ident::for_type<std::remove_reference_t<T>>();
        } else if constexpr (std::is_pointer  <T>::value && sizeof(T) == 1) { /// char* is abstracted away to char because we vectorize them as char elements
            return ident::for_type<std::remove_pointer_t  <T>>(); /// for everything else, we set the pointer type to a primitive so we dont lose context
        } else {
            bool is_mx    = ion::is_mx<T>(); /// this one checks just for type == mx
            bool is_obj   = !is_mx && ion::inherits<mx, T>(); /// used in composer for assignment; will merge in is_mx when possible
            memory *mem   = memory::raw_alloc(null, sizeof(idata), 1, 1);
            type          = (idata*)mem_origin(mem);
            type->base_sz = sizeof(T);
            type->traits  = (is_primitive<T> () ? traits::primitive : 0) |
                            (is_integral <T> () ? traits::integral  : 0) |
                            (is_realistic<T> () ? traits::realistic : 0) | // if references radioshack catalog
                            (is_singleton<T> () ? traits::singleton : 0) |
                            (is_array    <T> () ? traits::array     : 0) |
                            (is_lambda   <T> () ? traits::lambda    : 0) |
                            (is_map      <T> () ? traits::map       : 0) |
                            (is_mx              ? traits::mx        : 0) |
                            (has_etype<T>::value ? traits::mx_enum  : 0) |
                            (is_obj             ? traits::mx_obj    : 0);

            type->name    = parse_fn(__PRETTY_FUNCTION__);

            if constexpr (true || registered<T>() || is_external<T>::value) {
                ops<T> *fn = (ops<T>*)ftable<T>();

                type->functions = (ops<none>*)fn;

                using _T = T;
                using  C = T;

                if constexpr (!ion::inherits<ex, T>()) {
                    if constexpr (has_addition<T>::value) {
                        fn->add = [](const T &a, const T &b, T &res) -> void {
                            res = T(a + b);
                        };
                    }

                    if constexpr (has_multiply<T>::value) {
                        fn->mul = [](const _T &a, const T &b, _T &res) -> void {
                            res = a * b;
                        };
                    }

                    if constexpr (has_multiply_scalar<T>::value) {
                        static_assert(std::is_same<decltype(std::declval<T>() * std::declval<float>()), T>::value, "Operator * does not return expected type");
                        fn->mul_scalar = [](const _T &a, const float b, _T &res) -> void {
                            res = a * b;
                        };
                    }
                }


                if constexpr (!std::is_array<T>::value && ion::inherits<mx, T>())
                    fn->set_memory = [](_T *dst, ion::memory *mem) -> void {
                        if constexpr (ion::inherits<mx, _T>())
                            if (dst->mem != mem) {
                                dst -> ~_T();
                                new (dst) _T(mem ? ion::hold(mem) : (ion::memory*)null);
                            }
                    };

                /// deprecate!
                if constexpr (!std::is_array<T>::value) {
                    fn->alloc_new  = []() -> T* {
                        return new T();
                    };
                    /// deprecate!
                    fn->del        = [](_T *ph, _T *ptr) { delete ptr; };

                    if constexpr (!ion::inherits<mx, T>())
                        fn->valloc = [](C *ph, _T *type, size_t count) -> _T* {
                            return (_T*)calloc(sizeof(_T), count);
                        };

                    /// we want an add, and div
                    if constexpr (has_mix<T>::value)
                        fn->mix = [](T *a, T *b, double v) -> T* {
                            if constexpr (has_mix<T>::value)
                                return new T(a->mix(*b, v));
                            return null;
                        };

                    fn->construct  = [](C *dst0, _T *dst) -> void { new (dst) _T(); };

                    fn->destruct   = [](C *dst0, _T *dst) -> void { dst -> ~_T(); };
                }


                fn->copy       = [](C *dst0, _T *dst, _T *src) -> void {
                    if constexpr (std::is_same_v<_T, std::filesystem::path>)
                        *dst = src->string();
                    else if constexpr (std::is_assignable_v<_T&, const _T&>)
                        *dst = *src;
                    else {
                        printf("cannnot assign in _copy() generic with type: %s\n", typeof(_T)->name);
                        exit(1);
                    }
                };

                if constexpr (has_bool<T>)
                    fn->boolean = [](C *dst0, _T *src) -> bool {
                        if constexpr (has_bool<_T>) 
                            return bool(*src); 
                        else 
                            return false; 
                    };
                
                fn->assign     = [](C *dst0, _T *dst, _T *src) -> void {
                    if constexpr (std::is_copy_constructible<_T>()) {
                        dst -> ~_T();
                        new (dst) _T(*src);
                    }
                };

                if constexpr (has_init<T>::value) {
                    fn->init = [](_T *src) -> void {
                        if constexpr (has_init<_T>::value) {
                            src->init();
                        }
                    };
                    type->traits |= traits::init;
                }

                if constexpr (registered_instance_to_string<_T>())
                    fn->to_string  = [](_T *src) -> memory* {
                        return src ? src->to_string() : null;
                    };
                else if constexpr (external_to_string<_T>())
                    fn->to_string = [](_T *src) -> memory* {
                        return src ? _to_string(src) : null;
                    };
                
                if constexpr (identical<T, mx>() || std::is_constructible<T, cstr>::value)
                    fn->from_string = [](_T *placeholder, cstr src) -> _T* {
                        if constexpr (identical<_T, mx>()) {
                            return new _T(src, typeof(char));
                        }
                        else if constexpr (std::is_constructible<_T, cstr>::value) {
                            return new _T(src);
                        } else
                            return null;
                    };
                else if constexpr (external_from_string<T>())
                    fn->from_string = [](_T *placeholder, cstr src) -> _T* { // deprecate the placeholders
                        return _from_string(placeholder, src);
                    };

                if constexpr (identical<T, mx>())
                    fn->from_string = [](_T *placeholder, cstr src) -> _T* {
                        if constexpr (identical<_T, mx>()) {
                            return new _T(src, typeof(char));
                        }
                        else if constexpr (std::is_constructible<_T, cstr>::value) {
                            return new _T(src);
                        } else
                            return null;
                    };
                else if constexpr (external_from_string<T>())
                    fn->from_string = [](_T *placeholder, cstr src) -> _T* { // deprecate the placeholders
                        return _from_string(placeholder, src);
                    };

                if constexpr (registered_compare<T>())
                    fn->compare = CompareFn<T>(T::_compare);
                
                if constexpr (registered_hash<T>())
                    fn->hash = HashFn<T>(T::hash);
                else if constexpr (external_hash<T>())
                    fn->hash = HashFn<T>(_hash);
                
                if constexpr (has_push<T>())
                    fn->push = PushFn(T::pushv);

                if constexpr (has_process<T>()) /// deprecate
                    fn->process = [](_T *src, struct memory *mem) -> void {
                        src->process(mem);
                    };
            }

            /// if the type contains enum, we want to link the enum back to its parent type
            /// if there is a parent type, we etype == schema->data
            if constexpr (has_etype<T>::value) {
                type_t etype = typeof(typename T::etype);
                etype->ref = type; ///
                etype->traits |= traits::enum_primitive;
                //type->ref = etype; // they reference each other
            }
            
            /// all mx classes have a parent_class; we use this to know the polymorphic chain
            if constexpr (!identical<mx, T>() && ion::inherits<mx, T>()) {
                type->parent = typeof(typename T::parent_class);
            }

            /// make a lambda calling lambda that utilizes runtime type conversion for debug shell, possible css users
            if constexpr (is_lambda<T>()) {
                using lcontainer = typename T::container;
                using traits     = lambda_traits<T>;
                using args_t     = typename traits::arg_types;
                using rtype      = typename traits::return_type;

                /// will only set the function if we have a convertible return type (or void) and args that dont use pointers/refs
                /// refs ARE possible.  pointers have far too many snags in this case though.
                /// for simplicity sake just none of either
                /// when you dont have this set on the type, the test for it failed; thus your lambda cannot be invoked through introspection
                if constexpr (allowed_types<args_t>::value && (identical<void, rtype>() || is_convertible<rtype, mx>()))
                    type->generic_lambda = new GenericLambda([type=type](void* ldata, array<str> &args) -> mx {
                        ///
                        lcontainer *data = (lcontainer*)ldata;

                        constexpr size_t n_args = std::tuple_size_v<args_t>;
                        if (args.len() != n_args)
                            throw std::runtime_error("arg count mismatch");

                        mx result;
                        #define ARG(N) \
                            using   T ## N = std::remove_const_t<std::remove_reference_t<std::tuple_element_t<N, args_t>>>;\
                            type_t  t ## N = typeof(T ## N);\
                            T ## N* a ## N = (T ## N *)(t ## N)->functions->from_string((none*)null, args[N].cs());
                        if constexpr (n_args == 0) {
                            if constexpr (identical<void, rtype>()) 
                                (*data->fn)();
                            else
                                result = (*data->fn)();
                        } else if constexpr (n_args == 1) {
                            ARG(0)
                            if constexpr (identical<void, rtype>())
                                (*data->fn)(*a0);
                            else
                                result = (*data->fn)(*a0);
                            delete a0;
                        } else if constexpr (n_args == 2) {
                            ARG(0) ARG(1)
                            if constexpr (identical<void, rtype>())
                                (*data->fn)(*a0, *a1);
                            else
                                result = (*data->fn)(*a0, *a1);
                            delete a0; delete a1;
                        } else if constexpr (n_args == 3) {
                            ARG(0) ARG(1) ARG(2)
                            if constexpr (identical<void, rtype>())
                                (*data->fn)(*a0, *a1, *a2);
                            else
                                result = (*data->fn)(*a0, *a1, *a2);
                            delete a0; delete a1; delete a2;
                        } else if constexpr (n_args == 4) {
                            ARG(0) ARG(1) ARG(2) ARG(3)
                            if constexpr (identical<void, rtype>())
                                (*data->fn)(*a0, *a1, *a2, *a3);
                            else
                                result = (*data->fn)(*a0, *a1, *a2, *a3);
                            delete a0; delete a1; delete a2; delete a3;
                        }
                        #undef ARG
                        return result;
                    });
            }
            if constexpr (registered_instance_meta<T>() || is_lambda<T>() || is_primitive<T>() || ion::inherits<ion::mx, T>() || is_hmap<T>::value || is_doubly<T>::value) {
                schema_populate(type, (T*)null);
            }

            ///
            if constexpr (registered_instance_meta<T>()) {
                static T *def = new T();
                type->meta    = new doubly<prop> { def->meta() }; /// make a reference to this data
                for (prop &p: *type->meta) {
                    p.offset     = size_t(p.member_addr) - size_t(def);
                    p.s_key      = new str(p.key->hold());
                    p.parent_type = type; /// we need to store what type it comes from, as we dont always have this context

                    /// this use-case is needed for user interfaces without css defaults
                    p.init_value = (p.type->functions && p.type->functions->alloc_new) ?
                        p.type->functions->alloc_new() : calloc64(1, p.type->base_sz);

                    if (p.type->functions) {
                        u8 *prop_dest = &(((u8*)def)[p.offset]);
                        if (p.type->traits & traits::mx_obj) {
                            mx *mx_data = (mx*)prop_dest;
                            p.type->functions->set_memory((none*)p.init_value, mx_data->mem);
                        } else {
                            p.type->functions->assign((none*)null, (none*)p.init_value, (none*)prop_dest);
                        }
                    }
                }
                delete def;
                prop_map     *pmap = new prop_map(size_t(16));
                doubly<prop> *meta = (doubly<prop>*)type->meta;
                for (ion::prop &prop: *meta) {
                    symbol prop_name = prop.name();
                    (*pmap)[prop_name] = &prop;
                }
                type->meta_map = pmap;
            }
            types::hash_type(type);
        }
    }
    return type_t(type);
}

template <typename T>
T *memory::data(size_t index) const {
    if constexpr (type_complete<T>) {
        type_t dtype = ident::for_type<T>();
        return (T*)typed_data(dtype, index);
    } else {
        assert(index == 0);
        return (T*)origin;
    }
}

template <typename T>
ops<T> *ftable() {
    static ops<T> gen;
    return (ops<T>*)&gen;
}

template <typename V>
void map<V>::print() {
    for (field<V> &f: *this) {
        str k = str(f.key.hold());
        str v = str(f.value.hold());
        console.log("key: {0}, value: {1}", { k, v });
    }
}

template <typename K, typename V>
V &hmap<K,V>::operator[](K input) {
    u64 k;
    
    /// lookup a pointer to V
    using ldata = typename doubly<pair>::ldata;
    ldata *b = null;
    V*  pv = lookup(input, &k, &b); /// this needs to lock in here
    if (pv) return *pv;
    
    /// default allocation on V; here we need constexpr switch on V type, res
    if        constexpr (std::is_integral<V>()) {
        *b += pair { input, V(0) };
    } else if constexpr (std::is_pointer<V>()) {
        *b += pair { input, V(null) };
    } else if constexpr (std::is_default_constructible<V>()) {
        *b += pair { input, V() };
    } else
        static_assert("hash V-type is not default initializable (and you Know it must be!)");
    
    V &result = b->last().value;
    return result;
}

struct test1 {
    static inline int test;
};

template <typename K, typename V>
V* hmap<K,V>::lookup(K input, u64 *pk, bucket **pbucket) const {
    u64 k = hash_index(input, data->sz); // todo: make sure this has uniformity
    if (pk) *pk  =   k;
    bucket &hist = (*data)[k];

    for (pair &p: hist)
        if (p.key == input) {
            if (pbucket) *pbucket = &hist;
            return &p.value;
        }
    if (pbucket) *pbucket = &hist;
    return null;
}

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
        console.log("path conversion to {0} is not implemented. do it, im sure it wont take long.", { mx(typeof(T)->name) });
        return false;
    }
    return true;
}

template<typename T, typename = void>
struct has_int_conversion : false_type {};

template<typename T>
struct has_int_conversion<T, std::void_t<decltype(static_cast<int>(std::declval<T>()))>> : true_type {};


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

    if constexpr (identical<T, array<u8>>()) {
        std::ifstream input(data->p, std::ios::binary);
        std::vector<char> buffer(std::istreambuf_iterator<char>(input), { });
        array<u8> res(buffer.size());
        memcpy(res.data, (u8*)buffer.data(), buffer.size());
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
u8*     property_find(void *origin, type_t type, str &name, prop *&rprop);
u8*     get_member_address(type_t type, raw_t data, str &name, prop *&rprop);
bool    get_bool(type_t type, raw_t data, str &name);
memory *get_string(type_t type, raw_t data, str &name);

}