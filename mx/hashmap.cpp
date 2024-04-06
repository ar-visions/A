#include <mx/mx.hpp>

namespace ion {

u64 hash_value(const mx &key) {
    return key.hash();
}

u64 hash_index(const mx &key, size_t mod) {
    return hash_value(key) % mod;
}

item* hashmap::item(const mx &key, bucket **list, u64 *phash) const {
    const u64 hash = hash_value(key);
    const u64 k    = hash % data->sz;
    if (phash) *phash = hash;
    const bucket &hist = (*data)[k];
    if (list) *list = (bucket*)&hist;
    for (ion::item &fi: hist.items(hash)) {
        ion::field &f = *(ion::field*)fi.mem->origin;
        if ((const mx &)f.key == key)
            return &fi;
    }
    return null;
}

/// always creates a field with fetch
field &hashmap::fetch(const mx &key) {
    bucket     *list = null;
    u64         hash = 0;
    ion::item  *fi   = item(key, &list, &hash);
    ion::field *f    = null;
    if (!fi) {
        list->push(ion::field { ion::hold(key), null }, hash); /// we iterate with a filter on hash id in doubly
        f = &list->last<ion::field>();
    } else {
        f = fi->mem->get<ion::field>(0);
    }
    return *f;
}

ion::field &hashmap::fetch(cstr key) {
    u64 hash = djb2(cstr(key));
    u64 k    = hash % data->sz;
    bucket &list = (*data)[k];
    for (ion::field &f: list.elements<ion::field>(hash)) { /// looked up by mod of hash, filtering by entire hash u64
        if (strcmp((cstr)f.key.mem->origin, key) == 0)
            return f;
    }
    return list.push(ion::field { mem_symbol((symbol)key, ident::char_t), null }, hash);
}

mx &hashmap::operator[](const mx &key) {
    ion::field &f = fetch(key);
    return f.value;
}

mx &hashmap::value(const mx &key) {
    ion::item  *fi = item(key);
    ion::field *f  = (ion::field*)fi->mem->origin;
    return f ? (mx &)f->value : *defaults<mx>();
}

void hashmap::set(const mx &key, const mx &value) {
    field &f = fetch(key);
    if (value.mem != f.value.mem) {
        ion::drop(f.value);
        f.value = ion::hold(value);
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

bool hashmap::contains(cstr key) const {
    u64 hash = djb2(cstr(key));
    u64 k = hash % data->sz;
    bucket &hist = (*data)[k];
    for (ion::field &f: hist.elements<ion::field>(hash)) {
        /// store the hash key in item, so that field/item iteration can be filtered
        if (strcmp((cstr)f.key.mem->origin, key) == 0)
            return true;
    }
    return false;
}

bool hashmap::contains(const mx &key) const {
    return hashmap::item(key, null);
}

static idata hmdata_t {
    .name    = (cstr)"hmdata",
    .base_sz = sizeof(hmdata),
    .traits  = traits::mx_obj | traits::primitive
};

hashmap::hashmap(size_t sz) : mem(memory::alloc(&hmdata_t)), data((hmdata*)mem->origin) {
    data->h_pairs = (bucket*)calloc64(sz, sizeof(bucket)); /// inits from zero
    data->sz = sz;
}

hashmap& hashmap::operator=(hashmap &a) {
    ion::drop(mem);
    mem = ion::hold(a.mem);
    return *this;
}


}