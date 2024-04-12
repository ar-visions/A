#include <mx/mx.hpp>

namespace ion {

str::operator std::string() { return std::string(data); }

memory *str::symbolize() { return mem_symbol(data, typeof(char)); } 

str str::rand(size_t sz, char from, char to) {
    str res(sz);
    for (int i = 0; i < sz; i++)
        res += rand::uniform(from, to);
    return res;
}

str str::escape(str chars, char esc) {
    str res { len() * 2 };
    for (int i = 0; i < len(); i++) {
        char x = data[i];
        if (chars.index_of(x) > 0)
            res += '\\';
        res += x;
    }
    return res;
}

/// \\ = \ ... \x = \x
str str::parse_quoted(cstr *cursor, size_t max_len) {
    ///
    cstr first = *cursor;
    if (*first != '"')
        return "";
    ///
    bool last_slash = false;
    cstr start     = ++(*cursor);
    cstr s         = start;
    ///
    for (; *s != 0; ++s) {
        if (*s == '\\')
            last_slash = true;
        else if (*s == '"' && !last_slash)
            break;
        else
            last_slash = false;
    }
    if (*s == 0)
        return "";
    ///
    size_t len = (size_t)(s - start);
    if (max_len > 0 && len > max_len)
        return "";
    ///
    *cursor = &s[1];
    str result = "";
    for (int i = 0; i < int(len); i++)
        result += start[i];
    ///
    return result;
}

str::str(i64 value, u8 base, int width) : str(size_t(256)) {
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
            memmove(ptr + padding, ptr, length + 1);  // Include null terminator
            memset(ptr, '0', padding);
            length += padding;
        }
    }
    
    strcpy(data, ptr);
    mem->count = length;
}

str::str(utf16 d) : str(d.len()) { /// utf8 is a doable object but a table must be lazy loaded at best
    for (int i = 0; i < d.len(); i++)
        data[i] = d[i] <= 255 ? d[i] : '?';
}

/// static methods
str str::combine(const str sa, const str sb) {
    cstr       ap = (cstr)sa.data;
    cstr       bp = (cstr)sb.data;
    ///
    if (!ap) return sb.copy();
    if (!bp) return sa.copy();
    ///
    size_t    ac = sa.count();
    size_t    bc = sb.count();
    str       sc = sa.copy(((ac + bc) + 64) + math::max(sa.reserve(), sb.reserve()) * 2); /// needs to retain a reserve from the greater of either
    cstr      cp = (cstr)sc.data;
    ///
    memcpy(&cp[ac], bp, bc);
    sc.mem->count = ac + bc;
    return sc;
}

/// skip to next non-whitespace, this could be 
char &str::non_wspace(cstr cs) {
    cstr sc = cs;
    while (isspace(*sc))
        sc++;
    return *sc;
}

str::str(null_t)      : str(alloc<char>(null, 0, 16))      { }
str::str(size_t sz)   : str(alloc<char>(null, 0, sz + 1))  { }

str::str(std::ifstream &in) : str(alloc<char>(null, 0, ion::length(in) + 1)) {
    mem->count = mem->reserve - 1;
    in.read((char*)data, mem->count);
}

