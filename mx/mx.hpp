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

/// none is generic context, its internal is void.  so when we define origin it must be of the internal pointer
struct none {
    using intern = void;
};

/// everybody has to do this, because you cant just break anywhere
/// this lets us define where we put these so we can find them later
#define debug_break() do { int test = 0; } while(0);

struct idata;
struct mx;
struct memory;
struct MX;

using raw_t = void*;
using type_t = idata *;

void*     calloc64(size_t count, size_t size);
void        free64(void* ptr);
memory*       hold(memory *mem);
memory*       drop(memory *mem);
memory*       hold(const MX &o);
memory*       drop(const MX &o);
memory* mem_symbol(symbol cs, type_t ty, i64 id);
void*   mem_origin(memory *mem);
memory*    cstring(cstr cs, size_t len = UINT64_MAX, size_t reserve = 0, bool sym = false);

u64     hash_value(const mx &key);
u64     hash_index(const mx &key, size_t mod);

template <typename T> T *mdata(memory *mem, size_t index);

struct doubly;
struct size;

/// mx type check must look for MX<CTX> too; the allocation wont work otherwise
template<typename T>
struct Memory {
    size_t              refs;
    u64                 attrs;
    type_t              type;
    size_t              count, reserve;
    size               *shape; // if null, then, its just 1 dim, of count.  otherwise this is the 'shape' of the data and strides through according
    doubly             *atts;
    size_t              id;
    typename T::intern* origin;
};


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

template <typename T, typename = void> struct registered_instance_to_string : false_type { };
template <typename T>                  struct registered_instance_to_string<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().to_string()), memory *>>> : true_type { };

template <typename> struct is_external   : false_type { };
template <typename> struct is_opaque     : false_type { };
template <typename> struct is_singleton  : false_type { };

template <typename T>
constexpr bool is_mx();

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

/// just-works and please-dont-break macros
#include <mx/macros.hpp>

/// design-time templates
#include <mx/design.hpp>

/// primitive to/from string here
memory *_to_string(cstr data);
size_t  _hash(cstr data, size_t count);

static inline void yield() {
#ifdef _WIN32
    SwitchToThread(); // turns out you can. lol.
#else
    sched_yield();
#endif
}
// this doesnt work.  replacing it with std::initializer_list<T> works; it doesnt match constructors, and it worked for years before. decided not to work once i rewrote a lot of mx
//template <typename T> using initial = std::initializer_list<T>;

/// this is memory-specific now
struct item {
    struct item *next;
    struct item *prev;
    memory*      mem;
    u64          hash; /// keep hash here; use as a filter in iteration

    /// return the wrapped item memory
    static item *container(memory *&mem) {
        return (item *)((symbol)&mem - sizeof(struct item*) * 2);
    }
    template <typename T>
    T &ref() const;

};

/// iterator unique for doubly
struct liter_item {
    item* cur;
    liter_item(item *cur) : cur(cur) { }
    ///
    liter_item& operator++() { cur = cur->next; return *this; }
    liter_item& operator--() { cur = cur->prev; return *this; }

    item& operator *     () const;
          operator item &() const;

    bool operator==  (const liter_item& b) const { return cur == b.cur; }
    bool operator!=  (const liter_item& b) const { return cur != b.cur; }
};

/// doubly is partnered with hashmap, so an iterator filters by hash
struct liter_item_hash:liter_item {
    u64   hash;
    using LT = liter_item;
    liter_item_hash(item *cur, u64 hash) : liter_item(cur), hash(hash) {
        while (LT::cur && (LT::cur->hash != hash))
            LT::cur = LT::cur->next;
    }

    ///
    liter_item_hash& operator++() {
        do { LT::cur = LT::cur->next; } while (LT::cur && LT::cur->hash != hash);
        return *this;
    }
    liter_item_hash& operator--() {
        do { LT::cur = LT::cur->prev; } while (LT::cur && LT::cur->hash != hash);
        return *this;
    }
};

/// iterator unique for doubly
template <typename T>
struct liter {
    item* cur;
    ///
    liter& operator++() { cur = cur->next; return *this; }
    liter& operator--() { cur = cur->prev; return *this; }

