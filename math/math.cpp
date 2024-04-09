#include <mx/mx.hpp>
#include <math/math.hpp>

namespace ion {

#define vec2_impl(T, t) \
    vec2##t::vec2##t() : x(0), y(0) { } \
    vec2##t::vec2##t(const vec2##t &v) : x(v.x), y(v.y) { } \
    vec2##t::vec2##t(T x) : x(x), y(x) { } \
    vec2##t::vec2##t(T x, T y) : x(x), y(y) { } \
    vec2##t::vec2##t(cstr s) : vec2##t(str(s)) { } \
    T vec2##t::dot() const { return T(x*x) + T(y*y); }\
    T vec2##t::length() const { return std::sqrt(dot()); }\
    T vec2##t::dot(const vec2##t &b) const {\
        return x * b.x + y * b.y;\
    }\
    vec2##t  vec2##t::mix(const vec2##t &b, double v) const { \
        const vec2##t &a = *this; \
        return vec2##t { \
            T(a.x * (1.0 - v) + b.x * v), \
            T(a.y * (1.0 - v) + b.y * v) \
        }; \
    } \
    vec2##t::vec2##t(str s) {               \
        Array<str>  v  = s.split();         \
        size_t     len = v.length();        \
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
    const T &vec2##t::operator[](int i) const { return * (&x + i); } \
    vec2##t::operator bool() const { return !(x == 0 && y == 0); } \
    vec2##t::operator mx() const { return array { x, y }; } \
    vec2##t vec2##t::operator+ (const vec2##t &b) const { return vec2##t { T(x + b.x), T(y + b.y) }; } \
    vec2##t vec2##t::operator- (const vec2##t &b) const { return vec2##t { T(x - b.x), T(y - b.y) }; } \
    vec2##t vec2##t::operator* (const vec2##t &b) const { return vec2##t { T(x * b.x), T(y * b.y) }; } \
    vec2##t vec2##t::operator/ (const vec2##t &b) const { return vec2##t { T(x / b.x), T(y / b.y) }; } \
    vec2##t vec2##t::operator* (const T v)     const { return vec2##t { T(x * v), T(y * v) }; } \
    vec2##t vec2##t::operator/ (const T v)     const { return vec2##t { T(x / v), T(y / v) }; } \
    vec2##t &vec2##t::operator += (vec2##t v) { x += v.x; y += v.y; return *this; } \
    vec2##t &vec2##t::operator -= (vec2##t v) { x -= v.x; y -= v.y; return *this; } \
    vec2##t &vec2##t::operator *= (vec2##t v) { x *= v.x; y *= v.y; return *this; } \
    vec2##t &vec2##t::operator /= (const vec2##t &v) { x /= v.x; y /= v.y; return *this; } \
    vec2##t &vec2##t::operator *= (T v) { x *= v; y *= v; return *this; } \
    vec2##t &vec2##t::operator /= (T v) { x /= v; y /= v; return *this; } \

