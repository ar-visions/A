#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <mx/mx.hpp>

namespace ion {

#define vec2_decl(T, t) \
struct vec2##t { \
    T x, y; \
    operator bool(); \
    vec2##t(); \
    vec2##t(const vec2##t &v); \
    vec2##t(T x); \
    vec2##t(T x, T y); \
    vec2##t(cstr s); \
    vec2##t mix(vec2##t &b, double v); \
    vec2##t(str s); \
    operator mx(); \
    double length(); \
    vec2##t &operator= (vec2##t v); \
    vec2##t operator+ (const vec2##t &b) const; \
    vec2##t operator- (const vec2##t &b) const; \
    vec2##t operator* (const vec2##t &b) const; \
    vec2##t operator/ (const vec2##t &b) const; \
    vec2##t operator* (const T v) const; \
    vec2##t operator/ (const T v) const; \
    vec2##t &operator += (vec2##t v); \
    vec2##t &operator -= (vec2##t v); \
    vec2##t &operator *= (vec2##t v); \
    vec2##t &operator /= (vec2##t v); \
    vec2##t &operator *= (T v); \
    vec2##t &operator /= (T v); \
}; \

#define vec3_decl(T, t) \
struct vec3##t { \
    T x, y, z; \
    operator bool(); \
    vec3##t(); \
    vec3##t(T x); \
    vec3##t(T x, T y, T z); \
    vec3##t(cstr s); \
    operator mx(); \
    vec3##t mix(vec3##t &b, double v); \
    vec3##t(str s); \
}; \

#define vec4_decl(T, t) \
struct vec4##t { \
    T x, y, z, w; \
    operator bool() { return !(x == 0 && y == 0 && z == 0 && w == 0); } \
    vec4##t(); \
    vec4##t(T x); \
    vec4##t(T x, T y, T z, T w); { } \
    vec4##t(cstr s); { } \
    T &operator[](int i); \
    operator mx(); \
    vec4##t mix(vec4##t &b, double v); \
    vec4##t(str s); \
}; \

#define mat4_decl(T, t) \
struct mat##t { \
    vec4 rows[4]; \
    operator bool(); \
    mat##t(); \
    mat##t(T x); \
    mat##t(T x, T y, T z, T w); { } \
    mat##t(const &T[16]); \
    operator mx(); \
    mat##t mix(mat##t &b, double v); \
    mat##t(str s); \
}; \

vec2_decl(float,  f)
vec2_decl(double, d)
vec2_decl(i8,     i8)
vec2_decl(u8,     u8)
vec2_decl(i16,    i16)
vec2_decl(u16,    u16)
vec2_decl(i32,    i32)
vec2_decl(u32,    u32)
vec2_decl(i64,    i64)
vec2_decl(u64,    u64)

vec3_decl(float,  f)
vec3_decl(double, d)
vec3_decl(i8,     i8)
vec3_decl(u8,     u8)
vec3_decl(i16,    i16)
vec3_decl(u16,    u16)
vec3_decl(i32,    i32)
vec3_decl(u32,    u32)
vec3_decl(i64,    i64)
vec3_decl(u64,    u64)

vec4_decl(float,  f)
vec4_decl(double, d)
vec4_decl(i8,     i8)
vec4_decl(u8,     u8)
vec4_decl(i16,    i16)
vec4_decl(u16,    u16)
vec4_decl(i32,    i32)
vec4_decl(u32,    u32)
vec4_decl(i64,    i64)
vec4_decl(u64,    u64)

mat4_decl(float,  f)

using m44d    = mat44d;
using m44f    = mat44f;

}
