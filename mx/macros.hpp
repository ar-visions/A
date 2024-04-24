#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

/// useful single statement error and print with fprintf stderr formatting, auto new-line?
#define errorf(TXT, ...) do { fprintf(stderr, TXT "\n", __VA_ARGS__); exit(1); } while (0);

constexpr int num_occurances(const char* cs, char c) {
    return cs[0] ? (cs[0] == c) + num_occurances(cs + 1, c) : 0; 
}

#define num_args(...) (ion::num_occurances(#__VA_ARGS__, ',') + 1)
#define str_args(...) (str(#__VA_ARGS__))

#define infinite_loop() { for (;;) { usleep(1000); } }

//#define typeof(T)   (ident::for_type<T>())
#define typeof(T)         (ident::for_type2<T>())
#define signatureof(CL,M,MPTR,MPTR_SZ) (ident::for_type2<M,CL>((void*)MPTR,MPTR_SZ))
//#define typeof(T) ((idata*)null)

/// for mx_declare
#define mx_basic_declare(C, B, D) \
    using parent_class  = B;\
    using context_class = C;\
    using intern = D;\
    static type_t intern_t;\
    D *data;\
    operator D &() const;\
    D  *operator->();\
    C       &operator=(const C &b);\
    static type_t  register_data();\
    static type_t  register_class();\
    C(memory*  mem);\
    C(const mx  &o);\
    C();\
    C(const D  &data);\

/// for mx_implement
#define mx_basic_implement(C, B, D) \
    type_t C::intern_t  = typeof(D);\
    D *C::operator-> () { return  data; }\
    C &C::operator=(const C &b) {\
        if(mx::mem != b.mx::mem) {\
            ion::drop(mx::mem);\
            mx::mem = ion::hold(b.mx::mem);\
            data = (intern*)mx::get<intern>(0);\
        }\
        return *this;\
    }\
    C::operator D &() const {\
        return *(D *)data;\
    }\
    type_t  C::register_data()    { return typeof(D); }\
    type_t  C::register_class()   { return typeof(C); }\
    C::C(memory*   mem)  : B(mem), data(ion::mdata<D>(mem, 0)) { }\
    C::C(const mx &o)    : C(ion::hold(o)) { }\
    C::C()               : C(mx::alloc<C>()) { }\
    C::C(const D  &data) : C(mx::alloc<C>((void*)&data)) { }\

/// for mx_object
#define mx_basic_object(C, B, D) \
    using parent_class  = B;\
    using context_class = C;\
    using intern = D;\
    static inline type_t intern_t;\
    D *data;\
    operator D &() const {\
        return *(D *)data;\
    }\
    D  &operator *() { return *data; }\
    D *operator-> () { return  data; }\
    C &operator=(const C &b) {\
        if(mx::mem != b.mx::mem) {\
            ion::drop(mx::mem);\
            mx::mem = ion::hold(b.mx::mem);\
            data = (intern*)mx::get<intern>(0);\
        }\
        return *this;\
    }\
    static type_t  register_data()    { return typeof(D); }\
    static type_t  register_class()   { return typeof(C); }\
    C(memory*   mem)  : B(mem), data(ion::mdata<D>(mem, 0)) { }\
    C(mx o)           : C(ion::hold(o)) { }\
    C()               : C(mx::alloc<C>()) { }\
    C(const D  &data) : C(mx::alloc<C>((void*)&data)) { }\

#define mx_declare(C, B, D) \
    mx_basic_declare(C, B, D)

#define mx_implement(C, B, D) \
    mx_basic_implement(C, B, D)

#define mx_object(C, B, D) \
    mx_basic_object(C, B, D)

#define mx_basic(C) mx_object(C, mx, M)