#define vec3_impl(T, t) \
    vec3##t::operator bool() const { return !(x == 0 && y == 0 && z == 0); } \
    vec3##t::vec3##t() : x(0), y(0), z(0) { } \
    vec3##t::vec3##t(T x) : x(x), y(x), z(x) { } \
    vec3##t::vec3##t(T x, T y, T z) : x(x), y(y), z(z) { } \
    vec3##t::vec3##t(cstr s) : vec3##t(str(s)) { } \
    vec3##t::operator mx() const { return Array<T> { x, y, z }; } \
    const T &vec3##t::operator[](int i) const { return * (&x + i); } \
    T vec3##t::dot(const vec3##t &b) const {\
        return x * b.x + y * b.y + z * b.z;\
    }\
    vec3##t vec3##t::cross(const vec3##t &b) const {\
        return vec3##t {\
            T((double(y) * b.z) - (double(z) * b.y)),\
            T((double(z) * b.x) - (double(x) * b.z)),\
            T((double(x) * b.y) - (double(y) * b.x))\
        };\
    }\
    T vec3##t::dot() const { return T(x*x) + T(y*y) + T(z*z); }\
    T vec3##t::length() const { return std::sqrt(dot()); }\
    vec3##t vec3##t::normalize() const {\
        double len = std::sqrt(dot());\
        return vec3##t {\
            T(x / len),\
            T(y / len),\
            T(z / len)\
        };\
    }\
    vec3##t vec3##t::mix(const vec3##t &b, double v) const { \
        const vec3##t &a = *this; \
        return vec3##t { \
            T(a.x * (1.0 - v) + b.x * v), \
            T(a.y * (1.0 - v) + b.y * v), \
            T(a.z * (1.0 - v) + b.z * v) \
        }; \
    } \
    vec3##t::vec3##t(str s) { \
        Array<str> v = s.split(); \
        size_t len = v.length(); \
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
    vec3##t vec3##t::operator+ (const vec3##t &b) const { return vec3##t { T(x + b.x), T(y + b.y), T(z + b.z) }; } \
    vec3##t vec3##t::operator- (const vec3##t &b) const { return vec3##t { T(x - b.x), T(y - b.y), T(z - b.z) }; } \
    vec3##t vec3##t::operator* (const vec3##t &b) const { return vec3##t { T(x * b.x), T(y * b.y), T(z * b.z) }; } \
    vec3##t vec3##t::operator/ (const vec3##t &b) const { return vec3##t { T(x / b.x), T(y / b.y), T(z / b.z) }; } \
    vec3##t vec3##t::operator* (const T v)     const { return vec3##t { T(x * v), T(y * v), T(z * v) }; } \
    vec3##t vec3##t::operator/ (const T v)     const { return vec3##t { T(x / v), T(y / v), T(z / v) }; } \
    vec3##t &vec3##t::operator += (const vec3##t &v) { x += v.x; y += v.y; z += v.z; return *this; } \
    vec3##t &vec3##t::operator -= (const vec3##t &v) { x -= v.x; y -= v.y; z -= v.z; return *this; } \
    vec3##t &vec3##t::operator *= (const vec3##t &v) { x *= v.x; y *= v.y; z *= v.z; return *this; } \
    vec3##t &vec3##t::operator /= (const vec3##t &v) { x /= v.x; y /= v.y; z /= v.z; return *this; } \
    vec3##t &vec3##t::operator *= (T v) { x *= v; y *= v; z *= v; return *this; } \
    vec3##t &vec3##t::operator /= (T v) { x /= v; y /= v; z /= v; return *this; } \

