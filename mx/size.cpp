#include <mx/mx.hpp>

namespace ion {

size::size(num        sz = 0) { memset(values, 0, sizeof(values)); values[0] = sz; count = 1; }
size::size(null_t           ) : size(num(0))  { }
size::size(size_t         sz) : size(num(sz)) { }
size::size(i8             sz) : size(num(sz)) { }
size::size(u8             sz) : size(num(sz)) { }
size::size(i16            sz) : size(num(sz)) { }
size::size(u16            sz) : size(num(sz)) { }
size::size(i32            sz) : size(num(sz)) { }
size::size(u32            sz) : size(num(sz)) { }
size::size(std::initializer_list<num> args) : base() {
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
explicit size::operator  i8() const { return  i8(area()); }
explicit size::operator  u8() const { return  u8(area()); }
explicit size::operator i16() const { return i16(area()); }
explicit size::operator u16() const { return u16(area()); }
explicit size::operator i32() const { return i32(area()); }
explicit size::operator u32() const { return u32(area()); }
//explicit size::operator i64() const { return i64(area()); }
explicit size::operator u64() const { return u64(area()); }
explicit size::operator r32() const { return r32(area()); }
explicit size::operator r64() const { return r64(area()); }

num  &size::operator[](num i) { return values[i]; }

size &size::operator=(i8   i) { *this = size(i); return *this; }
size &size::operator=(u8   i) { *this = size(i); return *this; }
size &size::operator=(i16  i) { *this = size(i); return *this; }
size &size::operator=(u16  i) { *this = size(i); return *this; }
size &size::operator=(i32  i) { *this = size(i); return *this; }
size &size::operator=(u32  i) { *this = size(i); return *this; }
size &size::operator=(i64  i) { *this = size(i); return *this; }
size &size::operator=(u64  i) { *this = size(i64(i)); return *this; }

size &size::operator=(const size b);

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

}