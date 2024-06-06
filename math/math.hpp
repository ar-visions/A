#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <mx/mx.hpp>

namespace ion {

// this gets the type from the x value on a type T
template<typename T>
using vtype = typename std::decay<decltype(std::declval<T>().x)>::type;

template <typename T> inline T cross    (const T &a, const T &b) { return a.cross(b); }
template <typename T> inline T normalize(const T &a)             { return a.normalize(); }
template <typename T> inline vtype<T> dot(const T &a, const T &b) { return a.dot(b); }

#define vec2_decl(T, t) \
struct vec2##t { \
    T x, y; \
    vec2##t(); \
    vec2##t(const vec2##t &v); \
    vec2##t(T x); \
    vec2##t(T x, T y); \
    vec2##t(str s); \
    vec2##t(cstr s); \
    operator bool() const; \
    const T &operator[](int i) const; \
    operator object() const; \
    vec2##t&operator= (const vec2##t &v); \
    vec2##t operator+ (const vec2##t &b) const; \
    vec2##t operator- (const vec2##t &b) const; \
    vec2##t operator* (const vec2##t &b) const; \
    vec2##t operator/ (const vec2##t &b) const; \
    vec2##t operator* (const T v) const; \
    vec2##t operator/ (const T v) const; \
    vec2##t &operator += (vec2##t v); \
    vec2##t &operator -= (vec2##t v); \
    vec2##t &operator *= (vec2##t v); \
    vec2##t &operator /= (const vec2##t &v); \
    vec2##t &operator *= (T v); \
    vec2##t &operator /= (T v); \
    vec2##t mix(const vec2##t &b, double v) const; \
    T dot() const;\
    T length() const;\
    T dot(const vec2##t &b) const;\
}; \

#define vec3_decl(T, t) \
struct vec3##t { \
    T x, y, z; \
    vec3##t(); \
    vec3##t(str s); \
    vec3##t(cstr s); \
    vec3##t(T x); \
    vec3##t(T x, T y, T z); \
    operator bool()                 const; \
    const T &operator[](int i)      const; \
    operator object()               const; \
    vec3##t&operator= (const vec3##t &v); \
    vec3##t operator+ (const vec3##t &b) const; \
    vec3##t operator- (const vec3##t &b) const; \
    vec3##t operator* (const vec3##t &b) const; \
    vec3##t operator/ (const vec3##t &b) const; \
    vec3##t operator* (const T v) const; \
    vec3##t operator/ (const T v) const; \
    vec3##t &operator += (const vec3##t &v); \
    vec3##t &operator -= (const vec3##t &v); \
    vec3##t &operator *= (const vec3##t &v); \
    vec3##t &operator /= (const vec3##t &v); \
    vec3##t &operator *= (T v); \
    vec3##t &operator /= (T v); \
    T dot()                         const; \
    T dot(const vec3##t &b)         const; \
    T length()                      const; \
    vec3##t normalize()             const; \
    vec3##t cross(const vec3##t &b) const; \
    vec3##t mix  (const vec3##t &b, double v) const; \
}; \

#define vec4_decl(T, t) \
struct vec4##t { \
    T x, y, z, w; \
    vec4##t(); \
    vec4##t(str s); \
    vec4##t(cstr s); \
    vec4##t(const vec3##t &xyz, T w); \
    vec4##t(const vec2##t &a, const vec2##t &b); \
    vec4##t(T x); \
    vec4##t(T x, T y, T z, T w); \
    vec4##t&operator= (const vec4##t &v); \
    vec4##t operator+ (const vec4##t &b) const; \
    vec4##t operator- (const vec4##t &b) const; \
    vec4##t operator* (const vec4##t &b) const; \
    vec4##t operator/ (const vec4##t &b) const; \
    vec4##t operator* (const T v) const; \
    vec4##t operator/ (const T v) const; \
    vec4##t &operator += (const vec4##t &v); \
    vec4##t &operator -= (const vec4##t &v); \
    vec4##t &operator *= (const vec4##t &v); \
    vec4##t &operator /= (const vec4##t &v); \
    vec4##t &operator *= (T v); \
    vec4##t &operator /= (T v); \
    operator bool()                         const; \
    const T &operator[](int i)              const; \
    operator object()                       const; \
    vec3##t xyz()                           const; \
    str color()                             const; \
    T dot(const vec4##t &b)                 const; \
    vec4##t mix(const vec4##t &b, double v) const; \
}; \

vec2_decl(float,  f)
vec2_decl(double, d)
vec2_decl(int,    i)
vec2_decl(u8,    u8)

vec3_decl(float,  f)
vec3_decl(double, d)
vec3_decl(int,    i)
vec3_decl(u8,    u8)

vec4_decl(float,  f)
vec4_decl(double, d)
vec4_decl(int,    i)
vec4_decl(u8,    u8)

#define mat44_decl(T, t) \
struct mat44##t { \
    vec4##t cols[4]; \
    mat44##t(); \
    mat44##t(T x); \
    mat44##t(str s); \
    mat44##t(const vec4##t &r0, const vec4##t &r1, const vec4##t &r2, const vec4##t &r3);\
    mat44##t(T x, T y, T z); \
    mat44##t(T x, T y, T z, T w); \
    mat44##t(const quat##t &q); \
    mat44##t(const vec4##t  &v); \
    mat44##t(const mat44##t &v); \
             operator bool() const; \
    const vec4##t &operator[](int i) const; \
             operator object() const; \
    mat44##t operator*(const mat44##t &b) const; \
    vec4##t  operator*(const vec4##t  &b) const; \
    vec3##t  operator*(const vec3##t  &b) const; \
    mat44##t&operator= (const mat44##t &v); \
    mat44##t &operator *= (const mat44##t &v); \
    static mat44##t make_translation(const vec3##t &v); \
    static mat44##t make_scaling(const vec3##t &v); \
    mat44##t translate(const vec3##t &v) const; \
    mat44##t scale(const vec3##t &v) const; \
    mat44##t mix(const mat44##t &b, double v) const; \
    static mat44##t perspective(T fov, T aspect, T near, T far);\
    static mat44##t look_at(const vec3##t &eye, const vec3##t &center, const vec3##t &up); \
    static mat44##t orthographic(T left, T right, T bottom, T top, T near, T far); \
};