#define vec4_impl(T, t) \
    using namespace ion;\
    ion::vec4##t::vec4##t() : x(0), y(0), z(0), w(0) { } \
    ion::vec4##t::vec4##t(T x) : x(x), y(x), z(x), w(x) { } \
    ion::vec4##t::vec4##t(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { } \
    ion::vec4##t::vec4##t(cstr s) : vec4##t(ion::str(s)) { } \
    T ion::vec4##t::dot(const vec4##t &b) const {\
        return x * b.x + y * b.y + z * b.z + w * b.w;\
    }\
    ion::vec4##t::vec4##t(ion::str h) { \
        size_t sz = h.len(); \
        i32    ir = 0, ig = 0, \
            ib = 0, ia = 255; \
        if (sz && h[0] == '#') { \
            auto nib = [&](char const n) -> i32 const { \
                return (n >= '0' && n <= '9') ?       (n - '0')  : \
                        (n >= 'a' && n <= 'f') ? (10 + (n - 'a')) : \
                        (n >= 'A' && n <= 'F') ? (10 + (n - 'A')) : 0; \
            }; \
            switch (sz) { \
                case 5: \
                    ia  = nib(h[4]) << 4 | nib(h[4]); \
                    [[fallthrough]]; \
                case 4: \
                    ir  = nib(h[1]) << 4 | nib(h[1]); \
                    ig  = nib(h[2]) << 4 | nib(h[2]); \
                    ib  = nib(h[3]) << 4 | nib(h[3]); \
                    break; \
                case 9: \
                    ia  = nib(h[7]) << 4 | nib(h[8]); \
                    [[fallthrough]]; \
                case 7: \
                    ir  = nib(h[1]) << 4 | nib(h[2]); \
                    ig  = nib(h[3]) << 4 | nib(h[4]); \
                    ib  = nib(h[5]) << 4 | nib(h[6]); \
                    break; \
            } \
            if constexpr (is_integral<T>()) { \
                x = T(ir); \
                y = T(ig); \
                z = T(ib); \
                w = T(ia); \
            } else { \
                x = T(ir) / T(255.0); \
                y = T(ig) / T(255.0); \
                z = T(ib) / T(255.0); \
                w = T(ia) / T(255.0); \
            } \
        } else if (sz && (h[0] == '-' || (h[0] >= '0' && h[0] <= 9))) { \
            ion::Array<ion::str> v = h.split(); \
            size_t len = v.length(); \
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
        }\
    } \
    ion::vec4##t ion::vec4##t::mix(const vec4##t &b, double v) const { \
        const vec4##t &a = *this; \
        return vec4##t { \
            T(a.x * (1.0 - v) + b.x * v), \
            T(a.y * (1.0 - v) + b.y * v), \
            T(a.z * (1.0 - v) + b.z * v), \
            T(a.w * (1.0 - v) + b.w * v) \
        }; \
    } \
    ion::vec4##t::operator bool() const { return !(x == 0 && y == 0 && z == 0 && w == 0); } \
    ion::vec4##t::operator mx() const { return Array<T> { x, y, z, w }; } \
    const T &ion::vec4##t::operator[](int i) const { return * (&x + i); } \
    ion::str ion::vec4##t::color() const {\
        char    res[64];\
        u8      arr[4];\
        real sc;\
        if constexpr (identical<T, uint8_t>())\
            sc = 1.0;\
        else\
            sc = 255.0;\
        arr[0] = uint8_t(math::round(x * sc));\
        arr[1] = uint8_t(math::round(y * sc));\
        arr[2] = uint8_t(math::round(z * sc));\
        arr[3] = uint8_t(math::round(w * sc));\
        res[0] = '#';\
        for (size_t i = 0, len = sizeof(arr) - (arr[3] == 255); i < len; i++) {\
            size_t index = 1 + i * 2;\
            snprintf(&res[index], sizeof(res) - index, "%02x", arr[i]);\
        }\
        return res;\
    }\
    vec4##t vec4##t::operator+ (const vec4##t &b) const { return vec4##t { T(x + b.x), T(y + b.y), T(z + b.z), T(w + b.w) }; } \
    vec4##t vec4##t::operator- (const vec4##t &b) const { return vec4##t { T(x - b.x), T(y - b.y), T(z - b.z), T(w - b.w) }; } \
    vec4##t vec4##t::operator* (const vec4##t &b) const { return vec4##t { T(x * b.x), T(y * b.y), T(z * b.z), T(w * b.w) }; } \
    vec4##t vec4##t::operator/ (const vec4##t &b) const { return vec4##t { T(x / b.x), T(y / b.y), T(z / b.z), T(w / b.w) }; } \
    vec4##t vec4##t::operator* (const T v)     const { return vec4##t { T(x * v), T(y * v), T(z * v), T(w * v) }; } \
    vec4##t vec4##t::operator/ (const T v)     const { return vec4##t { T(x / v), T(y / v), T(z / v), T(w / v) }; } \
    vec4##t &vec4##t::operator += (const vec4##t &v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; } \
    vec4##t &vec4##t::operator -= (const vec4##t &v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; } \
    vec4##t &vec4##t::operator *= (const vec4##t &v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; } \
    vec4##t &vec4##t::operator /= (const vec4##t &v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; } \
    vec4##t &vec4##t::operator *= (T v) { x *= v; y *= v; z *= v; w *= v; return *this; } \
    vec4##t &vec4##t::operator /= (T v) { x /= v; y /= v; z /= v; w /= v; return *this; } \

/*
| 1 - 2y^2 - 2z^2    2xy - 2wz        2xz + 2wy        0 |
| 2xy + 2wz          1 - 2x^2 - 2z^2  2yz - 2wx        0 |
| 2xz - 2wy          2yz + 2wx        1 - 2x^2 - 2y^2  0 |
| 0                  0                0                1 |
*/
#define exp(x,y) pow(x,y)

#define quad_impl(T, t) \
struct quat { \
    vec4##t v; \
    quat(vec3##t axis, T angle) { \
        T h = angle / T(2.0); \
        w = cos(h); \
        x = axis.x * sin(h); \
        y = axis.y * sin(h); \
        z = axis.z * sin(h); \
    } \
}; \

