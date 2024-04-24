#include <mx/mx.hpp>

namespace ion {

static cstr parse_fn(const std::string &cn) {
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
}

static void describe_type(memory *mem, cstr name, sz_t sz, u64 traits) {
    type_t type     = (type_t)mem->origin;
    type->mem       = mem;
    mem ->type      = type;
    type->name      = name;
    type->traits    = traits;
    type->base_sz   = sz;
    type->src       = type;
}

static void push_type(memory *mem) {
    type_t type = mem->type;
    mx key = memory::window(ident::char_t, type->name, strlen(type->name));
    mx value(hold(mem));
    ident::types->set(key, value);
}

void register_type(memory *mem, const std::string &sig, sz_t sz, u64 traits) {
    std::string copy = sig;
    type_t type = mem->type;
    cstr name = parse_fn(sig);
    describe_type(mem, name, sz, traits);
    push_type(mem);
}

/// these are called before registration, by ident::init
static memory *primitive_type(symbol name, sz_t sz) {
    memory *mem = memory::raw_alloc((idata*)0, sizeof(idata), 1, 1); /// dont need to specify type to raw_alloc if we know sz
    describe_type(mem, (cstr)name, sz, traits::primitive);
    return mem;
}

ident::ident(bool init) {
    static bool once;
    if (!once) {
        once = true;
        ident::char_t = typeof(char);
        ident::i64_t  = typeof(i64);
        ident::u64_t  = typeof(u64);
        ident::types  = new hashmap(64);
        push_type(ident::char_t->mem); /// when hashmap/doubly are invoked, their specialized for_type2 methods should return their info
        push_type(ident::i64_t ->mem);
        push_type(ident::u64_t ->mem);
    }
}

template <>
idata *ident::for_type2<null_t>(void*, sz_t) {
    static type_t type; if (type) return type;
    memory *mem = primitive_type("std::nullptr_t", sizeof(null_t));
    type = mem->type;
    return type;
}

template <>
idata *ident::for_type2<char>(void*, sz_t) {
    static type_t type; if (type) return type;
    memory *mem = primitive_type("char", sizeof(char));
    type = mem->type;
    return type;
}

template <>
idata *ident::for_type2<i64>(void*, sz_t) {
    static type_t type; if (type) return type;
    memory *mem = primitive_type("long long", sizeof(i64));
    type = mem->type;
    return type;
}

template <>
idata *ident::for_type2<u64>(void*, sz_t) {
    static type_t type; if (type) return type;
    memory *mem = primitive_type("unsigned long long", sizeof(u64));
    type = mem->type;
    return type;
}

}