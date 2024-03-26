#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <mx/mx.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace ion {

//template <typename T> using vec2 = glm::tvec2<T>;
//template <typename T> using vec3 = glm::tvec3<T>;
//template <typename T> using vec4 = glm::tvec4<T>;
//template <typename T> using rgba = glm::tvec4<T>;

template <typename T> using m44  = glm::tmat4x4<T>;
template <typename T> using m33  = glm::tmat3x3<T>;
template <typename T> using m22  = glm::tmat2x2<T>;

template <typename T>
struct vec2 {
    T x, y;

    operator bool() { return !(x == 0 && y == 0); }

    vec2() : x(0), y(0) { }

    vec2(const vec2 &v) : x(v.x), y(v.y) { }
 
    vec2(T x) : x(x), y(x) { }

    vec2(T x, T y) : x(x), y(y) { }

    template <typename X, typename Y>
    vec2(X x, Y y) : x(T(x)), y(T(y)) { }

    vec2(cstr s) : vec2(str(s)) { }

    vec2 mix(vec2 &b, double v) {
        vec2 &a = *this;
        return vec2 {
            T(a.x * (1.0 - v) + b.x * v),
            T(a.y * (1.0 - v) + b.y * v)
        };
    }

    vec2(str s) {
        array<str> v = s.split();
        size_t len = v.len();
        if (len == 1) {
            x = T(v[0].real_value<real>());
            y = x;
        }
        else if (len == 2) {
            x = T(v[0].real_value<real>());
            y = T(v[1].real_value<real>());
        } else {
            assert(false);
        }
    }

    operator mx() {
        return array<T> { x, y };
    }

    inline double length() {
        return sqrt((x * x) + (y * y));
    }

    inline vec2 &operator= (vec2 v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    inline vec2 operator+ (const vec2 &b) const {
        return { x + b.x, y + b.y };
    }

    inline vec2 operator- (const vec2 &b) const {
        return { x - b.x, y - b.y };
    }

    inline vec2 operator* (const vec2 &b) const {
        return { x * b.x, y * b.y };
    }

    inline vec2 operator/ (const vec2 &b) const {
        return { x / b.x, y / b.y };
    }

    inline vec2 operator* (const T v) const {
        return { x * v, y * v };
    }

    inline vec2 operator/ (const T v) const {
        return { x / v, y / v };
    }

    inline vec2 &operator += (vec2 v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    inline vec2 &operator -= (vec2 v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline vec2 &operator *= (vec2 v) {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    inline vec2 &operator /= (vec2 v) {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    inline vec2 &operator *= (T v) {
        x *= v;
        y *= v;
        return *this;
    }

    inline vec2 &operator /= (T v) {
        x /= v;
        y /= v;
        return *this;
    }
};

template <typename T>
struct vec3 {
    T x, y, z;

    operator bool() { return !(x == 0 && y == 0 && z == 0); }

    vec3() : x(0), y(0), z(0) { }

    vec3(T x) : x(x), y(x), z(x) { }

    vec3(T x, T y, T z) : x(x), y(y), z(z) { }

    template <typename X, typename Y, typename Z>
    vec3(X x, Y y, Z z) : x(T(x)), y(T(y)), z(T(z)) { }

    vec3(cstr s) : vec3(str(s)) { }

    operator mx() {
        return array<T> { x, y, z };
    }

    vec3 mix(vec3 &b, double v) {
        vec3 &a = *this;
        return vec3 {
            T(a.x * (1.0 - v) + b.x * v),
            T(a.y * (1.0 - v) + b.y * v),
            T(a.z * (1.0 - v) + b.z * v)
        };
    }

    vec3(str s) {
        array<str> v = s.split();
        size_t len = v.len();
        if (len == 1) {
            x = T(v[0].real_value<real>());
            y = x;
            z = x;
        }
        else if (len == 3) {
            x = T(v[0].real_value<real>());
            y = T(v[1].real_value<real>());
            z = T(v[2].real_value<real>());
        } else {
            assert(false);
        }
    }
};


template <typename T>
struct vec4 {
    T x, y, z, w;

    operator bool() { return !(x == 0 && y == 0 && z == 0 && w == 0); }

    vec4() : x(0), y(0), z(0), w(0) { }

    vec4(T x) : x(x), y(x), z(x), w(x) { }

    vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }

    template <typename X, typename Y, typename Z, typename W>
    vec4(X x, Y y, Z z, W w) : x(T(x)), y(T(y)), z(T(z)), w(T(w)) { }

    vec4(cstr s) : vec4(str(s)) { }

    operator mx() {
        return array<T> { x, y, z, w };
    }

    vec4 mix(vec4 &b, double v) {
        vec4 &a = *this;
        return vec4 {
            T(a.x * (1.0 - v) + b.x * v),
            T(a.y * (1.0 - v) + b.y * v),
            T(a.z * (1.0 - v) + b.z * v),
            T(a.w * (1.0 - v) + b.w * v)
        };
    }

    vec4(str s) {
        array<str> v = s.split();
        size_t len = v.len();
        if (len == 1) {
            x = T(v[0].real_value<real>());
            y = x;
            z = x;
            w = x;
        }
        else if (len == 4) {
            x = T(v[0].real_value<real>());
            y = T(v[1].real_value<real>());
            z = T(v[2].real_value<real>());
            w = T(v[3].real_value<real>());
        } else {
            assert(false);
        }
    }
};

//template <> struct is_opaque<glm::vec2> : true_type { };
//template <> struct is_opaque<glm::vec3> : true_type { };
//template <> struct is_opaque<glm::vec4> : true_type { };
//template <> struct is_opaque<glm::mat4> : true_type { };

//external(glmv2);
//external(glmv3);

/// vkg must use doubles!
using m44d    = m44 <r64>;
using m44f    = m44 <r32>;

using m33d    = m33 <r64>;
using m33f    = m33 <r32>;

using m22d    = m22 <r64>;
using m22f    = m22 <r32>;

using vec2u8  = vec2 <u8>;
using vec2i   = vec2 <i32>;
using vec2d   = vec2 <r64>;
using vec2f   = vec2 <r32>;

using vec3u8  = vec3 <u8>;
using vec3i   = vec3 <i32>;
using vec3d   = vec3 <r64>;
using vec3f   = vec3 <r32>;

using vec4u8  = vec4 <u8>;
using vec4i   = vec4 <i32>;
using vec4d   = vec4 <r64>;
using vec4f   = vec4 <r32>;

}
