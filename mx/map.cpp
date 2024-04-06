#include <mx/mx.hpp>

namespace ion {

void map::print() {
    for (field &f: fields()) {
        str k = str(ion::hold(f.key));
        str v = str(ion::hold(f.value));
        console.log("key: {0}, value: {1}", array { k, v });
    }
}

/// boolean operator for key
bool map::mdata::operator()(const mx &key) {
    if (hash_map)
        return hash_map->contains(key);
    else {
        for (field &f:fields.elements<field>())
            if ((const mx &)f.key == key.mem)
                return true;
    }
    return false; 
}

field &map::mdata::fetch(const mx &k) {
    field *f = lookup(k);
    if (f) return *f;
    ///
    fields += field { k.mem, (memory*)null };
    field &last = fields->last<field>();

    if (hash_map && *hash_map) (*hash_map)[k] = (field*)&last;

    return last;
}

field &map::mdata::first()  { return fields->first<field>(); }
field &map::mdata:: last()  { return fields-> last<field>(); }
size_t map::mdata::count()  { return fields->len();   }
size_t map::mdata::  len()  { return fields->len();   }

size_t    map::mdata::count(const mx &k) {
    type_t tk = k.type();
    memory *b = (memory*)k.mem;
    if (!(k.mem->attrs & memory::constant) && (tk->traits & traits::primitive)) {
        size_t res = 0;
        for (field &f:fields.elements<field>()) {
            memory *a = f.key.mem;
            assert(a->type->size() == b->type->size());
            if (a == b || (a->count == b->count && memcmp(a->origin, b->origin, a->count * a->type->size()) == 0)) // think of new name instead of type_* ; worse yet is handle base types in type
                res++;
        }
        return res;
    } else {
        size_t res = 0;
        for (field &f:fields.elements<field>())
            if ((const mx &)f.key == k)
                res++;
        return res;
    }
}

size_t map::mdata::count(cstr cs) {
    size_t res = 0;
    for (field &f:fields.elements<field>())
        if (strcmp(symbol(f.key.mem->origin), symbol(cs)) == 0)
            res++;
    return res;
}

field *map::mdata::lookup(const mx &k) const {
    if (hash_map) {
        item *pf = hash_map->item(k);
        return pf ? pf->mem->get<field>(0) : null;
    } else {
        for (field & f : fields.elements<field>())
            if ((const mx &)f.key == k)
                return &f;
    }
    return null;
}

bool map::mdata::remove(const mx &k) {
    bool removed = false;
    if (hash_map) {
        removed = hash_map->remove(k);
    } else {
        int index = 0;
        for (field & f : fields.elements<field>()) {
            if ((const mx &)f.key == k) {
                fields->remove(index);
                removed = true;
                break;
            }
            index++;
        }
    }
    return removed;
}

/// should not CONVERT. this is just copying the key used; should assert too.
array map::mdata::keys() {
    if (!fields)
        return array {};
    type_t key_t = fields->first<field>().key.mem->type;
    array res { key_t, fields->len() };
    for (field &f:fields.elements<field>())
        res.push(ion::hold(f.key));
    return res;
}

ldata::literable<field> map::mdata::elements() const {
    return fields->elements<field>();
}

mx &map::mdata::operator[](const mx &key) {
    return *(mx*)&fetch(key).value;
}

map::mdata::operator bool() { return ((hash_map && hash_map->len() > 0) || (fields->len() > 0)); }
    map map::parse(int argc, cstr *argv, map &def) {
        map iargs = map();
        for (int ai = 0; ai < argc; ai++) {
            cstr ps = argv[ai];
            ///
            if (ps[0] == '-') {
                bool   is_single = ps[1] != '-';
                mx key {
                    cstr(&ps[is_single ? 1 : 2]), typeof(char)
                };
                field* found;
                if (is_single) {
                    for (field &df: def.fields()) {
                        symbol s = symbol(df.key.mem->origin);
                        if (ps[1] == s[0]) {
                            found = &df;
                            break;
                        }
                    }
                } else found = def->lookup(key);
                ///
                if (found) {
                    str     aval = str(argv[ai + 1]);
                    type_t  type = found->value.mem->type;
                    mx mstr = type->functions->from_string((none*)null, (cstr)aval.mem->origin);
                    iargs[key] = mstr;
                } else {
                    printf("arg not found: %s\n", key.mem->get<char>(0)); // shouldnt do this dangerous thing with strings
                    return {};
                }
            }
        }
        ///
        /// return results in order of defaults, with default value given
        map res = map();
        for(field &df:def.data->fields.elements<field>()) {
            field *ov = iargs->lookup(df.key);
            res.data->fields += field { ion::hold(df.key), ov ? ion::hold(ov->value) : ion::hold(df.value) };
        }
        return res;
    }

    ldata::literable<field> map::fields() const {
        return data->elements();
    }

    size_t map::len() { return data->fields->len(); }
    
    /// props bypass dependency with this operator returning a list of field, which is supported by mx (map would not be)
    map::operator   doubly &() { return data->fields; }
    bool       map::operator()(mx key) { return (*data)(key); }

    bool map::contains(mx key) { return (*data)(key); }
    
    map::operator bool() { return mx::mem && *data; }

    /// when a size is specified to map, it engages hash map mode
    map::map(size sz) : map() {
        data->fields = doubly();
        if (sz) data->hash_map = new hashmap(sz);
    }

    map::map(size_t sz) : map() {
        data->fields = doubly();
        if (sz) data->hash_map = new hashmap(sz);
    }

    map::map(std::initializer_list<field> args) : map(size(0)) {
        for(const field &f:args)
            data->fields += f;
    }

    map::map(const array &arr) : map(size(default_hash_size)) {
        for(field &f:arr.elements<field>()) data->fields += f;
    }

    map::map(const doubly &li) : map(size(default_hash_size)) {
        for(field &f:li.elements<field>()) data->fields += f;
    }
    
    mx &map::operator[](const mx &k) { return (*data)[k]; }
}