    T& operator *  () const;
       operator T& () const;

    bool operator==  (const liter& b) const { return cur == b.cur; }
    bool operator!=  (const liter& b) const { return cur != b.cur; }
};

template <typename T>
struct liter_hash:liter<T> {
    u64   hash;
    using LT = liter<T>;
    ///
    liter_hash& operator++() {
        do { LT::cur = LT::cur->next; } while (LT::cur && LT::cur->hash != hash);
        return *this;
    }
    liter_hash& operator--() {
        do { LT::cur = LT::cur->prev; } while (LT::cur && LT::cur->hash != hash);
        return *this;
    }
};


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

    bool operator==(const iter& i) const { return start == i.start && index == i.index; }
    bool operator!=(const iter& i) const { return !(*this == i); }
};

/// removing template from doubly, map, and array

/// simplifying pair / fields declaration; we will go with this castable architecture for less generation

/// if we need context on the value, we dont have that once the memory is given
///
struct mx;

struct MX {
    memory *mem;
    MX(memory* mem) : mem(mem) { }
    MX(null_t n);
    MX(symbol v);
    MX(bool   v);
    MX(u8     v);
    MX(i8     v);
    MX(u16    v);
    MX(i16    v);
    MX(u32    v);
    MX(i32    v);
    MX(u64    v);
    MX(i64    v);
    MX(r32    v);
    MX(r64    v);
    template <typename T>
    T &ref() const;
    operator mx &() const;
};

struct field {
    MX key;
    MX value;

    template <typename V>
    operator V &() const;
};

struct ldata {
    size_t   icount;
    item    *ifirst, *ilast;
    type_t   type;

    ~ldata() {
        item* d = ifirst;
        while(d) {
            item* dn = d->next;
            delete d;
            d = dn;
        }
        icount = 0;
        ifirst = ilast = 0;
    }

    operator  bool() const { return ifirst != null; }
    bool operator!() const { return ifirst == null; }

    void shift(mx* prev_first = null);

    /// push by value, return its new instance
    template <typename T>
    T &push(const T &v, u64 hash = 0);

    /// push and return default instance
    template <typename T>
    T &push();

    /// 
    item *get(num index) const {
        if (index == icount)
            return ilast;
        else if (index == 0)
            return ifirst;
        item *i;
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
    template <typename T>
    item *insert(num before, T &data, u64 hash = 0);

    size_t    len() { return icount; }
    size_t length() { return icount; }

    bool remove(item *i) {
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
        item *i = get(index);
        return remove(i);
    }

    bool remove(num index, num count) {
        item *i = get(index);
        for (int v = 0; v < count; v++) {
            if (!remove(i))
                return false;
            i = i->next;
        }
        return count > 0;
    }

    item   *first_item() const;
    item   * last_item() const;

    mx     &  first_mx() const;
    mx     &   last_mx() const;

    void clear() {
        while (icount)
            remove(-1);
    }

    size_t count() const { return icount; }

    template <typename T>
    T     &first() const;
    
    template <typename T>
    T      &last() const;
    
    void pop(mx *prev = null);

    template <typename T>
    T &data(num ix) const;

    template <typename T>
    T   &operator+=   (const T& v) { return push(v); }

    bool operator-=   (num i) { return remove(i); }

    /// better to use the .elements<T> on generics; we cannot have a begin() on something without a template arg
    template <typename T>
    struct literable {
        item *ifirst, *ilast;
        literable(item *ifirst, item *ilast) :
            ifirst(ifirst), ilast(ilast) { }
        liter<T> begin() const { return liter<T>{ ifirst }; }
        liter<T>   end() const { return liter<T>{ null }; }
    };

    template <typename T>
    struct literable_hash:literable<T> {
        u64 hash;
        literable_hash(item *ifirst, item *ilast, u64 hash) :
            literable<T>(ifirst, ilast), hash(hash) { }
        liter_hash<T> begin() const { return liter_hash<T>{ literable<T>::ifirst }; }
        liter_hash<T>   end() const { return liter_hash<T>{ null }; }
    };

    struct literable_items {
        item *ifirst, *ilast;
        literable_items(item *ifirst, item *ilast) : ifirst(ifirst), ilast(ilast) { }
        liter_item begin() const { return liter_item{ ifirst }; }
        liter_item   end() const { return liter_item{ null }; }
    };

    struct literable_items_hash:literable_items {
        u64 hash;
        literable_items_hash(item *ifirst, item *ilast, u64 hash) :
            literable_items(ifirst, ilast), hash(hash) { }
        liter_item_hash begin() const { return liter_item_hash { literable_items::ifirst, hash }; }
        liter_item_hash   end() const { return liter_item_hash { null, 0 }; }
    };

    template <typename T>
    literable<T> elements() const { return literable<T> { ifirst, ilast }; }

    template <typename T>
    literable_hash<T> elements(u64 hash) const { return literable_hash<T>    { ifirst, ilast, hash }; }

    literable_items      items()         const { return literable_items      { ifirst, ilast }; }
    literable_items_hash items(u64 hash) const { return literable_items_hash { ifirst, ilast, hash }; }
};

/// maybe just switch to identical ?
template <typename T> struct is_array  : false_type {};
template <typename T> struct is_typed_array : false_type {};
template <typename T> struct is_map    : false_type {};
template <typename T> struct is_hmap   : false_type {};
template <typename T> struct is_doubly : false_type {};

struct doubly {
    using intern = ldata;
    using parent_class = none;

