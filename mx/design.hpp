#pragma once

template <typename T, typename = std::void_t<>> struct has_bool_op: false_type {};
template <typename T> struct has_bool_op<T, std::void_t<decltype(static_cast<bool>(std::declval<T&>()))>> : true_type {};
template <typename T> constexpr bool has_bool = has_bool_op<T>::value;

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

template<typename T, typename = void>
struct has_int_conversion : false_type {};

template<typename T>
struct has_int_conversion<T, std::void_t<decltype(static_cast<int>(std::declval<T>()))>> : true_type {};

///
template <typename T> using func    = std::function<T>;

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

/// deprecate
template <typename T, typename = void>
struct has_convert : false_type {};
template <typename T>
struct has_convert<T, std::void_t<decltype(std::declval<T>().convert((memory*)nullptr))>> : true_type {};

template <typename T, typename = void>
struct has_equals : std::false_type {};

template <typename T>
struct has_equals<T, std::enable_if_t<std::is_same<decltype(std::declval<T>() == std::declval<T>()), bool>::value>> : std::true_type {};

/// string with precision of float/double
template <typename T> std::string string_from_real(T a_value, int n = 6) {
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

constexpr bool is_debug() {
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
}

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

/// deprecate
template<typename, typename = void>
struct has_process : std::false_type {};

template<typename T>
struct has_process<T, std::void_t<decltype(std::declval<T>().process(std::declval<memory*>()))>> : std::true_type {};

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

template <typename T> struct registered_from_string<T, std::enable_if_t<std::is_same_v<decltype(T::_from_string(std::declval<T*>  (), std::declval<cstr>())), T*>>>     : true_type { };
template <typename T> struct registered_init       <T, std::enable_if_t<std::is_same_v<decltype(T::_init       (std::declval<T*>  (), std::declval<T*>  ())), void>>>   : true_type { };
template <typename T> struct registered_destruct   <T, std::enable_if_t<std::is_same_v<decltype(T::_destruct   (std::declval<T*>  (), std::declval<T*>  ())), void>>>   : true_type { };
template <typename T> struct registered_meta       <T, std::enable_if_t<std::is_same_v<decltype(T::meta        (std::declval<T*>  (), std::declval<T*>  ())), doubly>>> : true_type { };
template <typename T> struct registered_hash       <T, std::enable_if_t<std::is_same_v<decltype(T::hash        (std::declval<T*>  (), size_t(0))), size_t>>>        : true_type { };

/// externals need two args (first unused); its so we are explicit about its definition
template <typename T> struct external_assign       <T, std::enable_if_t<std::is_same_v<decltype(_assign        (std::declval<T*>(), std::declval<T*>(), std::declval<T*>())), void>>> : true_type { };
template <typename T> struct external_compare      <T, std::enable_if_t<std::is_same_v<decltype(_compare       (std::declval<T*>(), std::declval<T*>(), std::declval<T*>())), int>>>  : true_type { };
template <typename T> struct external_bool         <T, std::enable_if_t<std::is_same_v<decltype(_boolean       (std::declval<T*>(), std::declval<T*>())), bool>>>                     : true_type { };
template <typename T> struct external_copy         <T, std::enable_if_t<std::is_same_v<decltype(_copy          (std::declval<T*>(), std::declval<T*>(), std::declval<T*>(), size_t(0))), void>>> : true_type { };
template <typename T> struct external_construct    <T, std::enable_if_t<std::is_same_v<decltype(_construct     (std::declval<T*>(), std::declval<T*>(), size_t(0))), void>>>          : true_type { };

/// user must implement this one (see mx.cpp:_to_string of char*)
template <typename T> struct external_to_string    <T, std::enable_if_t<std::is_same_v<decltype(_to_string     (std::declval<T*>())), memory*>>> : true_type { };

template <typename T> struct external_from_string  <T, std::enable_if_t<std::is_same_v<decltype(_from_string   (std::declval<T*>(), std::declval<cstr>())), T*>>>   : true_type { };
template <typename T> struct external_init         <T, std::enable_if_t<std::is_same_v<decltype(_init          (std::declval<T*>(), std::declval<T*>())), void>>>   : true_type { };
template <typename T> struct external_destruct     <T, std::enable_if_t<std::is_same_v<decltype(_destruct      (std::declval<T*>(), std::declval<T*>())), void>>>   : true_type { };
template <typename T> struct external_meta         <T, std::enable_if_t<std::is_same_v<decltype(_meta          (std::declval<T*>(), std::declval<T*>())), doubly>>> : true_type { };
template <typename T> struct external_hash         <T, std::enable_if_t<std::is_same_v<decltype(_hash          (std::declval<T*>(), size_t(0))), size_t>>>  : true_type { };

template <typename T>
struct registered_instance_meta<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().meta()), doubly>>> : true_type { };