void str::code_to_utf8(int code, char *output) {
    if (code > 128) {
        // If codePage is greater than 128, convert it to UTF-8
        // For simplicity, this example handles only characters up to 0x7FF.
        if (code <= 0x7FF) {
            output[0] = 0xC0 | (code >> 6);
            output[1] = 0x80 | (code & 0x3F);
            output[2] = '\0';
        }  else if (code <= 0x7FF) {
            // Handle 2-byte UTF-8 encoding
            output[0] = 0xC0 | ((code >> 6) & 0x1F);
            output[1] = 0x80 | (code & 0x3F);
            output[2] = '\0';
        } else if (code <= 0xFFFF) {
            // Handle 3-byte UTF-8 encoding
            output[0] = 0xE0 | ((code >> 12) & 0x0F);
            output[1] = 0x80 | ((code >> 6) & 0x3F);
            output[2] = 0x80 | (code & 0x3F);
            output[3] = '\0';
        } else if (code <= 0x10FFFF) {
            // Handle 4-byte UTF-8 encoding
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

str str::from_code(int code) {
    str s { size_t(32) };
    if (code < 128)
        s[0] = code;
    else {
        code_to_utf8(code, s.data);
    }
    s.mem->count = strlen(s.data);
    return s;
}

str str::from_integer(i64 i) { return str(memory::string(std::to_string(i))); }

str::str(float  f32, int n) : str(memory::string(string_from_real(f32, n))) { }
str::str(double f64, int n) : str(memory::string(string_from_real(f64, n))) { }
str::str(symbol cs, size_t len, size_t rs) : str(cstring((cstr)cs, len, rs)) { }
//str::str(cstr   cs, size_t len, size_t rs) : str(cstring(      cs, len, rs)) { }
str::str(std::string s) : str(cstr(s.c_str()), s.length()) { }

cstr str::cs() const { return cstr(data); }

str str::expr(lambda<str(str)> fn) const {
    cstr   pa = data;
    auto   be = [&](int i) -> bool { return pa[i] == '{' && pa[i + 1] != '{'; };
    auto   en = [&](int i) -> bool { return pa[i] == '}'; };
    bool   in = be(0);
    int    fr = 0;
    size_t ln = byte_len();
    static size_t static_m = 4;
    for (;;) {
        bool exploding = false;
        size_t sz      = math::max(size_t(1024), ln * static_m);
        str    rs      = str(sz);
        for (int i = 0; i <= int(ln); i++) {
            if (i == ln || be(i) || en(i)) {
                int  cr = int(i - fr);
                if (cr > 0) {
                    if (in) {
                        str exp_in = str(&pa[fr], cr);
                        str out = fn(exp_in);
                        if ((rs.byte_len() + out.byte_len()) > sz) {
                            exploding = true;
                            break;
                        }
                        rs += out;
                    } else {
                        str out = str(&pa[fr], cr);
                        if ((rs.byte_len() + out.byte_len()) > sz) {
                            exploding = true;
                            break;
                        }
                        rs += out;
                    }
                }
                fr = i + 1;
                in = be(i);
            }
        }
        if (exploding) {
            static_m *= 2;
            continue;
        }
        return rs;

    }
    return null;
}

/// format is a user of expr
str str::format(const Array<mx> &args) const {
    return expr([&](str e) -> str {
        size_t index = size_t(e.integer_value());
        if (index >= 0 && index < args.len()) {
            mx     &a    = args.get<mx>(index);
            return a.to_string();
        }
        return null;
    });
}

/// just using cs here, for how i typically use it you could cache the strings
str str::format(symbol cs, const Array<mx> &args) {
    return str(cs).format(args);
}

str::operator fs::path() const { return fs::path(std::string(data));  }

void str::clear() const {
    if (mem) {
        if (mem->refs == 1)
            mem->count = *data = 0;
    }
}

bool  str::contains   (const array &a) const { return index_of_first(a, null) >= 0; }
str   str::operator+  (symbol       s) const { return combine(*this, (cstr )s);     }
bool  str::operator<  (const str   &b) const { return strcmp(data, b.data) <  0;    }
bool  str::operator>  (const str   &b) const { return strcmp(data, b.data) >  0;    }
bool  str::operator<  (symbol       b) const { return strcmp(data, b)      <  0;    }
bool  str::operator>  (symbol       b) const { return strcmp(data, b)	     >  0;   }
bool  str::operator<= (const str   &b) const { return strcmp(data, b.data) <= 0;    }
bool  str::operator>= (const str   &b) const { return strcmp(data, b.data) >= 0;    }
bool  str::operator<= (symbol       b) const { return strcmp(data, b)      <= 0;    }
bool  str::operator>= (symbol       b) const { return strcmp(data, b)      >= 0;    }
//bool  str::operator== (std::string  b) const { return strcmp(data, b.c_str()) == 0;  }
//bool  str::operator!= (std::string  b) const { return strcmp(data, b.c_str()) != 0;  }
bool  str::operator== (const str   &b) const { return strcmp(data, b.data)  == 0;  }
bool  str::operator!= (const str   &b) const { return strcmp(data, b.data)  != 0;  }
bool  str::operator== (symbol       b) const { return strcmp(data, b)       == 0;  }
bool  str::operator!= (symbol       b) const { return strcmp(data, b)       != 0;  }
char& str::operator[] (size_t       i) const { return (char&)data[i];               }
int   str::operator[] (const str   &b) const { return index_of(b);                  }
        str::operator             bool() const { return count() > 0;                  }
bool  str::operator!()                 const { return !(operator bool());           }
str   str::operator+  (const str &sb)  const { return combine(*this, sb);           }

str &str::operator+=(const str &b) {
    if (mem->reserve >= (mem->count + b.mem->count) + 1) {
        cstr    ap =   data;
        cstr    bp = b.data;
        size_t  bc = b.mem->count;
        size_t  ac =   mem->count;
        memcpy(&ap[ac], bp, bc); /// when you think of data size changes, think of updating the count. [/mops-away]
        ac        += bc;
        ap[ac]     = 0;
        mem->count = ac;
    } else {
        *this = combine(*this, b);
    }
    
    return *this;
}

str &str::operator+= (const char b) {
    if (mem->reserve >= (mem->count + 1) + 1) {
        memcpy(&data[mem->count], &b, 1); /// when you think of data size changes, think of updating the count. [/mops-away]
        data[++mem->count] = 0;
    } else {
        char v[2] = { b, 0 };
        *this = combine(*this, str((symbol)v));
    }
    return *this;
}

str &str::operator+= (symbol b) { return operator+=(str((cstr )b)); } /// not utilizing cchar_t yet.  not the full power.

bool str::iequals(str b) const { return len() == b.len() && lcase() == b.lcase(); }

int str::index_of_first(const array &elements, int *str_index) const {
    int  less  = -1;
    int  index = -1;
    assert(elements.element_type() == typeof(str));
    for (str &find: elements.elements<str>()) {
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

bool str::starts_with(symbol s) const {
    size_t l0 = strlen(s);
    size_t l1 = len();
    if (l1 < l0)
        return false;
    return memcmp(s, data, l0) == 0;
}

size_t str::len() const {
    return count();
}

size_t str::utf_len() const {
    int ilen = utf::len((uint8_t*)data);
    return size_t(ilen >= 0 ? ilen : 0);
}

bool str::ends_with(symbol s) const {
    size_t l0 = strlen(s);
    size_t l1 = len();
    if (l1 < l0) return false;
    cstr e = &data[l1 - l0];
    return memcmp(s, e, l0) == 0;
}

str str::read_file(fs::path path) {
    std::ifstream in(path);
    return str(in);
}
                                
str str::read_file(std::ifstream& in) {
    return str(in);
}
                                
str str::recase(bool lower) const {
    str     b  = copy();
    cstr    rp = b.data;
    num     rc = b.byte_len();
    int     iA = lower ? 'A' : 'a';
    int     iZ = lower ? 'Z' : 'z';
    int   base = lower ? 'a' : 'A';
    for (num i = 0; i < rc; i++) {
        char c = rp[i];
        rp[i]  = (c >= iA && c <= iZ) ? (base + (c - iA)) : c;
    }
    return b;
}

str str::ucase() const { return recase(false); }
str str::lcase() const { return recase(true);  } 

int str::nib_value(char c) {
    return (c >= '0' && c <= '9') ? (     c - '0') :
            (c >= 'a' && c <= 'f') ? (10 + c - 'a') :
            (c >= 'A' && c <= 'F') ? (10 + c - 'A') : -1;
}

char str::char_from_nibs(char c1, char c2) {
    int nv0 = nib_value(c1);
    int nv1 = nib_value(c2);
    return (nv0 == -1 || nv1 == -1) ? ' ' : ((nv0 * 16) + nv1);
}

str str::replace(str fr, str to, bool all) const {
    str&   sc   = (str&)*this;
    cstr   sc_p = sc.data;
    cstr   fr_p = fr.data;
    cstr   to_p = to.data;
    size_t sc_c = math::max(size_t(0), sc.byte_len() - 1);
    size_t fr_c = math::max(size_t(0), fr.byte_len() - 1);
    size_t to_c = math::max(size_t(0), to.byte_len() - 1);
    size_t diff = to_c > fr_c ? math::max(size_t(0), to_c - fr_c) : 0;
    str    res  = str(sc_c + diff * (sc_c / fr_c + 1));
    cstr   rp   = res.data;
    size_t w    = 0;
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
    res.mem->count = w;

    /// validate allocation and write
    assert(w <= res.mem->reserve);
    return res;
}

/// mid = substr; also used with array so i thought it would be useful to see them as same
str str::mid(num start, num len) const {
    int ilen = int(count());
    assert(std::abs(start) <= ilen);
    if (start < 0) start = ilen + start;
    int cp_count = len < 0 ? (ilen - start) : len;
    assert(start + cp_count <= ilen);
    return str(&data[start], cp_count);
}

iter<char>   str::begin() { return { data, 0 }; }
iter<char>   str::end()   { return { data, size_t(byte_len()) }; }

array str::split(symbol s) const { return split(str(s)); }
array str::split() { /// common split, if "abc, and 123", result is "abc", "and", "123"}
    array result;
    str   chars(mem->count + 1);
    ///
    cstr pc = data;
    for (;;) {
        char &c = *pc++;
        if  (!c) break;
        bool is_ws = isspace(c) || c == ',';
        if (is_ws) {
            if (chars) {
                result += chars.copy();
                chars = str(mem->count + 1);
            }
        } else
            chars += c;
    }
    ///
    if (chars || !result)
        result += chars;
    ///
    return result;
}

int str::index_of(char b, int from) const {
    if (!mem || mem->count == 0) return -1;
    
    cstr   ap =   data;
    int    ac = int(mem->count);

    if (from < 0) {
        for (int index = ac - 1 + from + 1; index >= 0; index--) {
            if (ap[index] == b)
                return index;
        }
    } else {
        for (int index = from; index <= ac - 1; index++)
            if (ap[index] == b)
                return index;
    }
    return -1;
}

int str::index_of(str b, int from) const {
    if (!b.mem || b.mem->count == 0) return  0;
    if (!  mem ||   mem->count == 0) return -1;

    cstr   ap =   data;
    cstr   bp = b.data;
    int    ac = int(  mem->count);
    int    bc = int(b.mem->count);
    
    /// dont even try if b is larger
    if (bc > ac) return -1;

    /// search for b, reverse or forward dir
    if (from < 0) {
        for (int index = ac - bc + from + 1; index >= 0; index--) {
            if (strncmp(&ap[index], bp, bc) == 0)
                return index;
        }
    } else {
        for (int index = from; index <= ac - bc; index++)
            if (strncmp(&ap[index], bp, bc) == 0)
                return index;
    }
    
    /// we aint found ****. [/combs]
    return -1;
}

int str::index_of(MatchType ct, symbol mp) const {
    int  index = 0;
    
    using Fn = func<bool(const char &)>;
    static umap<MatchType, Fn> match_table {
        { Alpha,     Fn([&](const char &c) -> bool { return  isalpha (c); }) },
        { Numeric,   Fn([&](const char &c) -> bool { return  isdigit (c); }) },
        { WS,        Fn([&](const char &c) -> bool { return  isspace (c); }) }, // lambda must used copy-constructed syntax
        { Printable, Fn([&](const char &c) -> bool { return !isspace (c); }) },
        { String,    Fn([&](const char &c) -> bool { return  strcmp  (&c, mp) == 0; }) },
        { CIString,  Fn([&](const char &c) -> bool { return  strcmp  (&c, mp) == 0; }) }
    };
    
    /// msvc thinks its ambiguous, so i am removing this iterator from str atm.
    cstr pc = data;
    for (;;) {
        char &c = pc[index];
        if  (!c)
            break;
        if (match_table[ct](c))
            return index;
        index++;
    }
    return -1;
}

i64 str::integer_value() const {
    return ion::integer_value(mx::mem);
}

bool str::has_prefix(str i) const {
    char    *s = data;
    size_t isz = i.byte_len();
    size_t  sz =   byte_len();
    return  sz >= isz ? strncmp(s, i.data, isz) == 0 : false;
}

bool str::numeric() const {
    return byte_len() && (data[0] == '-' || isdigit(*data));
}

bool str::matches(str input) const {
    lambda<bool(cstr, cstr)> fn;
    str&  a = (str &)*this;
    str&  b = input;
            fn = [&](cstr s, cstr p) -> bool {
            return (p &&  *p == '*') ? ((*s && fn(&s[1], &p[0])) || fn(&s[0], &p[1])) :
                    (!p || (*p == *s && fn(&s[1],   &p[1])));
    };
    for (cstr ap = a.data, bp = b.data; *ap != 0; ap++)
        if (fn(ap, bp))
            return true;
    return false;
}

str str::trim() const {
    cstr  s = data;
    int   c = int(byte_len());
    int   h = 0;
    int   t = 0;

    while (isspace(s[h]))
        h++;

    while (isspace(s[c - 1 - t]) && (c - t) > h)
        t++;

    return str(&s[h], c - (h + t));
}

size_t str::reserve() const { return mx::mem->reserve; }

utf16::utf16(size_t sz)   : utf16(mx::alloc<u16>(null, sz, sz)) { }

utf16::utf16(char *input) : utf16(strlen(input)) {
    char *i = input;
    num cursor = 0;
    while (*i) {
        data[cursor++] = *i;
        i++;
    }
}

utf16::utf16(symbol s) : utf16((char*)s) { }

utf16::utf16(ion::wstr input, size_t len) : utf16(len) {
    memcpy(data, input, len * sizeof(char_t));
}

utf16 utf16::operator+(symbol s) {
    size_t ln = strlen(s);
    utf16  r { mem->count + ln };
    memcpy( r.data, data, mem->count * 2 );
    memcpy(&r.data[mem->count], s, ln );
    return r;
}


utf16::char_t &utf16::operator[](num index) const {
    if (index < 0)
        return data[mem->count + index];
    return data[index];
}

int utf16::index_of(wchar_t find) const {
    int result = -1;
    for (int i = 0, ln = len(); i < ln; i++) {
        if (data[i] == find) {
            result = i;
            break;
        }
    }
    return result;
}

utf16 utf16::escape(utf16 chars, char esc) const { // probably not the greatest
    size_t n = 0;
    for (int i = 0; i < len(); i++) {
        char x = data[i];
        if (chars.index_of(x) > 0)
            n += 2;
        else
            n += 1;
    }
    if (!n)
        return utf16();
    utf16 res { n };
    for (int i = 0; i < len(); i++) {
        char_t x = data[i];
        if (chars.index_of(char(x)) > 0)
            res[i] = wchar_t(esc);
        res[i] = x;
    }
    return res;
}

utf16 utf16::mid(num start, num length) const {
    char_t *s  = start  >= 0 ? data + start  : &data[mem->count + start ];
    char_t *e  = length >= 0 ? data + length : &data[mem->count + length];
    if (e > s)
        return utf16();
    size_t sz = std::distance(s, e);
    utf16  res { sz };
    ///
    memcpy(res.data, s, sz * sizeof(char_t));
    return res;
}

size_t utf16::len() const {
    return mem ? mem->count : 0;
}

utf16 utf16::join(const array &src, utf16 j) {
    assert(src.element_type() == typeof(utf16));
    size_t sz = 0;
    num cursor = 0;
    for (utf16 &s: src.elements<utf16>()) {
        sz += s.mem->count;
        if (cursor < src.len() - 1)
            sz += j.len();
        cursor++;
    }

    utf16 res { sz };
    num   pos = 0;
    cursor = 0;
    for (utf16 &s: src.elements<utf16>()) {
        memcpy(&res.data[pos], s.data, s.len());
        pos += s.len();
        if (cursor < src.len() - 1) {
            memcpy(&res.data[pos], s.data, j.len());
            pos += j.len();
        }
        cursor++;
    }
    return res;
}
}