#define mat44_impl2(T, t) \
    ion::mat44##t::operator bool() const { return rows[3].w > T(0); }; \
    ion::mat44##t::mat44##t() : mat44##t(T(1.0)) { } \
    ion::mat44##t::mat44##t(const vec4##t &r0, const vec4##t &r1, const vec4##t &r2, const vec4##t &r3) {\
        rows[0] = r0;\
        rows[1] = r1;\
        rows[2] = r2;\
        rows[3] = r3;\
    }\
    ion::mat44##t::mat44##t(str s) : mat44##t() { } \
    ion::mat44##t::mat44##t(T x, T y, T z, T w) {\
        rows[0] = vec4##t( (1) - 2*exp(y,2) - 2*exp(z,2), (2*x*y) - (2*w*z),            (2*x*z) + (2*w*y), 0 ) ; \
        rows[1] = vec4##t( (2*x*y) + (2*w*z),             1 - 2*exp(x, 2) - 2*exp(z,2), (2*y*z) - (2*w*x), 0 ) ; \
        rows[2] = vec4##t( (2*x*z) - (2*w*y),             (2*y*z + 2*w*x),              1 - 2*exp(x, 2) - 2*exp(y, 2),       0 ) ; \
        rows[3] = vec4##t( 0, 0, 0, 1 ) ; \
    }\
    ion::mat44##t::mat44##t(T x, T y, T z) : mat44##t(x, y, z, T(1.0)) { } \
    ion::mat44##t::mat44##t(T v) : mat44##t(v, v, v, v) { } \
    ion::mat44##t::mat44##t(const vec4##t &v) : mat44##t(((vec4##t&)v)[0], v[1], v[2], v[3]) { } \
    ion::mat44##t::mat44##t(const mat44##t &v) { \
        memcpy((T*)&rows[0][0], (T*)&v[0][0], sizeof(T) * 4 * 4); \
    } \
    const ion::vec4##t &ion::mat44##t::operator[](int i) const { return rows[i]; } \
    ion::mat44##t ion::mat44##t::operator*(const mat44##t &b) const {\
        mat44##t result(T(0));\
        for (int i = 0; i < 4; i++) {\
            for (int j = 0; j < 4; j++) {\
                T &sum = (T&)result[i][j];\
                for (int k = 0; k < 4; k++)\
                    sum += rows[i][k] * b[k][j];\
            }\
        }\
        return result;\
    }\
    ion::vec4##t ion::mat44##t::operator*(const vec4##t &b) const {\
        vec4##t result(T(0));\
        for (int i = 0; i < 4; i++) {\
            T &sum = (T&)result[i];\
            for (int j = 0; j < 4; j++)\
                sum += rows[i][j] * b[j];\
        }\
        return result;\
    }\
    ion::vec3##t ion::mat44##t::operator*(const vec3##t &b) const {\
        vec4##t v { b.x, b.y, b.z, T(1.0) };\
        vec4##t r = *this * v;\
        return vec3##t(r.x, r.y, r.z);\
    }\
    mat44##t &mat44##t::operator *= (const mat44##t &v) {\
        *this = *this * v; \
        return *this; \
    } \
    ion::mat44##t::operator mx() const { \
        return mx::window(this); \
    }\
    ion::mat44##t mat44##t::mix(const mat44##t &b, double v) const {\
        return mat44##t {\
            rows[0].mix(b.rows[0], v),\
            rows[1].mix(b.rows[1], v),\
            rows[2].mix(b.rows[2], v),\
            rows[3].mix(b.rows[3], v)\
        };\
    } \
    ion::mat44##t look_at(const vec3##t &eye, const vec3##t &center, const vec3##t &u) {\
        vec3##t forward = ion::normalize(center - eye);\
        vec3##t right = ion::normalize(ion::cross(forward, u));\
        vec3##t up = ion::cross(right, forward);\
        return mat44##t {\
            vec4##t(right.x,    up.x,    -forward.x,    0),\
            vec4##t(right.y,    up.y,    -forward.y,    0),\
            vec4##t(right.z,    up.z,    -forward.z,    0),\
            vec4##t(-dot(right, eye), -dot(up, eye),  dot(forward, eye), 1)\
        };\
    }\
    ion::mat44##t perspective(T fov, T aspect, T near, T far) {\
        T ff = 1 / tan(fov / 2);\
        T nf = 1 / (near - far);\
        return mat44##t {\
            vec4##t(ff / aspect, 0, 0, 0),\
            vec4##t(0, ff, 0, 0),\
            vec4##t(0, 0, (far + near) * nf, -1),\
            vec4##t(0, 0, 2 * far * near * nf, 0)\
        };\
    }\

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

