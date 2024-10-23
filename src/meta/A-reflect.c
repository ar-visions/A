#include <A-type>
// we'll use manual declarations to prototypes in a flat API design here, 
// just so we arent bootstrapping our own A-instance and disrupting the type process
#include <dlfcn.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef long long       i64, num;
typedef short           i16;
typedef unsigned short  u16;
typedef int             i32;
typedef unsigned int    u32;
typedef char*           cstr;
typedef const char*     symbol;
//typedef struct AType*   AType;
typedef void*           handle;
typedef void*           path;
typedef void*           file;
typedef void*           string;
typedef void*           object;

typedef struct item {
    struct item* next;
    struct item* prev;
    object value;
    object key;
} *item;

typedef struct map {
    item first, last;
    num  count;
} *map;

#define null 0

static AType*(*A_types)      (num*);
static void  (*A_start)      ();
static object(*A_initialize) (object);
static object(*A_formatter)  (AType, object, bool, cstr, ...);
static object(*A_alloc)      (AType, num, bool);
static u16*  (*A_u16)        (u16);
static u32*  (*A_u32)        (u32);
static i32*  (*A_i32)        (i32);
static AType (*A_find_type)  (cstr);

static void  (*string_append)(string, char*);
static bool  (*string_eq)    (string, char*);
static num   (*string_cmp)   (string, char*);
static string(*string_mid)   (string, num, num);
static num   (*string_len)   (string);

static bool  (*map_contains) (map,    object);
static object(*map_get)      (map,    object);
static void  (*map_set)      (map,    object, object);
static num   (*map_len)      (map);

static path  (*path_with_string) (path, string);
static string(*path_stem)        (path);
static path  (*path_parent)      (path);
static int   (*path_exists)      (path);

static bool  (*file_write)       (file, object);
static void  (*file_close)       (file);

#define cmp(str, cstr)  string_cmp(str, cstr)
#define contains(m, k)  map_contains(m, k)
#define get(m, k)       map_get(m, k)
#define set(m, k, v)    map_set(m, k, v)
#define append(s, ch)   string_append(s, ch)
#define len(m)          map_len(m)
#define stem(m)         path_stem(m)
#define mid(m,s,c)      string_mid(m,s,c)
#define parent(m)       path_parent(m)
#define exists(m)       path_exists(m)
#define form(T, t, ...) (T)A_formatter(T##_type, null, false, t, ## __VA_ARGS__)
#define print(t, ...)      A_formatter(string_type, stdout, true, t, ## __VA_ARGS__)
#define write(f,data)   file_write(f,data)
#define close(f)        file_close(f)