    memory *mem;
    ldata *data;
    doubly(memory* mem);
    doubly(); // ^ memory* is base constructor

    template <typename T>
    doubly(std::initializer_list<T> a) : doubly() {
        for (auto &v: a)
            data->push(v);
    }

    static type_t register_class();
    static type_t register_data();

    template <typename T>
    ldata::literable<T> elements() const { return data->elements<T>(); }

    template <typename T>
    ldata::literable_hash<T> elements(u64 hash) const { return data->elements<T>(hash); }

    ldata::literable_items items() const { return data->items(); }

    ldata::literable_items_hash items(u64 hash) const { return data->items(hash); }

    operator bool() const { return bool(*data); }

    template <typename T>
    T &get(num ix) const { return data->data<T>(ix); }

    template <typename T>
    T   &operator+=(const T &v) { return data->push(v); }

    bool operator-=(num i)      { return data->remove(i); }

    size_t len() { return data->count(); }

    doubly& operator=(const doubly &b) {
             this -> ~doubly( ); /// destruct this
        new (this)    doubly(b); /// copy construct into this, from b; lets us reset refs
        return *this;
    }

    ldata *operator->() const {
        return data;
    }

     doubly(const doubly &ref);

    ~doubly();
};

template <> struct is_doubly<doubly> : true_type { };

using bucket = ldata;

struct hmdata {
    bucket *h_pairs;
    size_t  sz;

    /// i'll give you a million quid, or, This Bucket.
    bucket &operator[](u64 k) {
        assert(sz > 0);
        return h_pairs[k];
    }

    void push(const field &f) {
        u64 k = hash_index(f.key, sz); // if sz is 1 for non-hash use-cases, that would be a reduction
        ldata &b = (*this)[k];
        b.push(f);
    }
    
    ~hmdata() {
        for (size_t  i = 0; i < sz; i++)
            h_pairs[i]. ~ bucket(); // data destructs but does not construct and thats safe to do
        free64(h_pairs);
    }
};

/// would be interesting to allocate for instances of 'this'

struct hashmap {
    using intern = hmdata; // MX<context-here> not <data-here>
    using parent_class = none;
    memory *mem;
    hmdata *data;

    hashmap(size_t    sz = 1);
    hashmap(std::initializer_list<field> a) : hashmap(a.size() > 0 ? a.size() : 1) {
        for (auto &v: a)
            data->push(v);
    }

    hashmap& operator=(hashmap &a);

    ion::item  *item (const mx &key, bucket **list = null, u64 *phash = null) const;
    field &fetch(const mx &key);
    field &fetch(cstr key);

    mx    &value(const mx &key);
    void   set  (const mx &key, const mx &value);

    template <typename V>
    V &get(const mx &k);

    mx lookup(const mx &k) const;

    bool contains(cstr str) const;
    bool contains(const mx& key) const;