//ion::vec4d::operator bool() const { return !(x == 0 && y == 0 && z == 0 && w == 0); }

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

mat44_impl2(float, f)


/// returns x-value of intersection of two lines
double edge::x(const vec2 &c, const vec2 &d) const {
    T num = (a.x*b.y  -  a.y*b.x) * (c.x-d.x) - (a.x-b.x) * (c.x*d.y - c.y*d.x);
    T den = (a.x-b.x) * (c.y-d.y) - (a.y-b.y) * (c.x-d.x);
    return num / den;
}

/// returns y-value of intersection of two lines
double edge::y(const vec2 &c, const vec2 &d) const {
    T num = (a.x*b.y  -  a.y*b.x) * (c.y-d.y) - (a.y-b.y) * (c.x*d.y - c.y*d.x);
    T den = (a.x-b.x) * (c.y-d.y) - (a.y-b.y) * (c.x-d.x);
    return num / den;
}

/// returns xy-value of intersection of two lines
vec2d edge::xy(const vec2 &c, const vec2 &d) const { return { x(c, d), y(c, d) }; }

/// returns x-value of intersection of two lines
double edge::x(const edge &e) const {
    const vec2 &c = e.a;
    const vec2 &d = e.b;
    T num   = (a.x*b.y  -  a.y*b.x) * (c.x-d.x) - (a.x-b.x) * (c.x*d.y - c.y*d.x);
    T den   = (a.x-b.x) * (c.y-d.y) - (a.y-b.y) * (c.x-d.x);
    return num / den;
}

/// returns y-value of intersection of two lines
double edge::y(const edge &e) const {
    const vec2 &c = e.a;
    const vec2 &d = e.b;
    T num = (a.x*b.y  -  a.y*b.x) * (c.y-d.y) - (a.y-b.y) * (c.x*d.y - c.y*d.x);
    T den = (a.x-b.x) * (c.y-d.y) - (a.y-b.y) * (c.x-d.x);
    return num / den;
}

/// returns xy-value of intersection of two lines (via edge0 and edge1)
vec2d edge::xy(const edge &e) const { return { x(e), y(e) }; }

rect::rect(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) { }
rect::rect(const vec2 &p0, const vec2 &p1) : x(p0.x), y(p0.y), w(p1.x - p0.x), h(p1.y - p0.y) { }

rect  rect::offset(T a)                const { return { x - a, y - a, w + (a * 2), h + (a * 2) }; }
vec2d rect::sz()                       const { return { w, h }; }
vec2d rect::xy()                       const { return { x, y }; }
vec2d rect::center()                   const { return { x + w / 2.0, y + h / 2.0 }; }
bool  rect::contains(const vec2 &p)    const { return p.x >= x && p.x <= (x + w) && p.y >= y && p.y <= (y + h); }
bool  rect::operator== (const rect &r) const { return x == r.x && y == r.y && w == r.w && h == r.h; }
bool  rect::operator!= (const rect &r) const { return !operator==(r); }
rect  rect::operator + (const rect &r) const { return { x + r.x, y + r.y, w + r.w, h + r.h }; }
rect  rect::operator - (const rect &r) const { return { x - r.x, y - r.y, w - r.w, h - r.h }; }
rect  rect::operator + (const vec2 &v) const { return { x + v.x, y + v.y, w, h }; }
rect  rect::operator - (const vec2 &v) const { return { x - v.x, y - v.y, w, h }; }
rect  rect::operator * (T        r)    const { return { x * r, y * r, w * r, h * r }; }
rect  rect::operator / (T        r)    const { return { x / r, y / r, w / r, h / r }; }
      rect::operator bool()            const { return w > 0 && h > 0; }

void rect::set_rounded(const vec2 &tl, const vec2 &tr, const vec2 &br, const vec2 &bl) {
    this->r_tl = tl;
    this->r_tr = tr;
    this->r_br = br;
    this->r_bl = bl;
    this->rounded = true;
}

rect rect::clip(const rect &input) const {
    double x = math::max(x, input.x) ;
    double y = math::max(y, input.y) ;
    return rect {
        x, y,
        math::min(x + w, input.x + input.w) - x,
        math::min(y + h, input.y + input.h) - y
    };
}


}