#define verify(a, t, ...) \
    do { \
        if (!(a)) { \
            A_formatter(null, stderr, true,  t, ## __VA_ARGS__); \
            raise(SIGSTOP); \
        } \
    } while(0)

static AType string_type;
static AType map_type;
static AType path_type;
static AType file_type;

string str(cstr chars) {
    string n = A_alloc(string_type, 1, true);
    append(n, chars);
    A_initialize(n);
    return n;
}

string map_of(num sz) {
    map n = A_alloc(map_type, 1, true);
    A_initialize(n);
    return n;
}

string file_of(cstr loc, bool writing) {
    file n = A_alloc(file_type, 1, true);
    A_initialize(n);
    return n;
}

static path path_of(cstr chars) {
    path pa = A_alloc(path_type, 1, true);
    path_with_string(pa, str(chars));
    return pa;
}

int main(int argc, char* argv[]) {
    if (argc != 2)
        fprintf(stderr, "usage: %s libA.so\n", argv[0]);

    handle f = dlopen(argv[1], RTLD_NOW); // so we'll simply use A-type in this dlopen-way
    if   (!f) return 0; // tell cmake to keep the library it has made; without a .m the install fails
    
    string_append = dlsym (f, "string_append");
    string_eq     = dlsym (f, "string_eq");
    string_cmp    = dlsym (f, "string_cmp");
    string_mid    = dlsym (f, "string_mid");
    string_len    = dlsym (f, "string_len");

    file_write    = dlsym (f, "file_write");
    file_close    = dlsym (f, "file_close");

    map_contains  = dlsym (f, "map_contains");
    map_set       = dlsym (f, "map_set");
    map_get       = dlsym (f, "map_get");
    map_len       = dlsym (f, "map_len");

    path_with_string = dlsym (f, "path_with_string");
    path_stem        = dlsym (f, "path_stem");
    path_parent      = dlsym (f, "path_parent");
    path_exists      = dlsym (f, "path_exists");

    A_alloc       = dlsym (f, "A_alloc");
    A_u32         = dlsym (f, "A_u32");
    A_i32         = dlsym (f, "A_i32");
    A_u16         = dlsym (f, "A_u16");
    A_start       = dlsym (f, "A_start");
    A_initialize  = dlsym (f, "A_initialize");
    A_types       = dlsym (f, "A_types");
    A_find_type   = dlsym (f, "A_find_type");
    A_formatter   = dlsym (f, "A_formatter");

    A_start();

    string_type = A_find_type("string");
    path_type   = A_find_type("path");
    map_type    = A_find_type("map");
    file_type   = A_find_type("file");

    path   so_file = path_of(argv[1]);
    path       dir = parent(so_file);
    string      so =   stem(so_file);
    string  module =    mid(so, 3, string_len(so) - 3);
    print("module = %o", module);

    path    m_file =   form(path, "%o/lib%o.m", dir, module);
    print("m_file = %o", m_file);

    verify(exists(so_file), "shared-module not found: %o", so_file);

    num types_len = 0;
    AType*  types = A_types(&types_len);

    map name_symbols = map_of(64);
    map type_symbols = map_of(64);

    for (int i = 0; i < types_len; i++) {
        ftable_t* type = types[i];
        if (cmp(module, type->module) != 0)
            continue;
        /// process type name
        string type_symbol = str(type->name);
        if (!contains(type_symbols, type_symbol))
            set(type_symbols, type_symbol, A_u16(len(type_symbols)));
        for (int m = 0; m < type->member_count; m++) {
            type_member_t* mem = &type->members[m];
            string name_symbol = str(mem->name);
            if (!contains(name_symbols, name_symbol))
                 set     (name_symbols, name_symbol, A_u16(len(name_symbols)));
            if (!contains(type_symbols, type_symbol))
                 set     (type_symbols, type_symbol, A_u16(len(type_symbols)));
        }
    }

    // emit symbol table for member name
    i64 n_name = len(name_symbols);
    file output = file_of(loc, true);
    verify(n_name <= 65535, "name count overflow");
    write(output, A_u16(n_name));
    for (item i = name_symbols->first; i; i = i->next)
        write(output, i->key);
    
    // emit symbol table for type name
    i64 n_type = len(type_symbols);
    verify(n_type <= 65535, "type count overflow");
    write(output, A_u16(n_type));
    for (item i = type_symbols->first; i; i = i->next)
        write(output, i->key);

    // output info on types, their type id, name
    for (int i = 0; i < types_len; i++) {
        ftable_t* type = types[i];
        if (strcmp(type->module, module) != 0)
            continue;
        // write type ident (this is implicit but useful for verification)
        u16 *type_ident = get(type_symbols, str(type->name));
        write(output, type_ident);

        // write traits (u32)
        u32 *traits = A_u32(type->traits);
        write(output, traits);

        // write size (u32)
        u32 *size = A_u32(type->size);
        write(output, size);

        // write meta count and idents
        u16 *meta_count = A_u16(type->meta.count);
        write(output, meta_count);
        for (int m = 0; m < type->meta.count; m++) {
            AType  mtype = ((AType*)&type->meta.meta_0)[m];
            u16 *m_ident = get(type_symbols, str(((ftable_t*)mtype)->name));
            write(output, m_ident);
        }

        // write member count and member info
        u16 *mem_count  = A_u16(type->member_count);
        write(output, mem_count);
        for (int m = 0; m < type->member_count; m++) {
            type_member_t* mem = &type->members[m];
            //if (!mem->name || !mem->type)
            //    continue;
            
            // write member type
            write(output, A_u16(mem->member_type));

            // write member name ident
            u16 *name_ident = get(name_symbols, str(mem->name));
            write(output, name_ident);

            // write member type ident
            u16 *type_ident = get(type_symbols, str(((ftable_t*)mem->type)->name));
            write(output, type_ident);
            
            if (mem->member_type & A_TYPE_PROP) {
                // write member offset
                u16 *offset = A_u16(mem->offset);
                write(output, offset);
           } else if (mem->member_type & (A_TYPE_IMETHOD | A_TYPE_SMETHOD)) {
                // write arg count & arg idents
                write(output, A_u16(mem->args.count));
                for (int a = 0; a < mem->args.count; a++) {
                    AType  atype = ((AType*)&mem->args.meta_0)[a];
                    u16 *atype_ident = get(type_symbols, str(((ftable_t*)atype)->name));
                    write(output, atype_ident);
                }
            }
        }
    }
    close(output);
    return 0;
}

/*
static A_start_fn             _A_start;
static A_types_fn             _A_types;
static A_total_types_len_fn   _A_total_types_len;

bool reflect(string module, path loc) {
    file f = new(file, src, loc, write, true);
    if(!f->f)
        return false;

    map name_symbols = new(map, hsize, 64);
    map type_symbols = new(map, hsize, 64);
    num    types_len = 0;
    AType* types     = _A_types(&types_len);
    for (int i = 0; i < types_len; i++) {
        AType type = types[i];
        if (cmp(module, type->module) != 0)
            continue;
        /// process type name
        string type_symbol = str(type->name);
        if (!contains(type_symbols, type_symbol)) {
            set(type_symbols, type_symbol, A_u16(len(type_symbols)));
        }
        for (int m = 0; m < type->member_count; m++) {
            type_member_t* mem = &type->members[m];
            string name_symbol = str(mem->name);
            if (!contains(name_symbols, name_symbol))
                 set     (name_symbols, name_symbol, A_u16(len(name_symbols)));
            if (!contains(type_symbols, type_symbol))
                 set     (type_symbols, type_symbol, A_u16(len(type_symbols)));
        }
    }

    // emit symbol table for member name
    i64 n_name = len(name_symbols);
    verify(n_name <= 65535, "name count overflow");
    write(f, A_u16(n_name));
    for (item i = name_symbols->first; i; i = i->next)
        write(f, i->key);
    
    // emit symbol table for type name
    i64 n_type = len(type_symbols);
    verify(n_type <= 65535, "type count overflow");
    write(f, A_u16(n_type));
    for (item i = type_symbols->first; i; i = i->next)
        write(f, i->key);

    // output info on types, their type id, name
    for (int i = 0; i < types_len; i++) {
        AType type = types[i];
        if (strcmp(type->module, module) != 0)
            continue;
        // write type ident (this is implicit but useful for verification)
        u16 *type_ident = get(type_symbols, str(type->name));
        write(f, type_ident);

        // write traits (u32)
        u32 *traits = A_u32(type->traits);
        write(f, traits);

        // write size (u32)
        u32 *size = A_u32(type->size);
        write(f, size);

        // write meta count and idents
        u16 *meta_count = A_u16(type->meta.count);
        write(f, meta_count);
        for (int m = 0; m < type->meta.count; m++) {
            AType  mtype = ((AType*)&type->meta.meta_0)[m];
            u16 *m_ident = get(type_symbols, str(mtype->name));
            write(f, m_ident);
        }

        // write member count and member info
        u16 *mem_count  = A_u16(type->member_count);
        write(f, mem_count);
        for (int m = 0; m < type->member_count; m++) {
            type_member_t* mem = &type->members[m];
            //if (!mem->name || !mem->type)
            //    continue;
            
            // write member type
            write(f, A_u16(mem->member_type));

            // write member name ident
            u16 *name_ident = get(name_symbols, str(mem->name));
            write(f, name_ident);

            // write member type ident
            u16 *type_ident = get(type_symbols, str(mem->type->name));
            write(f, type_ident);
            
            if (mem->member_type & A_TYPE_PROP) {
                // write member offset
                u16 *offset = A_u16(mem->offset);
                write(f, offset);
           } else if (mem->member_type & (A_TYPE_IMETHOD | A_TYPE_SMETHOD)) {
                // write arg count & arg idents
                write(f, A_u16(mem->args.count));
                for (int a = 0; a < mem->args.count; a++) {
                    AType  atype = ((AType*)&mem->args.meta_0)[a];
                    u16 *atype_ident = get(type_symbols, str(atype->name));
                    write(f, atype_ident);
                }
            }
        }
    }
    close(f);
    return true;
}

int main(int argc, cstr argv[]) {
    if (argc != 2)
        fault("usage: %s libA.so", argv[0]);

    int count = A_total_types_len();
    handle f = dlopen(argv[1], RTLD_NOW); // so we'll simply use A-type in this dlopen-way
    if   (!f) return 0; // tell cmake to keep the library it has made; without a .m the install fails
    _A_start = dlsym (f, "A_start");
    _A_types = dlsym (f, "A_types");
    _A_start();

    path   so_file =    new(path, chars, argv[1]);
    path       dir = parent(so_file);
    string      so =   stem(so_file);
    string  module =    mid(so, 3, len(so) - 3);
    path    m_file =   form(path, "%o/lib%o.m", dir, module);

    verify(file_exists("%o", so_file), "shared-module not found: %o", so_file);

    // call our reflect function to iterate through the types
    // notice this A-module is separate from our own we designed with
    // filter results by type->module == module
    bool res = reflect(module, m_file);
    dlclose(f);
    return 0; /// again do not fail with this command.  its annoying when it does, egg gets its chicken erased
}
*/