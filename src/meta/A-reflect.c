#include <A>
#include <dlfcn.h>

typedef AType*(*A_types_fn)  (num*);
typedef void  (*A_start_fn)  ();

static A_start_fn   _A_start;
static A_types_fn   _A_types;

bool reflect(symbol module, symbol loc) {
    path m_location = new(path, chars, (cstr)loc);
    file f = new(file, src, m_location, write, true);
    if(!f->f)
        return false;

    map name_symbols = new(map, hsize, 64);
    map type_symbols = new(map, hsize, 64);
    num    types_len = 0;
    AType* types     = _A_types(&types_len);
    for (int i = 0; i < types_len; i++) {
        AType type = types[i];
        if (strcmp(type->module, module) != 0)
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
    A_start();
    
    if (argc != 2)
        fault("usage: %s libA.so", argv[0]);

    path   so_file = new(path, chars, argv[1]);
    string      so = stem(so_file);
    string  module = copy(so);

    // load library with dlopen
    //void* handle = dlopen(so_file, RTLD_LAZY);
    //_A_start     = dlsym (handle, "A_start");
    //_A_types     = dlsym (handle, "A_types");
    //_A_start();

    
    // call our reflect function to iterate through the types
    // notice we are using a separate from the one we load
    //bool     res = reflect(module, m_file);
    return 0;//  res ? 0 : 1;
}