    bool remove(const mx &key);

    mx &operator[](const mx &key);

    size_t    len() { return data->sz; }
    operator bool() { return data->sz > 0; }
};

template <> struct is_hmap<hashmap> : true_type { };

struct prop;
using prop_map = hashmap;

struct symbol_data {
    hashmap   djb2 { 32 }; // u64 -> memory*
    hashmap   ids  { 32 }; // i64 -> memory*
    doubly list { }; // memory*
};

static inline void sleep(u64 u) {
    #ifdef _WIN32
        Sleep(u);
    #else
        usleep(useconds_t(u) * 1000);
    #endif
}

/// why do we even allow opaque types to be externally registered?
/// when a type is impl() we may not have a lambda table?
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
        primitive_array = 32768
    };
};

bool chdir(std::string c);

struct ident;

template <typename T, const size_t SZ>
struct buffer {
    T      values[SZ];
    size_t count;
};

struct size:buffer<num, 16> {
    using base = buffer<num, 16>;

    size(num        sz = 0);
    size(null_t           );
    size(size_t         sz);
    size(i8             sz);
    size(u8             sz);
    size(i16            sz);
    size(u16            sz);
    size(i32            sz);
    size(u32            sz);

    size(std::initializer_list<num> args);

    size_t    x() const;
    size_t    y() const;
    size_t    z() const;
    size_t    w() const;
    num    area() const;
    size_t dims() const;

    void assert_index(const size &b) const;

    bool operator==(size_t b) const;
    bool operator!=(size_t b) const;
    bool operator==(size &sb) const;
    bool operator!=(size &sb) const;
    
    void   operator++(int);
    void   operator--(int);
    size  &operator+=(size_t sz);
    size  &operator-=(size_t sz);

    num &operator[](size_t index) const;

    size &zero();

