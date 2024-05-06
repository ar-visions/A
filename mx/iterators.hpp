#pragma once

template <typename T>
struct Iterator {
    Item*  i_cur;
    type_t e_type;
    u8*    e_start;
    size_t e_size;
    size_t e_index;

    Iterator(Item *i) : i_cur(i), e_type(null), e_start(null), e_size(0), e_index(0) { }

    Iterator(T *start, int index) : i_cur(null), e_type(typeof(T)), e_start((u8*)start), e_size(sizeof(T)), e_index(index) { }

    Iterator& operator++() {
        if (e_start) {
            e_index++;
        } else {
            i_cur = i_cur->next;
        }
        return *this;
    }

    Iterator& operator--() {
        if (e_start) {
            e_index--;
        } else {
            i_cur = i_cur->prev;
        }
        return *this;
    }

    T& operator *() const {
        if (e_start) {
            u8 *src = &e_start[e_size * e_index];
            assert(e_type == typeof(T));
            return *(T*)src;
        } else {
            if constexpr (identical<M, T>())
                return i_cur->element;
            else {
                static id* i_type = i_cur->element.a->type;
                if (i_type == typeof(Pointer))
                    return *(T*)i_cur->element.v_pointer->ptr;
                else
                    return *(T*)i_cur->element.a;
            }
        }
    }

    operator T& () const {
        return **this;
    }

    bool operator==  (const Iterator& b) const {
        if (e_start)
            return e_index == b.e_index;
        else
            return i_cur == b.i_cur;
    }
    bool operator!=(const Iterator& b) const { return !operator==(b); }
};