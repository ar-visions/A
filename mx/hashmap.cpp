#include <mx/mx.hpp>

namespace ion {

u64 hash_value(const mx &key) {
    type_t type = key.mem->type;
    raw_t  k    = key.mem->origin;

    if (type == typeof(i64)  || type == typeof(u64))    return (u64)*(u64*)&k;
    if (type == typeof(i32)  || type == typeof(u32))    return (u64)*(u32*)&k;
    if (type == typeof(cstr) || type == typeof(symbol)) return (u64)djb2(cstr(k));
    if (type->functions->hash) return type->functions->hash((none*)k, key.count());

    assert(false);
    return 0;
}

u64 hash_index(const mx &key, size_t mod) {
    return hash_value(key) % mod;
}

item* hashmap::item(const mx &key, bucket **list) const {
    const u64 k = hash_index(key, data->sz);
    const bucket &hist = (*data)[k];
    if (list)
        *list = (bucket*)&hist;
    for (ion::item &fi: hist.items()) {
        ion::field &f = *(ion::field*)fi.mem->origin;
        const mx &k = *(mx*)&f.k;
        if (k == key)
            return &fi;
    }
    return null;
}

/// always creates a field with fetch
field &hashmap::fetch(const mx &key) {
    bucket     *list = null;
    ion::item  *fi   = item(key, &list);
    ion::field *f    = null;
    if (!fi) {
        list->push(ion::field { ion::hold(key), null });
        f = &list->last<ion::field>();
    } else {
        f = fi->mem->get<ion::field>(0);
    }
    return *f;
}

mx &hashmap::operator[](const mx &key) {
    ion::field &f = fetch(key);
    return *(mx*)&f.v;
}

mx &hashmap::value(const mx &key) {
    ion::item  *fi = item(key);
    ion::field *f  = (ion::field*)fi->mem->origin;
    return f ? *(mx*)&f->v : *defaults<mx>();
}

void hashmap::set(const mx &key, const mx &value) {
    field &f = fetch(key);
    if (value.mem != f.v) {
        ion::drop(f.v);
        f.v = ion::hold(value);
    }
}

bool hashmap::remove(const mx &key) {
    bucket *list = null;
    ion::item *fi = item(key, &list);
    if (fi && list) {
        list->remove(fi);
        return true;
    }
    return false;
}

bool hashmap::contains(const mx &key) const {
    return hashmap::item(key, null);
}

}