    template <typename T>
    size_t operator()(T *addr, const size &index) {
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

    size &operator=(i8   i);
    size &operator=(u8   i);
    size &operator=(i16  i);
    size &operator=(u16  i);
    size &operator=(i32  i);
    size &operator=(u32  i);
    size &operator=(i64  i);
    size &operator=(u64  i);

    size &operator=(const size b);

    size_t index_value(const size &index) const;
};

template <typename T>
memory *talloc(size_t count = 1, size_t reserve = 0);

struct hash { u64 value; hash(u64 v) : value(v) { } operator u64() { return value; } };

struct str;
struct array;

#undef min
#undef max

struct util {
    static cstr copy(symbol cs) {
        size_t   sz = strlen(cs) + 1;
        cstr result = cstr(malloc(sz));
        memcpy(result, cs, sz - 1);
        result[sz - 1] = 0;
        return result;
    }
};

struct math {
    template <typename T>
    static inline T min (T a, T b)                             { return a <= b ? a : b; }
    template <typename T> static inline T max  (T a, T b)      { return a >= b ? a : b; }
    template <typename T> static inline T clamp(T v, T m, T x) { return v >= x ? x : x < m ? m : v; }
    template <typename T> static inline T round(T v)           { return std::round(v);  }
};

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

struct str;

static constexpr uint64_t FNV_PRIME    = 0x100000001b3;
static constexpr uint64_t OFFSET_BASIS = 0xcbf29ce484222325;

#include <mx/meta.hpp>


template <typename T>
T &MX::ref() const {
    return *mem->get<T>(0);
}

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

struct size;

template <typename T>
T &item::ref() const {
    return *mem->get<T>(0);
}

template <typename T>
T &ldata::push(const T &v, u64 hash) {
    if constexpr (identical<T, field>()) {
        /// get hash from T
        if (hash == 0) {
            field &fv = *(field*)&v;
            mx mx_key = mx(hold(fv.key));
            hash = mx_key.hash();
        }
    }
    item *plast = ilast;
    ilast = new item { null, ilast,
        memory::alloc(typeof(T), 1, 1, (raw_t)&v), hash };
    ///
    (!ifirst) ? 
    ( ifirst      = ilast) : 
    ( plast->next = ilast);
    ///
    icount++;
    return *ilast->mem->get<T>(0);
}

template <typename T>
T &ldata::push() {
    item *plast = ilast;
        ilast = new item { null, ilast, memory::alloc(typeof(T)) }; /// default-construction on data
    ///
    (!ifirst) ? 
    ( ifirst      = ilast) : 
    ( plast->next = ilast);
    ///
    icount++;
    return ilast->ref<T>();
}

template <typename T>
T   &ldata::first() const { return ifirst->ref<T>(); }

template <typename T>
T   &ldata::last() const { return ilast->ref<T>(); }

template <typename T>
T &ldata::data(num ix) const {
    assert(ix >= 0 && ix < num(icount));
    return ldata::get(ix)->ref<T>();
}


template <typename T>
item *ldata::insert(num before, T &data, u64 hash) {
    item *i;
    if (before == icount) {
        i = new item { null, ilast, memory::alloc(typeof(T), 1, 1, &data), hash };
        if (ilast)
            ilast->next = i;
        ilast = i;
        if (icount == 0)
            ifirst = i;
    } else {
        item *s = get(before);
        i = new item { s, s->prev, memory::alloc(typeof(T), 1, 1, &data), hash };
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

template <typename T>
T& liter<T>::operator *() const { return *cur->mem->get<T>(0); }

template <typename T>
liter<T>::operator T& () const { return *cur->mem->get<T>(0); }

//template <typename K>
//K &field::  key() { return *k->get<K>(); }

//template <typename V>
//V &field::value() { return *v->get<V>(); }

using arg = field;
using ax  = Array<arg>;


template <typename V>
field::operator V &() const {
    assert(value.mem);
    assert(typeof(V) == value.mem->type);
    return value.ref<V>();
}

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
        const array& args)
{
    using traits = lambda_traits<Lambda>;
    return lambda(convert_str<std::tuple_element_t<Is, typename traits::arg_types>>(args.get<str>(Is))...);
}

template <typename Lambda>
auto invoke(const Lambda& lambda, const array& args) {
    using traits = lambda_traits<Lambda>;
    constexpr size_t numArgs = std::tuple_size_v<typename traits::arg_types>;

    if (args.len() != numArgs) {
        throw std::runtime_error("Incorrect number of arguments.");
    }
    /// 
    return invokeImpl(lambda, std::make_index_sequence<numArgs>{}, args);
}

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

/// probably dont need V mentioned in here; maps can store anything by key
struct map:mx {
    inline static const size_t default_hash_size = 64;

    struct mdata {
        doubly fields;
        hashmap  *hash_map;

        /// boolean operator for key
        bool operator()(const mx &key);

        field &fetch(const mx &k);
        field &first();
        field & last();
        size_t count();
        size_t   len();
        size_t  count(const mx &k);

        size_t count(cstr cs);

        field *lookup(const mx &k) const;

        bool remove(const mx &k);

        /// should not CONVERT. this is just copying the key used; should assert too.
        array keys();

        // value could be item memory reference
        /*template <typename V>
        array values() {
            array res { typeof(V), fields->len() };
            for (field &f:fields.elements<field>()) {
                assert(f.value.mem->type == typeof(V));
                if constexpr (ion::inherits<mx, V>())
                    res.push(ion::hold(f.value)); /// this calls the mx constructor which performs an increment to the reference count
                else
                    res.push(f.value.mem->get<V>(0));
            }
            return res;
        }*/

        ldata::literable<field> elements() const;

        mx &operator[](const mx &key);

        operator bool();
    };

    static map parse(int argc, cstr *argv, map &def);

    ldata::literable<field> fields() const;

    template <typename K>
    array keys() {
        return data->keys(); /// cannot pass K without a phony default arg.
    }

    size_t len();
    
    void print();

    /// props bypass dependency with this operator returning a list of field, which is supported by mx (map would not be)
    operator   doubly &();
    bool       operator()(mx key);

    bool contains(mx key);
    
    operator bool() const;

    template <typename V>
    V &get(const mx &k) const {
        field &f = data->fetch(k);
        assert(f.value.mem);
        assert(f.value.mem->type == typeof(V));
        return f.value.ref<V>();
    }

    void set(const mx &k, const mx &v) const {
        field &f = data->fetch(k);
        f.value = v;
    }

    mx_object(map, mx, mdata);

    /// when a size is specified to map, it engages hash map mode
    map(size sz);

    map(size_t sz);

    map(std::initializer_list<field> args);

    map(const array &arr);

    map(const doubly &li);
    
    mx &operator[](const mx &k);

    template <typename K>
    mx &operator[](K k) {
        field &f = data->fetch(k);
        return f.value;
    }
};

template <> struct is_map  <map>   : true_type { };

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

    memory *string() const {
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
    static void _print(const str &st, const array &ar, const states<option> opts) {
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
    inline void log(mx m, const Array<mx> &ar = {}) {
        str st = m.to_string();
        _print(st, ar, { });
    }

    void test(const bool cond, mx templ = {}, const Array<mx> &ar = {}) {
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

    inline void fault(mx m, Array<mx> ar = Array<mx> { }) { str s = m.to_string(); _print(s, ar, { err }); brexit(); }
    inline void error(mx m, Array<mx> ar = Array<mx> { }) { str s = m.to_string(); _print(s, ar, { err }); brexit(); }
    inline void print(mx m, Array<mx> ar = Array<mx> { }) { str s = m.to_string(); _print(s, ar, { append }); }
    inline void debug(mx m, Array<mx> ar = Array<mx> { }) { str s = m.to_string(); _print(s, ar, { append }); }
};

/// define a logger for global use; 'console' can certainly be used as delegate in mx or node, for added context
extern logger console;

/// use char as base.
struct path:mx {
    inline static std::error_code ec;

    using Fn = lambda<void(path)>;

    enums(option, recursion,
         recursion, no_sym_links, no_hidden, use_git_ignores);

    enums(op, none,
         none, deleted, modified, created);

    static path cwd();

    struct M {
        fs::path p;
        memory* to_string();
    };

    mx_basic(path);

    path(str      s);
    path(symbol  cs);

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
    path remove_filename();
    bool    has_filename() const;
    path            link() const;
    bool         is_file() const;
    char             *cs() const;
    str             ext () const;
    str             ext4() const;
    path            file() const;
    bool     copy(path to) const;
    
    path &assert_file();

    /// create an alias; if successful return its location
    path  link(path alias) const;

    /// operators
    operator        bool()         const;
    operator         str()         const;
    path          parent()         const;
    path operator / (path       s) const;
    path operator / (symbol     s) const;
    path operator / (const str& s) const;
    path relative   (path    from) const;
    bool  operator==(path&      b) const;
    bool  operator!=(path&      b) const;
    bool  operator==(symbol     b) const;
    bool  operator!=(symbol     b) const;
    operator str();
    operator cstr() const;

    /// methods
    static path uid(path b);
    static path  format(str t, array args);
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

using FnFuture = lambda<void(mx)>;

template <typename T>
struct sp:mx {
    mx_object(sp, mx, T);
};

bool         chdir(std::string c);
memory* mem_symbol(ion::symbol cs, type_t ty, i64 id);
void *  mem_origin(memory *mem);
memory *   cstring(cstr cs, size_t len, size_t reserve, bool is_constant);


template <typename T>
T *memory::get(size_t index) const {
    if constexpr (type_complete<T>) {
        type_t dtype = typeof(T);
        return (T*)typed_data(dtype, index);
    } else {
        assert(index == 0);
        return (T*)origin;
    }
}

template <typename T>
T *memory::set(size_t index, const T &v) {
    if constexpr (type_complete<T>) {
        type_t dtype = typeof(T);
        *(T*)typed_data(dtype, index) = v;
        return (T*)origin + index;
    } else {
        assert(index == 0);
        return (T*)origin + index;
    }
}

template <typename V>
V &hashmap::get(const mx &k) {
    const mx key(k);
    ion::field &f = fetch(key);
    return f.value.ref<V>();
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
u8*     property_find(void *origin, type_t type, str &name, prop *&rprop);
u8*     get_member_address(type_t type, raw_t data, str &name, prop *&rprop);
bool    get_bool(type_t type, raw_t data, str &name);
memory *get_string(type_t type, raw_t data, str &name);

}