#define quat_decl(T, t) \
struct quat##t:vec4##t { \
    quat##t(); \
    quat##t(vec3##t axis, T angle) ; \
    quat##t(T w, T x, T y, T z) ; \
    quat##t &operator=(const quat##t &b); \
}; \

quat_decl(float, f)
mat44_decl(float, f)

using m44f    = mat44f;

m44f translate(const m44f &m, const vec3f &b);
m44f scale(const m44f &m, const vec3f &b);

struct edge {
    using vec2 = ion::vec2d;
    using vec4 = ion::vec4d;
    using T    = double;
    vec2 a, b;

    /// returns x-value of intersection of two lines
    T x(const vec2 &c, const vec2 &d) const;
    
    /// returns y-value of intersection of two lines
    T y(const vec2 &c, const vec2 &d) const;
    
    /// returns xy-value of intersection of two lines
    vec2 xy(const vec2 &c, const vec2 &d) const;

    /// returns x-value of intersection of two lines
    T x(const edge &e) const;
    
    /// returns y-value of intersection of two lines
    T y(const edge &e) const;

    /// returns xy-value of intersection of two lines (via edge0 and edge1)
    vec2 xy(const edge &e) const;
};


struct rect {
    using T = double;
    using vec2 = ion::vec2d;
    T x, y, w, h;

    vec2 r_tl = { 0, 0 };
    vec2 r_tr = { 0, 0 };
    vec2 r_bl = { 0, 0 };
    vec2 r_br = { 0, 0 };
    bool rounded = false;

    rect(T x = 0, T y = 0, T w = 0, T h = 0);
    rect(int x, int y, int w, int h);

    rect(const vec2 &p0, const vec2 &p1);

    rect  offset(T a)                const;
    vec2  sz()                       const;
    vec2  xy()                       const;
    vec2  center()                   const;
    bool  contains(const vec2 &p)    const;
    rect &operator=  (const rect &r);
    bool  operator== (const rect &r) const;
    bool  operator!= (const rect &r) const;
    rect  operator + (const rect &r) const;
    rect  operator - (const rect &r) const;
    rect  operator + (const vec2 &v) const;
    rect  operator - (const vec2 &v) const;
    rect  operator * (T        r)    const;
    rect  operator / (T        r)    const;
          operator bool()            const;

    void set_rounded(const vec2 &tl, const vec2 &tr, const vec2 &br, const vec2 &bl);

    rect clip(const rect &input) const;
};

}
