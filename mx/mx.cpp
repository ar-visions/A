#include <mx/mx.hpp>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__linux__) || defined(__APPLE__)
#include <sys/time.h>
#endif

namespace ion {

A_impl(str, String)
A_impl(utf16, UTF16)
A_impl(field, Field)
A_impl(list, List)
A_impl(item, Item)
A_impl(hashmap, Hashmap)

int length(std::ifstream& in);

Iterator<char> String::begin() { return { chars, 0 }; }
Iterator<char> String::end()   { return { chars, length }; }

void var::push(const M& v) { v_array->push(v); }
M   &var::last()           { return v_array->last(); }

M var::parse_obj(cstr *start, type_t type) {
    if (type && type->intern) type = type->intern;
    /// supports primitive, meta-bound structs and mx objects with data type
    bool is_map = !type || (type->traits & traits::map);
    M    mx_result;
    map  map_result;

    if (!is_map) {
        void *alloc = type->ctr();
        if (type->traits & traits::mx_obj) {
            mx_result = M((A*)alloc);
        } else
            mx_result = M::pointer(type, alloc, true);
    }

    cstr cur = *start;
    assert(*cur == '{');
    ws(&(++cur));

    type_t p_type = (type && !is_map) ? type : null;

    /// read this object level, parse_values work recursively with 'cur' updated
    while (*cur != '}') {
        /// parse next field name
        M parsed = parse_quoted(&cur, null);
        M field = parsed.a_str;

        /// assert field length, skip over the ':'
        ws(&cur);
        assert(field);
        assert(*cur == ':');
        ws(&(++cur));

        prop** p = null;
        
        //assert(!p_type || (p_type->meta_map && (p = p_type->meta_map->lookup((symbol)field.mem->origin, null, null))));
        type_t chosen_t = null;
        if (p_type) {
            Field *f = (*p_type->meta)->fetch(field);
            if (!f)
                console.fault("var: field not found on destination type when parsing object: {0}", {field});
            ion::prop &prop = f->value;
            chosen_t = prop.type;
        }

        /// parse value at newly listed mx value in field, upon requesting it
        *start   = cur;
        M value = parse_value(start, chosen_t, field);
        if (is_map)
            map_result[field] = value;
        else
            M::set(mx_result, field, value); /// this is designed to give a direct object, make sure it works here
        
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
        mx_result = map_result;
    return mx_result;
}

/// no longer will store compacted data
M var::parse_arr(cstr *start, type_t type) { /// array must be a Array<T> type
    if (type && type->intern) type = type->intern;
    type_t   e_type = type;
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
            M  pv = parse_value(start, e_type, M());
            if (container)
                (*container)->push(pv);
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
        M res = (*container)->hold();
        type->f.dtr(container);
        /// free container
        return res;
    }
    return a_result;
}

void var::skip_alpha(cstr *start) {
    cstr cur = *start;
    while (*cur && isalpha(*cur))
        cur++;
    *start = cur;
}

M var::parse_value(cstr *start, type_t type, const M& field) {
    if (type && type->intern) type = type->intern;
    char first_chr = **start;
    bool numeric   =   first_chr == '-' || isdigit(first_chr);

    if (first_chr == '{') {
        /// type must have meta info
        /// issue is how to automatically prefer context, but still fall back to data
        assert(!type || type->meta);
        return parse_obj(start, type);
    } else if (first_chr == '[') {
        /// simple runtime check for array
        assert(!type || type->traits & traits::array);
        return parse_arr(start, type); /// needs to pass in array<Test3> not Test3, unless we can get the type back
    } else if (first_chr == 't' || first_chr == 'f') {
        assert(!type || type == typeof(bool));
        bool   v = first_chr == 't';
        skip_alpha(start);
        return v;
    } else if (first_chr == '"') {
        assert(!type || (type == typeof(str) || type->f.ctr_str));
        ion::symbol b64 = "data:application/octet-stream;base64,";
        int      blen = strlen(b64);
        ion::symbol start1 = (ion::symbol)&(*start)[1];
        bool   is_b64 = strncmp(start1, b64, blen) == 0;
        if (is_b64)
            *start += 1 + blen;
        
        if (!is_b64 && type == typeof(vector<u8>)) { // we already switch baed on type, we must use this still; no reducing back to array
            M file = parse_quoted(start, null);
            FILE *f = fopen(file.symbol(), "rb");
            fseek(f, 0, SEEK_END);
            ulong sz = ftell(f);
            u8   *b  = (u8*)malloc(sz);
            fseek(f, 0, SEEK_SET);
            bool read = fread(b, 1, sz, f) == sz;
            assert(read);
            vector<u8> buffer(sz);
            memcpy(buffer->data(), b, sz);
            free(b);
            return buffer;
        }
        return parse_quoted(start, is_b64 ? typeof(vector<u8>) : type); /// this updates the start cursor
    
    } else if (numeric) {
        bool floaty;
        str value = parse_numeric(start, floaty);
        assert(value != "");

        if ((floaty && !type) || type == typeof(float) || type == typeof(double)) {
            double v = double(value);
            if (type == typeof(float))
                return float(v);
            return v;
        } else if (type && (type->traits & traits::mx_obj)) {
            /// constuct from string
            void *obj = type->f.ctr_str(null, value);
            A *mem = ((A*)obj)->hold();
            type->f.dtr(obj);
            return mem;
        }

        i64 v = (i64)value;
        if (type) {
            if (type == typeof(String)) { char vc  = (char)v; return vc;   }
            if (type == typeof(i8))     { i8  vi8  = (i8)v;   return vi8;  }
            if (type == typeof(u8))     { u8  vu8  = (u8)v;   return vu8;  }
            if (type == typeof(i16))    { i16 vi16 = (i16)v;  return vi16; }
            if (type == typeof(u16))    { u16 vu16 = (u16)v;  return vu16; }
            if (type == typeof(i32))    { i32 vi32 = (i32)v;  return vi32; }
            if (type == typeof(u32))    { u32 vu32 = (u32)v;  return vu32; }
            if (type == typeof(i64))    { i64 vi64 = (i64)v;  return vi64; }
            if (type == typeof(u64))    { u64 vu64 = (u64)v;  return vu64; }
            if (type == typeof(size_t)) { size_t sz64 = (size_t)v; return sz64; }
            assert(false);
        }
        return v;
    } 

    /// symbol can be undefined or null.  stored as instance of null_t
    skip_alpha(start);
    return M(null);
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
M var::parse_quoted(cstr *cursor, type_t type) {
    ion::symbol first = *cursor;
    str    result(256);
    if (type && type->intern) type = type->intern;
    if (type == typeof(Vector<u8>)) {
        cstr start = *cursor; /// no quote
        *cursor = strchr(*cursor, '"');
        size_t len = std::distance(start, *cursor);
        result = str(size_t(len));
        memcpy(result->chars, start, len);
        result->length = len;
        result->chars[len] = 0;
        (*cursor)++;
    } else if (*first == '"') {
        char         ch = 0;
        bool last_slash = false;
        cstr      start = ++(*cursor);
        size_t     read = 0;
        for (; (ch = start[read]) != 0; read++) {
            if (ch == '\\')
                last_slash = !last_slash;
            else if (last_slash) {
                switch (ch) {
                    case 'n': ch = '\n'; break;
                    case 'r': ch = '\r'; break;
                    case 't': ch = '\t'; break;
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
        cstr cs_result = result->cs();
        if (type == typeof(Vector<u8>)) {
            vector<u8> buffer = base64::decode(cs_result, result->len());
            return buffer;
        } else
            return M::from_string(result->cs(), type);
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
    String* s = to_string();
    std::string res(s->chars, s->length);
    s->drop();
    return res;
}

M var::get(str key) {
    if (a->type != typeof(Hashmap))
        return null;
    Field *f = v_map->fetch(key);
    return f ? f->value : M();
}

/// called from path::read<var>()
M var::parse(cstr js, type_t type) {
    return parse_value(&js, type, M());
}

str var::stringify() const {
    auto encode_str = [](const str &m) -> str {
        str res(m->length * 2);
        char *s = m->chars;
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
    lambda<str(const M&)> fn;
    fn = [&](const M &i) -> str {

        /// used to output specific mx value -- can be any, may call upper recursion
        auto format_v = [&](const M &e) {
            type_t  t = e.data_type();

            /// check if this is an object with meta data; then we can describe with the above
            hashmap *meta = t->meta;
            if (meta && *meta)
                return fn(e);
            
            str res(1024);
            if (!e.a || t == typeof(null_t))
                res += "null";
            else {
                if (t == typeof(M) || t == typeof(map))
                    res += fn(e);
                else {
                    M smem = ((M&)e).to_string();
                    type_t mt = smem.type();
                    if (mt == typeof(null_t))
                        res += "null";
                    else if (t->traits & (traits::integral | traits::realistic))
                        res += smem; /// string of the number, no quotes!
                    else if (mt == typeof(String)) {
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
        str ar(1024);
        type_t t = i.type();

        /// top level fields output for meta-enabled object
        hashmap *meta = t->meta;
        if (meta && *meta) {
            if (i) {
                ar += "{";
                size_t n_fields = 0;
                for (prop &p: *meta) {
                    M value = M::get(i, p.key);//ion::hold(skey));
                    if (n_fields)
                        ar += ",";
                    ar += "\"";
                    ar += p.key->to_string();
                    ar += "\"";
                    ar += ":";
                    ar += format_v(value);
                    n_fields++;
                }
                ar += "}";
            } else {
                ar += "null";
            }
        } else if (t == typeof(Map)) {
            map m = map(i);
            ar += "{";
            int n_fields = 0;
            for (Field &f: m) {
                if (n_fields)
                    ar += ",";
                ar += "\"";
                ar += str(f.key);
                ar += "\"";
                ar += ":";
                ar += format_v(f.value);
                n_fields++;
            }
            ar += "}"; /// needs array output if below doesnt cover
        } else if (t == typeof(Array)) {
            array arr(i);
            ar += "[";
            for (size_t ii = 0; ii < arr->len(); ii++) {
                M &e = arr[ii];
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

/// default constructor constructs map
var::var()      : M(new Map()) { } /// var poses as different classes.
var::var(M b)   : M(b) { }
var::var(map m) : M(m) { }

M var::json(M i, type_t type) {
    if (type && type->intern) type = type->intern;
    type_t ty = i.type();
    cstr   cs = null;
    if (ty == typeof(String))
        cs = cstr(i.a_str->chars);
    else {
        console.fault("unsupported type: {0}", { str(ty->name) });
        return null;
    }
    return parse(cs, type);
}

String *var::to_string() {
    String* s = a->to_string();
    return s; //stringify(); /// output should be ref count of 1.
}

var::operator str() {
    return is_string() ? str(a_str->hold()) : str(stringify());
}

var::operator i64() {
    type_t t = type();
    if (t == typeof(String))
        return (i64)*a_str;
    else if (t == typeof(Value<double>))
        return (i64)(double)*v_r64;
    else if (t == typeof(Value<float>))
        return (i64)(float)*v_r32;
    else if (t == typeof(Value<i64>)) {
        i64 v = *v_i64;
        return v;
    } else if (t == typeof(null_t)) {
        return 0;
    }
    assert(false);
    return 0;
}

var::operator double() {
    type_t t = type();
    if (t == typeof(String))             return double(*a_str);
    else if (t == typeof(Value<double>)) return v_r64->value;
    else if (t == typeof(Value<i64>))    return double(v_i64->value);
    else if (t == typeof(null_t))        return 0;
    assert(false);
    return 0;
}

var::operator bool() {
    type_t t = type();
    if (t == typeof(String))             return a_str->length > 0;
    else if (t == typeof(Value<double>)) return v_r64->value > 0;
    else if (t == typeof(Value<i64>))    return v_i64->value > 0;
    return false;
}

UTF16::UTF16(size_t sz)   : A(typeof(UTF16)) {
    chars    = new char_t[sz];
    alloc    = sz;
    length   = 0;
    chars[0] = 0;
}

UTF16::UTF16(char *input) : UTF16(strlen(input)) {
    char    *i = input;
    num cursor = 0;
    while (*i) {
        chars[cursor++] = *i;
        i++;
    }
}

UTF16::UTF16(ion::symbol s) : UTF16((char*)s) { }

UTF16::UTF16(ion::wstr input, size_t len) : UTF16(len) {
    memcpy(chars, input, len * sizeof(char_t));
}

UTF16* UTF16::operator+(ion::symbol s) {
    size_t ln = strlen(s);
    UTF16 *r = new UTF16(length + ln);
    memcpy(r->chars, chars, length * 2);
    for (int i = 0; i < ln; i++)
        r->chars[length + i] = UTF16::char_t(s[i]);
    return r;
}

UTF16::char_t &UTF16::operator[](num index) const {
    if (index < 0)
        return chars[length + index];
    assert(index < length);
    return chars[index];
}

int UTF16::index_of(wchar_t find) const {
    int result = -1;
    for (int i = 0, ln = len(); i < ln; i++) {
        if (chars[i] == find) {
            result = i;
            break;
        }
    }
    return result;
}

UTF16* UTF16::escape(UTF16 *chars, char esc) const {
    int n = 0;
    for (int i = 0; i < len(); i++) {
        char x = chars[i];
        if (chars->index_of(x) > 0)
            n += 2;
        else
            n += 1;
    }
    UTF16 *res = new UTF16(n);
    if (n > 0)
    for (int i = 0; i < len(); i++) {
        char_t x = chars[i];
        if (chars->index_of(x) > 0)
            res[i] = wchar_t(esc);
        res[i] = x;
    }
    chars->drop();
    return res;
}

UTF16* UTF16::mid(num start, num slen) const {
    char_t *s  = start  >= 0 ? chars + start  : &chars[length + start];
    char_t *e  = slen   >= 0 ? chars + slen   : &chars[length + slen];
    if (e > s)
        return new UTF16();
    size_t sz = std::distance(s, e);
    UTF16* res = new UTF16 { sz };
    ///
    memcpy(res->chars, s, sz * sizeof(char_t));
    return res;
}

size_t UTF16::len() const {
    return length;
}

UTF16* UTF16::join(const array &src, UTF16* j) {
    int sz = 0;
    num cursor = 0;
    for (UTF16 &s: src) {
        sz += s.length;
        if (cursor < src->len() - 1)
            sz += j->len();
        cursor++;
    }

    UTF16* res = new UTF16(sz);
    num   pos = 0;
    cursor = 0;
    for (UTF16 &s: src) {
        memcpy(&res->chars[pos], s.chars, s.len());
        pos += s.len();
        if (cursor < src->len() - 1) {
            memcpy(&res->chars[pos], s.chars, j->len());
            pos += j->len();
        }
        cursor++;
    }
    return res;
}

ion::wstr UTF16::wstr() {
    return (ion::wstr)chars;
}

cstr UTF16::to_utf8() {
    int   sz = length;
    cstr res = (cstr)calloc(1, sz + 1);
    for (int i = 0; i < sz; i++)
        res[i] = (char)chars[i];
    res[sz] = 0;
    return res;
}

static void code_to_utf8(int code, char *output) {
    if (code > 128) {
        // If codePage is greater than 128, convert it to UTF-8
        // only characters up to 0x7FF
        if (code <= 0x7FF) {
            output[0] = 0xC0 | (code >> 6);
            output[1] = 0x80 | (code & 0x3F);
            output[2] = '\0';
        }  else if (code <= 0x7FF) {
            // 2-byte UTF-8 encoding
            output[0] = 0xC0 | ((code >> 6) & 0x1F);
            output[1] = 0x80 | (code & 0x3F);
            output[2] = '\0';
        } else if (code <= 0xFFFF) {
            // 3-byte UTF-8 encoding
            output[0] = 0xE0 | ((code >> 12) & 0x0F);
            output[1] = 0x80 | ((code >> 6) & 0x3F);
            output[2] = 0x80 | (code & 0x3F);
            output[3] = '\0';
        } else if (code <= 0x10FFFF) {
            // 4-byte UTF-8 encoding
            output[0] = 0xF0 | ((code >> 18) & 0x07);
            output[1] = 0x80 | ((code >> 12) & 0x3F);
            output[2] = 0x80 | ((code >> 6) & 0x3F);
            output[3] = 0x80 | (code & 0x3F);
            output[4] = '\0';
        } else {
            // Code point is not a valid Unicode code point
            fprintf(stderr, "Invalid code point U+%X\n", code);
            output[0] = '\0';
        }
    } else {
        output[0] = code;
        output[1] = 0;
    }
}

String* String::from_code(int code) {
    String* s = new String(16);
    if (code < 128) {
        char buf[2] = { char(code), 0 };
        s->append(buf);
    } else {
        char buf[32];
        code_to_utf8(code, buf);
        s->append(buf);
    }
    return s;
}

String::operator std::string() { return std::string(chars); }

String::String(const UTF16& u) : String(u.length) { /// utf8 is a doable object but a table must be lazy loaded at best
    for (int i = 0; i < u.length; i++)
        chars[i] = u.chars[i] <= 255 ? char(u.chars[i]) : '?';
}

String::String(int size) : A(typeof(String)) {
    chars = new char[size];
    alloc = size;
    length = 0;
    chars[0] = 0;
}

String::String(const char v) : A(typeof(String)) {
    length = 1;
    alloc  = 2;
    chars  = new char[2];
    memcpy(chars, &v, length);
    chars[length] = 0;
}

String::String(const char *v, int v_len) : A(typeof(String)) {
    length = v_len == -1 ? strlen(v) : v_len;
    alloc = 64;
    while (alloc < length)
        alloc <<= 1;
    chars = new char[64];
    memcpy(chars, v, length);
    chars[length] = 0;
}

cstr String::cs() const {
    return chars;
}

String::String(std::ifstream &in) : String(ion::length(in) + 1) {
    in.read((char*)chars, alloc - 1);
}

bool String::iequals(String* b) const { return len() == b->len() && lcase() == b->lcase(); }

int String::index_of_first(const array &elements, int *str_index) const {
    int  less  = -1;
    int  index = -1;
    for (String &find: elements) {
        ++index;
        int i = index_of(find);
        if (i >= 0 && (less == -1 || i < less)) {
            less = i;
            if (str_index)
                *str_index = index;
        }
    }
    if (less == -1 && str_index) *str_index = -1;
    return less;
}

bool String::starts_with(ion::symbol s) const {
    size_t l0 = strlen(s);
    size_t l1 = len();
    if (l1 < l0)
        return false;
    return memcmp(s, chars, l0) == 0;
}

int String::len() const {
    return length;
}

size_t String::utf_len() const {
    int ilen = utf::len((uint8_t*)chars);
    return size_t(ilen >= 0 ? ilen : 0);
}

bool String::ends_with(ion::symbol s) const {
    size_t l0 = strlen(s);
    size_t l1 = len();
    if (l1 < l0) return false;
    cstr e = &chars[l1 - l0];
    return memcmp(s, e, l0) == 0;
}

String *String::read_file(fs::path path) {
    std::ifstream in(path);
    return new String(in);
}
                                
String *String::read_file(std::ifstream& in) {
    return new String(in);
}

String *String::copy() const {
    String *res = new String(chars, length);
    return res;
}
                                
String *String::recase(bool lower) const {
    String *b  = copy();
    cstr    rp = b->chars;
    num     rc = b->length;
    int     iA = lower ? 'A' : 'a';
    int     iZ = lower ? 'Z' : 'z';
    int   base = lower ? 'a' : 'A';
    for (num i = 0; i < rc; i++) {
        char c = rp[i];
        rp[i]  = (c >= iA && c <= iZ) ? (base + (c - iA)) : c;
    }
    return b;
}

String *String::ucase() const { return recase(false); }
String *String::lcase() const { return recase(true);  } 

int String::nib_value(char c) {
    return (c >= '0' && c <= '9') ? (     c - '0') :
            (c >= 'a' && c <= 'f') ? (10 + c - 'a') :
            (c >= 'A' && c <= 'F') ? (10 + c - 'A') : -1;
}

char String::char_from_nibs(char c1, char c2) {
    int nv0 = nib_value(c1);
    int nv1 = nib_value(c2);
    return (nv0 == -1 || nv1 == -1) ? ' ' : ((nv0 * 16) + nv1);
}

String* String::replace(const String& fr, const String& to, bool all) const {
    String* sc  = (String*)this;
    cstr   sc_p = sc->chars;
    cstr   fr_p = fr.chars;
    cstr   to_p = to.chars;
    int    sc_c = math::max(int(0), sc->len() - 1);
    int    fr_c = math::max(int(0), fr.len() - 1);
    int    to_c = math::max(int(0), to.len() - 1);
    int    diff = to_c > fr_c ? math::max(int(0), to_c - fr_c) : 0;
    String *res = new String(sc_c + diff * (sc_c / fr_c + 1));
    cstr   rp   = res->chars;
    int    w    = 0;
    bool   once = true;

    /// iterate over string, check strncmp() at each index
    for (size_t i = 0; i < sc_c; ) {
        if ((all || once) && strncmp(&sc_p[i], fr_p, fr_c) == 0) {
            /// write the 'to' string, incrementing count to to_c
            memcpy (&rp[w], to_p, to_c);
            i   += to_c;
            w   += to_c;
            once = false;
        } else {
            /// write single char
            rp[w++] = sc_p[i++];
        }
    }

    /// end string, set count (count includes null char in our data)
    rp[w++] = 0;
    res->length = w;
    return res;
}

String::operator fs::path() const { return fs::path(std::string(chars));  }

String* String::combine(const String &b) const {
    String* res = new String(length + b.length + 1);
    res->append(chars);
    res->append(b.chars);
    return res;
}

bool   String::contains   (const array  &a) const { return index_of_first(a, null) >= 0;  }
String*String::operator+  (symbol        s) const { return combine(String(s));            }
bool   String::operator<  (const String &b) const { return strcmp(chars, b.chars)  <  0;  }
bool   String::operator>  (const String &b) const { return strcmp(chars, b.chars)  >  0;  }
bool   String::operator<  (symbol        b) const { return strcmp(chars, b)        <  0;  }
bool   String::operator>  (symbol        b) const { return strcmp(chars, b)	       >  0;  }
bool   String::operator<= (const String &b) const { return strcmp(chars, b.chars)  <= 0;  }
bool   String::operator>= (const String &b) const { return strcmp(chars, b.chars)  >= 0;  }
bool   String::operator<= (symbol        b) const { return strcmp(chars, b)        <= 0;  }
bool   String::operator>= (symbol        b) const { return strcmp(chars, b)        >= 0;  }
bool   String::operator== (const String &b) const { return strcmp(chars, b.chars)  == 0;  }
bool   String::operator!= (const String &b) const { return strcmp(chars, b.chars)  != 0;  }
bool   String::operator== (symbol        b) const { return strcmp(chars, b)        == 0;  }
bool   String::operator!= (symbol        b) const { return strcmp(chars, b)        != 0;  }
char&  String::operator[] (size_t        i) const { return (char&)chars[i];               }
int    String::operator[] (const String &b) const { return index_of(b);                   }
       String::operator              bool() const { return len() > 0;                     }
bool   String::operator!()                  const { return !(operator bool());            }
String* String::operator+(const String &sb) const {
    String* res = new String(length + sb.length + 1);
    res->append(chars);
    res->append(sb.chars);
    return res;
}

String* String::operator+=(const String &b) {
    append(b.chars, b.length);
    return this;
}

void String::clear() {
    chars[0] = 0;
    length = 0;
}

bool String::numeric() const {
    return length && (chars[0] == '-' || isdigit(*chars));
}

bool String::matches(const String& b) const {
    lambda<bool(cstr, cstr)> fn;
    fn = [&](cstr s, cstr p) -> bool {
        return (p &&  *p == '*') ? ((*s && fn(&s[1], &p[0])) || fn(&s[0], &p[1])) :
                (!p || (*p == *s && fn(&s[1],   &p[1])));
    };
    for (cstr ap = chars, bp = b.chars; *ap != 0; ap++)
        if (fn(ap, bp))
            return true;
    return false;
}

bool String::has_prefix(const String& i) const {
    char    *s = chars;
    size_t isz = i.length;
    size_t  sz = length;
    return  sz >= isz ? strncmp(s, i.chars, isz) == 0 : false;
}

String::String(i64 value, u8 base, int width) : String(256) {
    if (base < 2 || base > 36)
        fprintf(stderr, "str: base should be between 2 and 36\n");
    
    char  buffer[8 * sizeof(int) + 1];
    char* ptr = &buffer[sizeof(buffer) - 1];
    *ptr = '\0';

    do {
        int digit = value % base;
        value /= base;
        *--ptr = "0123456789abcdefghijklmnopqrstuvwxyz"[abs(digit)];
    } while (value != 0);

    if (value < 0) {
        *--ptr = '-';
    }

    int length = strlen(ptr);
    if (width) {
        int padding = width - length;
        if (padding > 0) {
            memmove(ptr + padding, ptr, length + 1);  // include null
            memset(ptr, '0', padding);
            length += padding;
        }
    }
    
    strcpy(chars, ptr);
    this->length = length;
}


u64 fnv1a_hash(const void* data, size_t length, u64 hash) {
    const u8* bytes = (const u8*)data;
    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];  // xor bottom with current byte
        hash *= FNV_PRIME; // multiply by FNV prime
    }
    return hash;
}

static cstr parse_fn(const std::string &cn);

static void describe_type(id *type, cstr name, sz_t sz, u64 traits);

static void push_type(id *type);

void register_type2(id *type, const std::string &sig, sz_t sz, u64 traits);

static id *primitive_type(symbol name, sz_t sz);

template <>
id *id::for_type<null_t>(void*, size_t) {
    static id* type; if (type) return type;
    type = primitive_type("std::nullptr_t", sizeof(null_t));
    return type;
}

template <>
id *id::for_type<char>(void*, sz_t) {
    static id* type; if (type) return type;
    type = primitive_type("char", sizeof(char));
    type->traits |= traits::integral;
    return type;
}

template <>
id *id::for_type<i64>(void*, sz_t) {
    static id* type; if (type) return type;
    type = primitive_type("long long", sizeof(i64));
    type->traits |= traits::integral;
    return type;
}

template <>
id *id::for_type<u64>(void*, sz_t) {
    static id* type; if (type) return type;
    type = primitive_type("unsigned long long", sizeof(u64));
    type->traits |= traits::integral;
    return type;
}


cstr parse_fn(const std::string &cn) {
    std::string      st = "with TT = ";
    std::string      en = ";";
    num		         p  = cn.find(st) + st.length();
    num              ln = cn.find(en, p) - p;
    std::string      nm = cn.substr(p, ln);
    auto             sp = nm.find(' ');
    std::string  s_name = (sp != std::string::npos) ? nm.substr(sp + 1) : nm;
    num ns = s_name.find("ion::");
    if (ns >= 0 && s_name.substr(0, ns) != "std")
        s_name = s_name.substr(ns + 5);
    int len = s_name.length();
    cstr res = (cstr)calloc(1, len + 1);
    memcpy(res, s_name.c_str(), len);
    res[len] = 0;
    return res;
}

void describe_type(id *type, cstr name, sz_t sz, u64 traits) {
    type->name      = name;
    type->traits    = traits;
    type->base_sz   = sz;
    type->src       = type;
}

void push_type(id *type) {
    M key(type->name);
    M value(M::pointer(type));
    id::types->set(key, value);
}

void register_type(id *type, const std::string &sig, sz_t sz, u64 traits) {
    std::string copy = sig;
    cstr name = parse_fn(sig);
    describe_type(type, name, sz, traits);
    push_type(type);
}

/// these are called before registration, by ident::init
id *primitive_type(symbol name, sz_t sz) {
    id *type = new id();
    memset(type, 0, sizeof(id));
    describe_type(type, (cstr)name, sz, traits::primitive);
    return type;
}

M symbolize(cstr cs, id* type, i32 id) {
    if (!type->symbols)
        type->symbols = new symbols;

    M name(cs);
    type->symbols->by_name[name] = id;
    type->symbols->by_value[id]  = name;
    type->symbols->list += new Symbol(name.symbol(), id);
    return name;
}

prop::prop(const prop &ref) {
    key         = ref.key ? new M(*ref.key) : null;
    member_addr = ref.member_addr;
    offset      = ref.offset;
    type        = ref.type;
    parent_type = ref.parent_type;
    init_value  = ref.init_value;
    is_method   = ref.is_method;
}

String* prop::to_string() {
    return (String*)(key ? key->a_str->hold() : null);
}

int Pointer::compare(const M& b) const {
    if (type->f.compare)
        return type->f.compare(ptr, b.v_pointer->ptr);
    return (size_t)ptr - (size_t)b.v_pointer->ptr;
}

prop::prop() : key(null), member_addr(0), offset(0), type(null), is_method(false) { }


void *prop::member_pointer(void *M) { return (void *)handle_t(&cstr(M)[offset]); }

symbol prop::name() const { return key->symbol(); }

int String::compare(const M &arg) const {
    String *b = arg.get<String>();
    return strcmp(chars, b->chars);
}

Vector<M>* String::split() const { /// common split, if "abc, and 123", result is "abc", "and", "123"}
    Vector<M> *res = new Vector<M>(length + 1);
    str   chars(length + 1);
    ///
    cstr pc = chars;
    for (;;) {
        char &c = *pc++;
        if  (!c) break;
        bool is_ws = isspace(c) || c == ',';
        if (is_ws) {
            if (chars) {
                *res += chars;
                chars = str(length + 1);
            }
        } else
            chars += c;
    }
    ///
    if (chars || !res)
        *res += chars;
    ///
    return res;
}

Vector<M> *String::split(const String &v) const {
    Vector<M> *res = new Vector<M>(length + 1);
    char *start  = chars;
    char *scan   = chars;
    char *sp     = v.chars;
    int   sp_len = v.length;
    assert(sp_len > 0);
    if (length > 0)
        for (;;) {
            if (*scan == 0 || strncmp(scan, sp, sp_len) == 0) {
                int str_len = (int)(size_t)(scan - start);
                if (str_len)
                    res->push(new String(start, str_len));
                scan += sp_len;
                start = scan;
                if (*scan == 0) break;
            } else
                scan++;
        }
    return res;
}

String *String::format(const array &args) {
    String   *res = new String(length + 1 + args->len() * 128);
    char   *start = chars;
    char     *end = start + length;
    for (;*start;) {
        char *f = strstr(start, "{");
        if (f) {
            if (f[1] != '{') {
                res->append(start, (int)(size_t)(f - start));
                char *s = &f[1];
                char *number_start = s;
                for (; *s && *s != '}'; s++)
                    assert(isdigit(*s));
                assert(*s == '}');
                int   number_len = (int)(size_t)(s - number_start);
                char *number     = new char[number_len + 1];
                memcpy(number, number_start, number_len);
                number[number_len] = 0;
                int n = atoi(number);
                assert(n >= 0 && n < args->len());
                delete[] number;
                M &arg = args[n];
                M str  = arg.to_string();
                if (str.a_str) {
                    char *a_start = str.a_str->chars;
                    char *a_end   = str.a_str->chars + str.a_str->length;
                    res->append(a_start, (int)(size_t)(a_end - a_start));
                } else {
                    res->append("null", 4);
                }
                start = f + 1 + number_len + 1;
            } else {
                res->append(start, (int)(size_t)(f - start) + 1);
                start += 2;
            }
        } else {
            res->append(start, (int)(size_t)(end - start));
            break;
        }
    }
    args->drop();
    return res;
}

String *Field::to_string() {
    return str("{0}, {1}");
}


Item* Hashmap::find_item(const M &key, List **h_list, u64 *phash) {
    const u64 hash = hash_value((M&)key);
    const u64 k    = hash % sz;
    if (phash) *phash = hash;
    List &hist = list_for_key(k);
    if (h_list) *h_list = (List*)&hist;
    for (Field &f: hist) {
        if (f.key.hash() != hash)
            continue;
        if (f.key == key)
            return f.value;
    }
    return null;
}

M Hashmap::lookup(const M &key) {
    Item *fi = find_item(key, null, null);
    if (!fi)
        return M();
    Field *f = fi->element.get<Field>();
    return f->value;
}

/// always creates a field with fetch
Field *Hashmap::fetch(const M &key) {
    List  *h_list = null;
    u64     hash = 0;
    Item  *fi   = find_item(key, &h_list, &hash);
    Field *f    = null;
    if (!fi) {
        f = new Field(key, null);
        Item* i = h_list->push(f); /// we iterate with a filter on hash id in doubly
        i->peer = ordered->push(f);
        count++;
    } else {
        f = fi->element.get<Field>();
    }
    return f;
}

M &Hashmap::value(const M &key) {
    Field *f = fetch(key);
    return f->value;
}

M &Hashmap::operator[](const M &key) {
    Field *f = fetch(key);
    return f->value;
}

void Hashmap::set(const M &key, const M &value) {
    Field *f = fetch(key);
    if (value.a != f->value.a)
        f->value = value;
}

bool Hashmap::remove(const M &key) {
    List *h_list = null;
    Item *fi     = find_item(key, &h_list);
    if (fi && h_list) {
        ordered->remove(fi->peer); /// weak ref, no drop needed
        h_list->remove(fi);
        count--;
        return true;
    }
    return false;
}

bool Hashmap::contains(const M &key) {
    return Hashmap::find_item(key, null);
}

Hashmap::Hashmap(int sz) : A() {
    h_fields = new List[sz];
    ordered  = new List;
    count    = 0;
    this->sz = sz;
}


M M::get(const M& o, const M& prop) {
    id   *type = o.a->type == typeof(Pointer) ? o.v_pointer->type     : o.a->type;
    u8   *ptr  = o.a->type == typeof(Pointer) ? (u8*)o.v_pointer->ptr : (u8*)o.a;
    assert(type->meta);
    hashmap &meta = *type->meta;
    assert(meta->len() > 0);
    Field *f = meta->fetch(prop);
    assert(f);
    prop &pr = f->value; /// performs a type-check
    u8 *member_ptr = &ptr[pr.offset];
    assert(pr.type->f.m_ref);
    M* ref = pr.type->f.m_ref(member_ptr);
    M cp = *ref;
    delete ref;
    return cp;
}

void M::set(const M& o, const M& prop, const M& value) {
    M ref = M::get(o, prop);
    assert(ref.a->type == typeof(Pointer));
    /// check if value is a Value<T>
    /// that is for user types, primitives, misc structs (non-A)
    if (value.a->type->traits & traits::value) {
        /// values are copy constructed
        id*  value_type =  value.v_i32->value_type;
        void* value_ptr = &value.v_i32->value; /// will be the same for any value
        assert(value_type == ref.v_pointer->type);
        if (ref.v_pointer->type->f.dtr)
            ref.v_pointer->type->f.dtr(ref.v_pointer->ptr);
        ref.v_pointer->type->f.ctr_cp(ref.v_pointer->ptr, value_ptr);
    } else {

    }
}

void id::init() {
    static id* id_types;
    if (!id_types) id_types = new id(true);
}


id::id(bool init) {
    static bool once;
    if (!once) {
        once = true;
        id::char_t = typeof(String);
        id::i64_t  = typeof(i64);
        id::u64_t  = typeof(u64);
        id::types  = new Hashmap(64);
        push_type(id::char_t); /// when Hashmap/doubly are invoked, their specialized for_type2 methods should return their info
        push_type(id::i64_t);
        push_type(id::u64_t);
    }
}


size::size(num            sz) : buffer(typeof(size)) { memset(values, 0, sizeof(values)); values[0] = sz; count = 1; }
size::size(null_t           ) : size(num(0))  { }
size::size(size_t         sz) : size(num(sz)) { }
size::size(i8             sz) : size(num(sz)) { }
size::size(u8             sz) : size(num(sz)) { }
size::size(i16            sz) : size(num(sz)) { }
size::size(u16            sz) : size(num(sz)) { }
size::size(i32            sz) : size(num(sz)) { }
size::size(u32            sz) : size(num(sz)) { }

size::size(std::initializer_list<num> args) : buffer(typeof(size)) {
    size_t i = 0;
    for (auto &v: args)
        values[i++] = v;
    count = args.size();
}

size_t size::   x() const { return values[0];    }
size_t size::   y() const { return values[1];    }
size_t size::   z() const { return values[2];    }
size_t size::   w() const { return values[3];    }
num    size::area() const {
    num v = (count < 1) ? 0 : 1;
    for (size_t i = 0; i < count; i++)
        v *= values[i];
    return v;
}
size_t size::dims() const { return count; }

void size::assert_index(const size &b) const {
    assert(count == b.count);
    for (size_t i = 0; i < count; i++)
        assert(b.values[i] >= 0 && values[i] > b.values[i]);
}

bool size::operator==(size_t b) const { return  area() ==  b; }
bool size::operator!=(size_t b) const { return  area() !=  b; }
bool size::operator==(size &sb) const { return count == sb.count && memcmp(values, sb.values, count * sizeof(num)) == 0; }
bool size::operator!=(size &sb) const { return !operator==(sb); }

void   size::operator++(int)          { values[count - 1] ++; }
void   size::operator--(int)          { values[count - 1] --; }
size  &size::operator+=(size_t sz)    { values[count - 1] += sz; return *this; }
size  &size::operator-=(size_t sz)    { values[count - 1] -= sz; return *this; }

num &size::operator[](size_t index) const {
    return (num&)values[index];
}

size &size::zero() { memset(values, 0, sizeof(values)); return *this; }

size::operator num() const { return     area();  }
size::operator  i8() const { return  i8(area()); }
size::operator  u8() const { return  u8(area()); }
size::operator i16() const { return i16(area()); }
size::operator u16() const { return u16(area()); }
size::operator i32() const { return i32(area()); }
size::operator u32() const { return u32(area()); }
//explicit size::operator i64() const { return i64(area()); }
size::operator u64() const { return u64(area()); }
size::operator r32() const { return r32(area()); }
size::operator r64() const { return r64(area()); }

num  &size::operator[](num i) { return values[i]; }

size &size::operator=(i8   i) { *this = size(i); return *this; }
size &size::operator=(u8   i) { *this = size(i); return *this; }
size &size::operator=(i16  i) { *this = size(i); return *this; }
size &size::operator=(u16  i) { *this = size(i); return *this; }
size &size::operator=(i32  i) { *this = size(i); return *this; }
size &size::operator=(u32  i) { *this = size(i); return *this; }
size &size::operator=(i64  i) { *this = size(i); return *this; }
size &size::operator=(u64  i) { *this = size(i64(i)); return *this; }

size &size::operator=(const size b) {
    memcpy(values, b.values, sizeof(values));
    count = b.count;
    return *this;
}

size_t size::index_value(const size &index) const {
    size &shape = (size &)*this;
    assert_index(index);
    num result = 0;
    for (size_t i = 0; i < count; i++) {
        num vdim = index.values[i];
        for (size_t si = i + 1; si < count; si++)
            vdim *= shape.values[si];
        
        result += vdim;
    }
    return result;
}























































logger console;

void free64(void* ptr) {
#if defined(WIN32)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void* calloc64(size_t count, size_t size) {
    // Calculate total bytes
    size_t total_bytes = count * size;

    // We'll use aligned_alloc on platforms that support it
#if defined(WIN32)
    void* ptr = _aligned_malloc(total_bytes, 64);
    if (!ptr) {
        return NULL;
    }
    memset(ptr, 0, total_bytes);
    return ptr;
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    void* ptr = aligned_alloc(64, total_bytes);
    if (!ptr) {
        return NULL;
    }
    memset(ptr, 0, total_bytes);
    return ptr;
#else
    // For platforms that don't support C11's aligned_alloc
    void* ptr = NULL;
    // posix_memalign allocates aligned memory
    if (posix_memalign(&ptr, 64, total_bytes) != 0) {
        return NULL;  // posix_memalign will return nonzero on failure
    }
    memset(ptr, 0, total_bytes);
    return ptr;
#endif
}

int length(std::ifstream& in) {
    std::streamsize base = in.tellg();
    in.seekg(0, std::ios::end);
    std::streamsize to_end = in.tellg();
    in.seekg(base);
    return int(to_end - base);
}

int snprintf(cstr str, size_t size, const char *format, ...) {
    int n;
    va_list args;
    va_start(args, format);
    
#ifdef _MSC_VER
    n = _vsnprintf_s(str, size, _TRUNCATE, format, args);
#else
    n = vsnprintf(str, size, format, args);
#endif
    
    va_end(args);
    if (n < 0 || n >= (int)size) {
        // handle error here
    }
    return n;
}

#ifdef _WIN32
struct timeval {
    long tv_sec;
    long tv_usec;
};

int gettimeofday(struct timeval* tp, struct timezone* tzp) {
    // Note: Some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME system_time;
    FILETIME file_time;
    uint64_t time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time  = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}
#endif

void wait_until(u64 to) {
    for (i64 t = microseconds(); t < to; t = microseconds()) {
        usleep(10);
    }
}

void wait_until(i64 to) {
    for (i64 t = millis(); t < to; t = millis()) {
        usleep(1000);
    }
}

i64 millis() {
    struct timeval time;
	gettimeofday(&time, NULL);
	return uint64_t(time.tv_sec) * 1000 + (time.tv_usec / 1000);
}

u64 microseconds() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return uint64_t(time.tv_sec) * 1000 * 1000 + time.tv_usec;
}

#ifdef WIN32
void usleep(__int64 usec) {
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}
#endif

bool chdir(std::string c) {
#if defined(_WIN32)
    utf16 ws(c.c_str());
    return SetCurrentDirectoryW((LPCWSTR)ws.wstr());
#else
    int ret = ::chdir(c.c_str());
    if (ret != 0) {
        printf("chdir failed: errno = %d\n", errno);
        return false;
    }
    return true;
#endif
}

}