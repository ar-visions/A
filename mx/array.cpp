#include <mx/mx.hpp>

namespace ion {

bool array::operator==(const array &b) const {
    if (mx::mem == b.mem) return true;
    if (mx::mem && b.mem && mx::mem->type == b.mem->type) {
        if (mem->count != b.mem->count) return false;
        size_t sz = mem->type->base_sz;
        type_t e = element_type();
        if (e->traits & traits::mx_obj) {
            u8 *ua = (u8*)mem->origin;
            u8 *ub = (u8*)b.mem->origin;
            for (int i = 0; i < mem->count; i++) {
                mx &mx_a = *(mx*)&ua[sz * i];
                mx &mx_b = *(mx*)&ub[sz * i];
                if (mx_a.mem == mx_b.mem)
                    continue;
                if (mem->type->f.compare && mx_a.compare(mx_b) != 0)
                    return false;
                else if (memcmp(mx_a.mem->origin, mx_b.mem->origin, sz) != 0)
                    return false;
            }
            return true;
        }
        return memcmp(mem->origin, b.mem->origin, sz * mem->count) == 0;
    }
    return false;
}

}