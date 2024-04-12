#include <mx/mx.hpp>

namespace ion {

void var::push(mx v) {        array((mx*)this).push<mx>(v); }
mx  &var::last()     { return array((mx*)this).last<mx>();  }

mx var::parse_obj(cstr *start, type_t type) {
    /// supports primitive, meta-bound structs and mx objects with data type
    bool       is_map = !type || (type->traits & traits::map);
    mx      mx_result;
    map  m_result;

    if (!is_map) {
        void *alloc = type->ctr();
        /// if its an mx-based object, we need to get its memory
        if (type->traits & traits::mx_obj) {
            memory *mem = ((mx*)alloc)->mem->hold();
            mx_result = mx(mem);
            type->dtr(alloc);
        } else
            mx_result = memory::wrap(type, alloc, 1);
    }

    /// if type is not mx-based, lets say its an int type, we must wrap this parse result
    cstr cur = *start;
    assert(*cur == '{');
    ws(&(++cur));

    type_t p_type = (type && !is_map) ? type->meta_lookup() : null;

    /// read this object level, parse_values work recursively with 'cur' updated
    while (*cur != '}') {
        /// parse next field name
        mx parsed = parse_quoted(&cur, null);
        mx field = mem_symbol((symbol)parsed.mem->origin);

        /// assert field length, skip over the ':'
        ws(&cur);
        assert(field);
        assert(*cur == ':');
        ws(&(++cur));

        prop** p = null;
        
        //assert(!p_type || (p_type->meta_map && (p = p_type->meta_map->lookup((symbol)field.mem->origin, null, null))));
        type_t chosen_t = null;
        if (p_type) {
            mx addr = p_type->meta_map->value(field);
            if (!addr)
                console.fault("var: field not found on destination type when parsing object: {0}", {field});
            ion::prop *prop = (ion::prop *)addr.mem->origin;
            chosen_t = prop->type;
        }

        /// parse value at newly listed mx value in field, upon requesting it
        *start   = cur;
        mx value = parse_value(start, chosen_t);
        if (is_map)
            m_result[field] = value;
        else
            mx_result.set_meta(field, value);
        
        cur = *start;
        /// skip ws, continue past ',' if there is another
        if (ws(&cur) == ',')
            ws(&(++cur));
    }

    /// assert we arrived at end of block
    assert(*cur == '}');
    ws(&(++cur));

    *start = cur;
    if (is_map)
        mx_result = ion::hold(m_result);
    return mx_result;
}

/// no longer will store compacted data
mx var::parse_arr(cstr *start, type_t type) { /// array must be a Array<T> type
    type_t   e_type = type ? type->schema->bind->data : null;
    array *container = null;
    array a_result;

    if (type) {
        assert(type->traits & traits::typed_array);
        container = (array *)type->ctr();
    } else
        a_result = array();

    cstr cur = *start;
    assert(*cur == '[');
    ws(&(++cur));
    if (*cur == ']')
        ws(&(++cur));
    else {
        for (;;) {
            *start = cur;
            mx  pv = parse_value(start, e_type);
            if (container)
                container->push_memory(pv.mem);
            else
                a_result += pv;
            cur = *start;
            ws(&cur);
            if (*cur == ',') {
                ws(&(++cur));
                continue;
            }
            else if (*cur == ']') {
                ws(&(++cur));
                break;
            }
            assert(false);
        }
    }
    *start = cur;
    if (container) {
        memory *mem = ((array*)container)->mem->hold();
        type->f.dtr(container);
        return mx(mem);
    }
    return a_result;
}

void var::skip_alpha(cstr *start) {
    cstr cur = *start;
    while (*cur && isalpha(*cur))
        cur++;
    *start = cur;
}

mx var::parse_value(cstr *start, type_t type) {
    char first_chr = **start;
    bool numeric   =   first_chr == '-' || isdigit(first_chr);

    if (first_chr == '{') {
        /// type must have meta info
        /// issue is how to automatically prefer context, but still fall back to data
        assert(!type || type->meta_lookup());
        return parse_obj(start, type);
    } else if (first_chr == '[') {
        /// simple runtime check for array
        assert(!type || type->traits & traits::array);
        return parse_arr(start, type); /// needs to pass in array<Test3> not Test3, unless we can get the type back
    } else if (first_chr == 't' || first_chr == 'f') {
        assert(!type || type == typeof(bool));
        bool   v = first_chr == 't';
        skip_alpha(start);
        return mx(mx::alloc(&v));
    } else if (first_chr == '"') {
        assert(!type || (type == typeof(str) || type->f.ctr_str));
        symbol    b64 = "data:application/octet-stream;base64,";
        int      blen = strlen(b64);
        symbol start1 = (symbol)&(*start)[1];
        bool   is_b64 = strncmp(start1, b64, blen) == 0;
        if (is_b64)
            *start += 1 + blen;
        
        mx ret;
        if (!is_b64 && type == typeof(Array<u8>)) { // we already switch baed on type, we must use this still; no reducing back to array
            mx file = parse_quoted(start, null);
            FILE *f = fopen(file.origin<char>(), "rb");
            fseek(f, 0, SEEK_END);
            ulong sz = ftell(f);
            u8   *b  = (u8*)malloc(sz);
            fseek(f, 0, SEEK_SET);
            assert(fread(b, 1, sz, f) == sz);
            ret = memory::wrap(typeof(u8), b, sz, true);
        } else
            ret = parse_quoted(start, is_b64 ? typeof(Array<u8>) : type); /// this updates the start cursor

        return ret;
    } else if (numeric) {
        bool floaty;
        str value = parse_numeric(start, floaty);
        assert(value != "");

        if ((floaty && !type) || type == typeof(float) || type == typeof(double)) {
            real v = value.real_value<real>();
            if (type) {
                assert(type == typeof(float) || type == typeof(double));
                if (type == typeof(float)) {
                    float v32 = (float)v;
                    return mx::alloc(&v32);
                }
                return mx::alloc(&v);
            }
            return mx::alloc(&v);
        } else if (type && (type->traits & traits::mx_obj)) {
            /// handles mx enum, and all other objects that support a conversion function
            void *temp = type->ctr_mem(value.mem);
            memory *mem = ((mx*)temp)->mem->hold();
            type->f.dtr(temp);
            return mem;
        }

        i64 v = value.integer_value();
        if (type) {
            if (type == typeof(char)) { char vc  = (char)v; return mx::alloc(&vc);   }
            if (type == typeof(i8))   { i8  vi8  = (i8)v;   return mx::alloc(&vi8);  }
            if (type == typeof(u8))   { u8  vu8  = (u8)v;   return mx::alloc(&vu8);  }
            if (type == typeof(i16))  { i16 vi16 = (i16)v;  return mx::alloc(&vi16); }
            if (type == typeof(u16))  { u16 vu16 = (u16)v;  return mx::alloc(&vu16); }
            if (type == typeof(i32))  { i32 vi32 = (i32)v;  return mx::alloc(&vi32); }
            if (type == typeof(u32))  { u32 vu32 = (u32)v;  return mx::alloc(&vu32); }
            if (type == typeof(i64))  { i64 vi64 = (i64)v;  return mx::alloc(&vi64); }
            if (type == typeof(u64))  { u64 vu64 = (u64)v;  return mx::alloc(&vu64); }
            if (type == typeof(size_t))  { size_t sz64 = (size_t)v; return mx::alloc(&sz64); }
            assert(false);
        }
        return mx::alloc(&v);
    } 

    /// symbol can be undefined or null.  stored as instance of null_t
    skip_alpha(start);
    return memory::alloc(typeof(null_t), 1, 1, null);
}

str var::parse_numeric(cstr * cursor, bool &floaty) {
    cstr  s = *cursor;
    floaty  = false;
    if (*s != '-' && !isdigit(*s))
        return "";
    ///
    const int max_sane_number = 128;
    cstr      number_start = s;
    ///
    for (++s; ; ++s) {
        if (*s == '.' || *s == 'e' || *s == '-') {
            floaty = true;
            continue;
        }
        if (!isdigit(*s))
            break;
    }
    ///
    size_t number_len = s - number_start;
    if (number_len == 0 || number_len > max_sane_number)
        return null;
    
    /// 
    *cursor = &number_start[number_len];
    return str(number_start, int(number_len));
}

/// \\ = \ ... \x = \x
mx var::parse_quoted(cstr *cursor, type_t type) {
    symbol first = *cursor;
    str    result { size_t(256) };

    if (type == typeof(Array<u8>)) {
        cstr start = *cursor; /// no quote
        /// this is hacky but the string is offset from the "data:base64/header,base64,[here]
        /// so it does NOT have the start quote (has end quote).
        /// read until we see the ".  nothing else to do for base64 (not hacky!)
        *cursor = strchr(*cursor, '"');
        size_t len = std::distance(start, *cursor);
        result = str(size_t(len));
        memcpy(result.data, start, len);
        result.mem->count = len;
        result.data[len] = 0;
        (*cursor)++;
    } else if (*first == '"') {
        ///
        char         ch = 0;
        bool last_slash = false;
        cstr      start = ++(*cursor);
        
        size_t     read = 0;
        ///
        
        for (; (ch = start[read]) != 0; read++) {
            if (ch == '\\')
                last_slash = !last_slash;
            else if (last_slash) {
                switch (ch) {
                    case 'n': ch = '\n'; break;
                    case 'r': ch = '\r'; break;
                    case 't': ch = '\t'; break;
                    ///
                    default:  ch = ' ';  break;
                }
                last_slash = false;
            } else if (ch == '"') {
                read++; /// make sure cursor goes where it should, after quote, after this parse
                break;
            }
            
            if (!last_slash)
                result += ch;
        }
        *cursor = &start[read];
    }
    
    if (type) {
        cstr cs_result = result.cs();
        if (type == typeof(Array<u8>)) {
            Array<u8> buffer = base64::decode(cs_result, result.len());
            return buffer;
        } else
            return mx::from_string(result.cs(), type);
    }
    return result;
}

char var::ws(cstr *cursor) {
    cstr s = *cursor;
    while (isspace(*s))
        s++;
    *cursor = s;
    if (*s == 0)
        return 0;
    return *s;
}

var::operator std::string() {
    if(mem->type == typeof(char))
        return std::string((symbol)mem->origin, mem->count);
    return "";
}

mx *var::get(str key) {
    if (mem->type != typeof(map))
        return null;
    map::mdata *m = (map::mdata*)mem->origin;
    field *f = m->lookup(key);
    return f ? (mx*)f->value.mem->origin : null;
}

/// called from path::read<var>()
mx var::parse(cstr js, type_t type) {
    return parse_value(&js, type);
}

str var::stringify() const {
    auto encode_str = [](const mx &m) -> str {
        str res(m.count() * 2);
        char *s = (char*)m.origin<char>();
        while (*s) {
            if ((*s & 0b10000000) == 0) {
                if      (*s == '\r') res += "\\r";
                else if (*s == '\n') res += "\\n";
                else if (*s == '\"') res += "\\\"";
                else if (*s ==    8) res += "\\b";
                else if (*s ==    9) res += "\\t";
                else if (*s == 0x0C) res += "\\f";
                else res += *s;
                s++;
            } else {
                bool i4 = (*s & 0b1111100) == 0b1111000;
                bool i3 = (*s & 0b1111000) == 0b1110000;
                bool i2 = (*s & 0b1110000) == 0b1100000;
                int code = 0;
                if (i4) {
                    assert(s[1] && s[2] && s[3]);
                    code = int(0b00000111 & s[0]) << 18 |
                           int(0b00111111 & s[1]) << 12 |
                           int(0b00111111 & s[2]) <<  6 |
                           int(0b00111111 & s[3]) <<  0;
                    s += 4;
                } else if (i3) {
                    assert(s[1] && s[2]);
                    code = int(0b00001111 & s[0]) << 12 |
                           int(0b00111111 & s[1]) <<  6 |
                           int(0b00111111 & s[2]) <<  0;
                    s += 3;
                } else if (i2) {
                    assert(s[1]);
                    code = int(0b00011111 & s[0]) <<  6 |
                           int(0b00111111 & s[1]) <<  0;
                    s += 2;
                }
                char buf[32];
                snprintf(buf, sizeof(buf), "%04x", code);
                res += buf;
            }
        }
        return res;
    };

    /// main recursion function
    lambda<str(const mx&)> fn;
    fn = [&](const mx &i) -> str {

        /// used to output specific mx value -- can be any, may call upper recursion
        auto format_v = [&](const mx &e) {
            type_t  t = e.type();
            type_t vt = t->schema ? t->schema->bind->data : t;

            /// check if this is an object with meta data; then we can describe with the above
            properties *meta = vt->meta;
            if (meta && *meta)
                return fn(e);
            
            str res(size_t(1024));
            if (!e.mem || vt == typeof(null_t))
                res += "null";
            else {
                if (vt == typeof(mx) || vt == typeof(map))
                    res += fn(e);
                else {
                    mx smem = e.to_string();
                    type_t mt = smem.type();
                    if (mt == typeof(null_t))
                        res += "null";
                    else if (vt->traits & (traits::integral | traits::realistic))
                        res += smem; /// string of the number, no quotes!
                    else if (mt == typeof(char) || mt == typeof(const char)) {
                        res += "\"";
                        res += encode_str(smem);
                        res += "\"";
                    }
                    else
                        assert(false);
                }
            }
            return res;
        };
        ///
        str ar(size_t(1024));
        type_t t = i.type();

        /// top level fields output for meta-enabled object
        doubly *meta = t->schema ? t->schema->bind->data->meta : null;
        if (meta && *meta) {
            if (i) {
                ar += "{";
                size_t n_fields = 0;
                for (prop &p: meta->elements<prop>()) {
                    str &skey = *p.s_key;
                    mx  value = i.get_meta(skey);//ion::hold(skey));
                    if (n_fields)
                        ar += ",";
                    ar += "\"";
                    ar += skey;
                    ar += "\"";
                    ar += ":";
                    ar += format_v(value);
                    n_fields++;
                }
                ar += "}";
            } else {
                ar += "null";
            }
        } else if (t == typeof(map)) {
            map m = map(i);
            ar += "{";
            size_t n_fields = 0;
            for (field &fx: m.fields()) {
                str skey = str(fx.key.mem->hold());
                if (n_fields)
                    ar += ",";
                ar += "\"";
                ar += skey;
                ar += "\"";
                ar += ":";
                ar += format_v(fx.value);
                n_fields++;
            }
            ar += "}"; /// needs array output if below doesnt cover
        } else if (t == typeof(mx)) {
            mx *mx_p = i.mem->get<mx>(0);
            ar += "[";
            for (size_t ii = 0; ii < i.count(); ii++) {
                mx &e = mx_p[ii];
                if (ii)
                    ar += ",";
                ar += format_v(e);
            }
            ar += "]";
        } else
            ar += format_v(i);

        return ar;
    };

    return fn(*this);
}
/*
map var::items() {
    if (mem->type->traits & traits::map)
        return mem->hold();
    return map();
}

array var::list() {
    if (mem->type->traits & traits::array)
        return mem->hold();
    return array();
}
*/

/// default constructor constructs map
var::var()      : mx(mx::alloc<map>()) { } /// var poses as different classes.
var::var(mx b)  : mx(b.mem->hold()) { }
var::var(map m) : mx(m.mem->hold()) { }

mx var::json(mx i, type_t type) {
    type_t ty = i.type();
    cstr   cs = null;
    if (ty == typeof(u8))
        cs = cstr(i.get<u8>(0));
    else if (ty == typeof(i8))
        cs = cstr(i.get<i8>(0));
    else if (ty == typeof(char))
        cs = cstr(i.get<char>(0));
    else {
        console.fault("unsupported type: {0}", { str(ty->name) });
        return null;
    }
    return parse(cs, type);
}

memory *var::string() {
    return ion::hold(stringify()); /// output should be ref count of 1.
}

var::operator str() {
    assert(type() == typeof(char));
    return is_string() ? str(mx::mem->hold()) : str(stringify());
}

var::operator cstr() {
    assert(is_string()); /// this operator should not allocate data
    return mem->get<char>(0);
}

var::operator i64() {
    type_t t = type();
    if (t == typeof(char)) {
        return str(mx::mem->hold()).integer_value();
    } else if (t == typeof(real)) {
        i64 v = i64(*mx::get<real>());
        return v;
    } else if (t == typeof(i64)) {
        i64 v = *mx::get<i64>();
        return v;
    } else if (t == typeof(null_t)) {
        return 0;
    }
    assert(false);
    return 0;
}

var::operator real() {
    type_t t = type();
    if (t == typeof(char)) {
        return str(mx::mem->hold()).real_value<real>();
    } else if (t == typeof(real)) {
        real v = *mx::get<real>();
        return v;
    } else if (t == typeof(i64)) {
        real v = real(*mx::get<i64>());
        return v;
    } else if (t == typeof(null_t)) {
        return 0;
    }
    assert(false);
    return 0;
}

var::operator bool() {
    type_t t = type();
    if (t == typeof(char)) {
        return mx::mem->count > 0;
    } else if (t == typeof(real)) {
        real v = *mx::get<real>();
        return v > 0;
    } else if (t == typeof(i64)) {
        real v = real(*mx::get<i64>());
        return v > 0;
    }
    return false;
}
}