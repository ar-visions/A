#include <mx/mx.hpp>

#define vec2_impl(T, t) \
vec2##t::operator bool() { return !(x == 0 && y == 0); } \
vec2##t::vec2##t() : x(0), y(0) { } \
vec2##t::vec2##t(const vec2##t &v) : x(v.x), y(v.y) { } \
vec2##t::vec2##t(T x) : x(x), y(x) { } \
vec2##t::vec2##t(T x, T y) : x(x), y(y) { } \
vec2##t::vec2##t(cstr s) : vec2##t(str(s)) { } \
vec2##t::vec2##t mix(vec2##t &b, double v) { \
    vec2##t &a = *this; \
    return vec2##t { \
        T(a.x * (1.0 - v) + b.x * v), \
        T(a.y * (1.0 - v) + b.y * v) \
    }; \
} \
vec2##t::vec2##t(str s) {               \
    array<str> v   = s.split();         \
    size_t     len = v.len();           \
    if (len == 1) {                     \
        x = T(v[0].real_value<real>()); \
        y = x;                          \
    }                                   \
    else if (len == 2) {                \
        x = T(v[0].real_value<real>()); \
        y = T(v[1].real_value<real>()); \
    } else {                            \
        assert(false);                  \
    }                                   \
}                                       \
vec2##t::operator mx() { return array<T> { x, y }; } \
double vec2##t::length() { return sqrt((x * x) + (y * y)); } \
vec2##t &vec2##t::operator= (const vec2##t &v) { x = v.x; y = v.y; return *this; } \
vec2##t vec2##t::operator+ (const vec2##t &b) const { return { x + b.x, y + b.y }; } \
vec2##t vec2##t::operator- (const vec2##t &b) const { return { x - b.x, y - b.y }; } \
vec2##t vec2##t::operator* (const vec2##t &b) const { return { x * b.x, y * b.y }; } \
vec2##t vec2##t::operator/ (const vec2##t &b) const { return { x / b.x, y / b.y }; } \
vec2##t vec2##t::operator* (const T v)     const { return { x * v, y * v }; } \
vec2##t vec2##t::operator/ (const T v)     const { return { x / v, y / v }; } \
vec2##t &vec2##t::operator += (vec2##t v) { x += v.x; y += v.y; return *this; } \
vec2##t &vec2##t::operator -= (vec2##t v) { x -= v.x; y -= v.y; return *this; } \
vec2##t &vec2##t::operator *= (vec2##t v) { x *= v.x; y *= v.y; return *this; } \
vec2##t &vec2##t::operator /= (const vec2##t &v) { x /= v.x; y /= v.y; return *this; } \
vec2##t &vec2##t::operator *= (T v) { x *= v; y *= v; return *this; } \
vec2##t &vec2##t::operator /= (T v) { x /= v; y /= v; return *this; } \

#define vec3_impl(T) \
struct vec3##t { \
    vec3##t::operator bool() { return !(x == 0 && y == 0 && z == 0); } \
    vec3##t::vec3##t() : x(0), y(0), z(0) { } \
    vec3##t::vec3##t(T x) : x(x), y(x), z(x) { } \
    vec3##t::vec3##t(T x, T y, T z) : x(x), y(y), z(z) { } \
    vec3##t::vec3##t(cstr s) : vec3##t(str(s)) { } \
    vec3##t::operator mx() { return array<T> { x, y, z }; } \
    vec3##t vec3##t::mix(vec3##t &b, double v) { \
        vec3##t &a = *this; \
        return vec3##t { \
            T(a.x * (1.0 - v) + b.x * v), \
            T(a.y * (1.0 - v) + b.y * v), \
            T(a.z * (1.0 - v) + b.z * v) \
        }; \
    } \
    vec3##t::vec3##t(str s) { \
        array<str> v = s.split(); \
        size_t len = v.len(); \
        if (len == 1) { \
            x = T(v[0].real_value<real>()); \
            y = x; \
            z = x; \
        } \
        else if (len == 3) { \
            x = T(v[0].real_value<real>()); \
            y = T(v[1].real_value<real>()); \
            z = T(v[2].real_value<real>()); \
        } else { \
            assert(false); \
        } \
    } \
}; \

#define vec3_impl(T, t) \
    vec4##t::operator bool() { return !(x == 0 && y == 0 && z == 0 && w == 0); } \
    vec4##t() : x(0), y(0), z(0), w(0) { } \
    vec4##t(T x) : x(x), y(x), z(x), w(x) { } \
    vec4##t(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { } \
    vec4##t(cstr s) : vec4##t(str(s)) { } \
    vec4##t::operator mx() { \
        return array<T> { x, y, z, w }; \
    } \
    vec4##t vec4##t::mix(vec4 &b, double v) { \
        vec4 &a = *this; \
        return vec4 { \
            T(a.x * (1.0 - v) + b.x * v), \
            T(a.y * (1.0 - v) + b.y * v), \
            T(a.z * (1.0 - v) + b.z * v), \
            T(a.w * (1.0 - v) + b.w * v) \
        }; \
    } \
    T &vec4##t::operator[](int i) { return * (&x + i); } \
    vec4##t::vec4##t(str s) { \
        array<str> v = s.split(); \
        size_t len = v.len(); \
        if (len == 1) { \
            x = T(v[0].real_value<real>()); \
            y = x; \
            z = x; \
            w = x; \
        } \
        else if (len == 4) { \
            x = T(v[0].real_value<real>()); \
            y = T(v[1].real_value<real>()); \
            z = T(v[2].real_value<real>()); \
            w = T(v[3].real_value<real>()); \
        } else { \
            assert(false); \
        } \
    } \
}; \

#define mat4_impl(T, t) \
struct mat##t { \
    vec4##t rows[4]; \
    operator bool(); \
    mat##t::mat##t(); \
    mat##t::mat##t(T x); \
    mat##t(const vec4##t &); \
    mat##t(const &T[16] v); \
    operator mx(); \
    mat##t mix(mat##t &b, double v); \
    mat##t(str s); \
}; \

vec2_impl(float,  f)
vec2_impl(double, d)
vec2_impl(i8,     i8)
vec2_impl(u8,     u8)
vec2_impl(i16,    i16)
vec2_impl(u16,    u16)
vec2_impl(i32,    i32)
vec2_impl(u32,    u32)
vec2_impl(i64,    i64)
vec2_impl(u64,    u64)

vec3_impl(float,  f)
vec3_impl(double, d)
vec3_impl(i8,     i8)
vec3_impl(u8,     u8)
vec3_impl(i16,    i16)
vec3_impl(u16,    u16)
vec3_impl(i32,    i32)
vec3_impl(u32,    u32)
vec3_impl(i64,    i64)
vec3_impl(u64,    u64)

vec4_impl(float,  f)
vec4_impl(double, d)
vec4_impl(i8,     i8)
vec4_impl(u8,     u8)
vec4_impl(i16,    i16)
vec4_impl(u16,    u16)
vec4_impl(i32,    i32)
vec4_impl(u32,    u32)
vec4_impl(i64,    i64)
vec4_impl(u64,    u64)

mat4_impl(float,  f)