#define enums(C,D,...)\
    struct C:ex {\
        enum etype { __VA_ARGS__ };\
        enum etype&    value;\
        using parent_class  = ex;\
        using context_class = C;\
        using intern = etype;\
        inline static type_t intern_t;\
        static type_t  register_data()    { return typeof(etype); }\
        static type_t  register_class()   { return typeof(C); }\
        static memory* lookup(ion::symbol sym) { return typeof(C)->lookup(sym); }\
        static memory* lookup(i64     id) { return typeof(C)->lookup(id);  }\
        static doubly &symbols() { return typeof(C)->symbols->list; }\
        inline static const int count = num_args(__VA_ARGS__);\
        inline static const str raw   = str_args(__VA_ARGS__);\
        str name() { return (char*)symbol(); }\
        struct memory *to_string() { return typeof(C)->lookup(i64(value)); }\
        C(enum etype t = etype::D):ex(initialize(this,             t, (ion::symbol)raw.cs(), typeof(C)), this), value(*get<enum etype>()) { }\
        C(size_t     t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(*get<enum etype>()) { }\
        C(int        t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(*get<enum etype>()) { }\
        C(str sraw):C(ex::convert(sraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
        C(mx  mraw):C(ex::convert(mraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
        C(ion::symbol sym):C(ex::convert(sym, (ion::symbol)C::raw.cs(), (C*)null)) { }\
        C(memory* mem):C(mx(mem)) { }\
        operator etype() const { return value; }\
        C &operator=(const C &b) {\
            if(mx::mem != b.mx::mem) {\
                ion::drop(mx::mem);\
                mx::mem = ion::hold(b.mx::mem);\
                value = b.value;\
            }\
            return *this;\
        }\
        bool    operator== (const enum etype &v) const { return value == v; }\
        bool    operator== (ion::symbol v) const {\
            if (!mem && !v)\
                return true;\
            memory *m = lookup(v);\
            return (int)m->id == (int)value;\
        }\
        bool    operator!= (const enum etype &v) const { return value != v; }\
        bool    operator>  (const C &b)          const { return value >  b.value; }\
        bool    operator<  (const C &b)          const { return value <  b.value; }\
        bool    operator>= (const C &b)          const { return value >= b.value; }\
        bool    operator<= (const C &b)          const { return value <= b.value; }\
        explicit operator int() const   { return int(value); }\
        explicit operator i64() const   { return i64(value); }\
        operator str()         { return symbol(); }\
    };\

#define enums_declare(C,D,W,...)\
    struct W##Wrapper;\
    struct C:ex {\
        enum etype { __VA_ARGS__ };\
        enum etype&    value;\
        inline static type_t intern_t;\
        static type_t  register_data();\
        static type_t  register_class();\
        static memory* lookup(symbol sym) { return typeof(C)->lookup(sym); }\
        static memory* lookup(i64     id) { return typeof(C)->lookup(id);  }\
        static doubly &symbols() { return typeof(C)->symbols->list; }\
        inline static const int count = num_args(__VA_ARGS__);\
        inline static const str raw   = str_args(__VA_ARGS__);\
        str name();\
        memory *to_string();\
        C(enum etype t = etype::D);\
        C(size_t     t);\
        C(int        t);\
        C(str sraw);\
        C(mx  mraw);\
        C(ion::symbol sym);\
        C(memory* mem);\
        operator etype() const;\
        C&      operator=  (const C &b);\
        bool    operator== (const enum etype &v)  const;\
        bool    operator== (ion::symbol v)        const;\
        bool    operator!= (const enum etype &v)  const;\
        bool    operator>  (const C &b)           const;\
        bool    operator<  (const C &b)           const;\
        bool    operator>= (const C &b)           const;\
        bool    operator<= (const C &b)           const;\
        explicit operator int() const;\
        explicit operator i64() const;\
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
type_t  C::register_data()    { return typeof(etype); }\
type_t  C::register_class()   { return typeof(C); }\
str C::name() { return (char*)symbol(); }\
memory *C::to_string() { return typeof(C)->lookup(i64(value)); }\
C::C(enum etype t)           :ex(initialize(this,             t, (ion::symbol)raw.cs(), typeof(C)), this), value(*get<enum etype>()) { }\
C::C(size_t     t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(*get<enum etype>()) { }\
C::C(int        t)           :ex(initialize(this, (enum etype)t, (ion::symbol)raw.cs(), typeof(C)), this), value(*get<enum etype>()) { }\
C::C(str sraw):C(ex::convert(sraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
C::C(mx  mraw):C(ex::convert(mraw, (ion::symbol)C::raw.cs(), (C*)null)) { }\
C::C(ion::symbol sym):C(ex::convert(sym, (ion::symbol)C::raw.cs(), (C*)null)) { }\
C::C(memory* mem):C(mx(mem)) { }\
C::operator etype() const { return value; }\
C &C::operator=(const C &b) {\
    if(mx::mem != b.mx::mem) {\
        ion::drop(mx::mem);\
        mx::mem = ion::hold(b.mx::mem);\
        value = b.value;\
    }\
    return *this;\
}\
bool    C::operator== (const enum etype &v) const { return value == v; }\
bool    C::operator== (ion::symbol v) const {\
    if (!mem && !v)\
        return true;\
    memory *m = lookup(v);\
    return (int)m->id == (int)value;\
}\
bool    C::operator!= (const enum etype &v) const { return value != v; }\
bool    C::operator>  (const C &b) const       { return value >  b.value; }\
bool    C::operator<  (const C &b) const      { return value <  b.value; }\
bool    C::operator>= (const C &b) const      { return value >= b.value; }\
bool    C::operator<= (const C &b) const      { return value <= b.value; }\
C::operator int() const   { return int(value); }\
C::operator i64() const   { return i64(value); }\
C::operator str() const   { return symbol(); }\
C::C(const W##Wrapper &r):C((enum etype)(int)r.value) { }\
W##Wrapper C::convert() {\
    return W##Wrapper((W)(int)value);\
}









