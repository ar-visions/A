#include <A>

bool isname(char n) {
    return (n >= 'a' && n <= 'z') || (n >= 'A' && n <= 'Z') || (n == '_');
}

bool validate(path mfile) {
    file f = new(file, src, mfile, read, true);
    if(!cast(f, bool))
        return false;

    map   name_symbols = new(map, hsize, 64);
    array name_ordered = new(array, alloc, 32);
    map   type_symbols = new(map, hsize, 64);
    array type_ordered = new(array, alloc, 32);
    u16*  n_names      = read(f, typeid(u16));

    for (u32 i = 0; i < *n_names; i++) {
        string name = read(f, typeid(string));
        printf("first string: %s\n", name->chars);
        verify(len(name) && isname(name->chars[0]), "invalid identifier-name");
        set (name_symbols, name, A_u16(len(name_symbols)));
        push(name_ordered, name);
    }

    u16* n_types = read(f, typeid(u16));
    u32 types_len = *n_types;
    for (u32 i = 0; i < types_len; i++) {
        string type = read(f, typeid(string));
        verify(len(type) && isname(type->chars[0]), "invalid type-name");
        set (type_symbols, type, A_u16(len(type_symbols)));
        push(type_ordered, type);
    }

    // output info on types, their type id, name
    for (u32 i = 0; i < types_len; i++) {
        u16 *type_ident = read(f, typeid(u16));
        u32 *traits     = read(f, typeid(u32));
        u32 *size       = read(f, typeid(u32));
        u16 *meta_count = read(f, typeid(u16));

        printf("type_ident: %i (%s)\n", (int)*type_ident, 
            ((string)type_ordered->elements[(int)*type_ident])->chars);
        printf("traits:     %i\n", (int)*traits);
        printf("size:       %i\n", (int)*size);
        printf("meta_count: %i\n", (int)*meta_count);

        for (u32 m = 0; m < *meta_count; m++) {
            u16 *meta_ident = read(f, typeid(u16));
            verify(*meta_ident < len(type_ordered), "invalid type ident");
            string  meta_ident_symbol = type_ordered->elements[*meta_ident];
            verify(contains(type_symbols, meta_ident_symbol), "invalid data");
        }

        // write member count and member info
        u16 *mem_count  = read(f, typeid(u16));
        for (u32 m = 0; m < *mem_count; m++) {
            u32 *member_type = read(f, typeid(u32));
            verify (*member_type < 2048, "invalid member_type");

            u16 *name_ident  = read(f, typeid(u16));
            verify(*name_ident < len(name_ordered), "invalid type ident");
            string  name_ident_symbol = name_ordered->elements[*name_ident];
            verify(contains(name_symbols, name_ident_symbol), "invalid data");

            u16 *type_ident  = read(f, typeid(u16));
            verify(*type_ident < len(type_ordered), "invalid type ident");
            string  type_ident_symbol = type_ordered->elements[*type_ident];
            verify(contains(type_symbols, type_ident_symbol), "invalid data");

            if (*member_type & A_TYPE_PROP) {
                // write member offset
                u16 *offset  = read(f, typeid(u16));
                verify(*offset < *size, "invalid offset data");
           } else if (*member_type & (A_TYPE_IMETHOD | A_TYPE_SMETHOD)) {
                // write arg count & arg idents
                u16 *arg_count  = read(f, typeid(u16));
                for (int a = 0; a < *arg_count; a++) {
                    u16 *arg_type = read(f, typeid(u16));
                    verify(*arg_type < len(type_ordered), "invalid type ident");
                    string arg_type_symbol = type_ordered->elements[*arg_type];
                    verify(contains(type_symbols, arg_type_symbol), "invalid data");
                }
            }
        }
    }
    close(f);
    return true;
}

bool reflect(string module, path loc) {
    file f = new(file, src, loc, write, true);
    if(!cast(f, bool))
        return false;
    
    map name_symbols = new(map, hsize, 64);
    map type_symbols = new(map, hsize, 64);
    num    types_len = 0;
    AType* types     = A_types(&types_len);
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
    int ii = 0;
    for (int i = 0; i < types_len; i++) {
        AType type = types[i];
        if (strcmp(type->module, module->chars) != 0)
            continue;

        printf("type iteration: %i (%s)\n", ii++, type->name);
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
            
            // write member type
            write(f, A_u32(mem->member_type));

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
    if (argc != 1)
        fault("usage: %s module-name", argv[1]);
    
    string  module = str(MODULE);
    path    m_file = form(path, "lib%o.m", module);
    bool    res    = reflect(module, m_file);

    //verify(validate(m_file), "validation failure");
    return 0;
}