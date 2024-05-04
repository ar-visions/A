#pragma once

template <typename T>
struct list_iter;

struct aitem:A {
    friend struct alist;
    friend struct list_iter_item;
    friend struct list_iter_item_hash;

    template <typename T>
    friend struct list_iter;

    friend struct ahashmap;

    private:
    struct aitem* next;
    struct aitem* prev;
    struct aitem* peer; /// used with ahashmap; in keyed hashed list, the item contains a location of the list item (fifo list)
    public:
    m element; /// mx is still the intermediate class, since it holds onto and can debug any of the data you give it
    aitem() : A() {
        next = 0;
        prev = 0;
    }
    aitem(const m &e) : aitem() {
        element = (m&)e;
    }
    u64 hash() {
        if (A::h) return A::h;
        return A::h = element.hash();
    }
    operator bool() { return element.a; }
};

template <typename T>
struct list_iter {
    aitem* cur;
    ///
    list_iter& operator++() { cur = cur->next; return *this; }
    list_iter& operator--() { cur = cur->prev; return *this; }

    T& operator *  () const {
        if constexpr (identical<m, T>()) {
            return cur->element;
        } else {
            static id* type = cur->element.a->type;
            if constexpr (inherits<A, T>()) {
                assert(type == typeOf(T));
                return *(T*)cur->element.a;
            } else if constexpr (has_intern<T>()) { /// this can be named is_user
                assert(type == typeOf(T));
                assert(typeOf(typename T::intern) == cur->element.a->type); /// for clarity, but we can do typeOf(T) and its the same; it resolves to the A-class
                return T(cur->element.a->hold());
            } else {
                assert(type == typeOf(Pointer));
                return *(T*)cur->element.v_pointer->ptr;
            }
        }
    }

    operator T& () const {
        return **this;
    }

    bool operator==  (const list_iter& b) const { return cur == b.cur; }
    bool operator!=  (const list_iter& b) const { return cur != b.cur; }
};

template <typename T>
struct list_iter_hash:list_iter<T> {
    u64   hash;
    using LT = list_iter<T>;
    ///
    list_iter_hash& operator++() {
        do { LT::cur = LT::cur->next; } while (LT::cur && LT::cur->hash != hash);
        return *this;
    }
    list_iter_hash& operator--() {
        do { LT::cur = LT::cur->prev; } while (LT::cur && LT::cur->hash != hash);
        return *this;
    }
};

/// iterator unique for doubly
struct list_iter_item {
    aitem* cur;
    list_iter_item(aitem *cur) : cur(cur) { }
    ///
    list_iter_item& operator++() { cur = cur->next; return *this; }
    list_iter_item& operator--() { cur = cur->prev; return *this; }

    aitem* operator *() const { return cur; }
    operator  aitem *() const { return cur; }

    bool operator==  (const list_iter_item& b) const { return cur == b.cur; }
    bool operator!=  (const list_iter_item& b) const { return cur != b.cur; }
};

/// doubly is partnered with hashmap, so an iterator filters by hash
struct list_iter_item_hash:list_iter_item {
    u64   hash;
    using LT = list_iter_item;
    list_iter_item_hash(aitem *cur, u64 hash) : list_iter_item(cur), hash(hash) {
        while (LT::cur && (LT::cur->hash() != hash))
            LT::cur = LT::cur->next;
    }

    ///
    list_iter_item_hash& operator++() {
        do { LT::cur = LT::cur->next; } while (LT::cur && LT::cur->hash() != hash);
        return *this;
    }
    list_iter_item_hash& operator--() {
        do { LT::cur = LT::cur->prev; } while (LT::cur && LT::cur->hash() != hash);
        return *this;
    }
};




template <typename T>
struct iter {
    T*      start;
    size_t  index;

    iter(T* s, size_t i) : start(s), index(i) {}  // Added constructor for initialization

    iter& operator++()       { index++; return *this; }
    iter  operator++(int)    { iter tmp = *this; ++(*this); return tmp; }  // Post-increment
    iter& operator--()       { index--; return *this; }
    iter  operator--(int)    { iter tmp = *this; --(*this); return tmp; }  // Post-decrement

    T&    operator* () const { return start[index]; }

    bool operator==(const iter& i) const { return start == i.start && index == i.index; }
    bool operator!=(const iter& i) const { return !(*this == i); }
};


/// better to use the .elements<T> on generics; we cannot have a begin() on something without a template arg
template <typename T>
struct list_iterable {
    aitem *first, *last;
    list_iterable(aitem *first, aitem *last) :
        first(first), last(last) { }
    list_iter<T> begin() const { return list_iter<T>{ first }; }
    list_iter<T>   end() const { return list_iter<T>{ null }; }
};

template <typename T>
struct list_iterable_hash:list_iterable<T> {
    u64 hash;
    list_iterable_hash(aitem *first, aitem *last, u64 hash) :
        list_iterable<T>(first, last), hash(hash) { }
    list_iter_hash<T> begin() const { return list_iter_hash<T>{ list_iterable<T>::first }; }
    list_iter_hash<T>   end() const { return list_iter_hash<T>{ null }; }
};

struct list_iterable_items {
    aitem *first, *last;
    list_iterable_items(aitem *first, aitem *last) : first(first), last(last) { }
    list_iter_item begin() const { return list_iter_item{ first }; }
    list_iter_item   end() const { return list_iter_item{ null }; }
};

struct list_iterable_items_hash:list_iterable_items {
    u64 hash;
    list_iterable_items_hash(aitem *first, aitem *last, u64 hash) :
        list_iterable_items(first, last), hash(hash) { }
    list_iter_item_hash begin() const { return list_iter_item_hash { list_iterable_items::first, hash }; }
    list_iter_item_hash   end() const { return list_iter_item_hash { null, 0 }; }
};

template <typename T>
struct iterator {
    T*      start;
    size_t  index;

    iterator(T* s, size_t i) : start(s), index(i) {}  // Added constructor for initialization

    iterator& operator++()       { index++; return *this; }
    iterator  operator++(int)    { iterator tmp = *this; ++(*this); return tmp; }  // Post-increment
    iterator& operator--()       { index--; return *this; }
    iterator  operator--(int)    { iterator tmp = *this; --(*this); return tmp; }  // Post-decrement

    T&    operator* () const { return start[index]; }

    bool operator==(const iterator& i) const { return start == i.start && index == i.index; }
    bool operator!=(const iterator& i) const { return !(*this == i); }
};