#!/bin/bash
# Script to generate wrapper macros for classes found in project header files

# Check for required arguments
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <init_header> <project_header> <project_name>"
    exit 1
fi

INIT_HEADER=$1
PROJECT_HEADER=$2
PROJECT=$3
UPROJECT=$4
SED=${SED:-sed}

# Only regenerate if necessary
if [ ! -f "$INIT_HEADER" ] || [ "$PROJECT_HEADER" -nt "$INIT_HEADER" ]; then
    # Start the file with header comments and include guards
    rm -f "$INIT_HEADER"
    echo "/* generated methods interface */" > "$INIT_HEADER"
    echo "#ifndef _${UPROJECT}_INIT_H_" >> "$INIT_HEADER"
    echo "#define _${UPROJECT}_INIT_H_" >> "$INIT_HEADER"
    echo >> "$INIT_HEADER"
    
    # Process class/mod/meta/vector declarations
    grep -o 'declare_\(class\|mod\|meta\|vector\)[[:space:]]*([[:space:]]*[^,)]*' "$PROJECT_HEADER" | \
    $SED -E 's/declare_(class|mod|meta|vector)[[:space:]]*\([[:space:]]*([^,)]*)[[:space:]]*(,[[:space:]]*([^,)[:space:]]*))?/\1 \2 \4/' | \
    while read type class_name arg; do
        if [ -z "${class_name}" ]; then
            continue
        fi
        
        # Debug definitions
        echo "#ifndef NDEBUG" >> "$INIT_HEADER"
        echo "    //#define TC_${class_name}(MEMBER, VALUE) A_validate_type(VALUE, A_member(isa(instance), A_TYPE_PROP|A_TYPE_INTERN|A_TYPE_PRIV, #MEMBER)->type)" >> "$INIT_HEADER"
        echo "    #define TC_${class_name}(MEMBER, VALUE) VALUE" >> "$INIT_HEADER"
        echo "#else" >> "$INIT_HEADER"
        echo "    #define TC_${class_name}(MEMBER, VALUE) VALUE" >> "$INIT_HEADER"
        echo "#endif" >> "$INIT_HEADER"
        
        # Variadic argument counting macros
        echo "#define _ARG_COUNT_IMPL_${class_name}(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, N, ...) N" >> "$INIT_HEADER"
        echo "#define _ARG_COUNT_I_${class_name}(...) _ARG_COUNT_IMPL_${class_name}(__VA_ARGS__, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)" >> "$INIT_HEADER"
        echo "#define _ARG_COUNT_${class_name}(...)   _ARG_COUNT_I_${class_name}(\"A-type\", ## __VA_ARGS__)" >> "$INIT_HEADER"
        
        # Combination macros
        echo "#define _COMBINE_${class_name}_(A, B)   A##B" >> "$INIT_HEADER"
        echo "#define _COMBINE_${class_name}(A, B)    _COMBINE_${class_name}_(A, B)" >> "$INIT_HEADER"
        
        # Argument handling macros
        echo "#define _N_ARGS_${class_name}_0( TYPE)" >> "$INIT_HEADER"
        
        if [ "$type" = "meta" ] || [ "$type" = "vector" ]; then
            echo "#define _N_ARGS_${class_name}_1( TYPE, a)" >> "$INIT_HEADER"
        else
            echo "#define _N_ARGS_${class_name}_1( TYPE, a) _Generic((a), TYPE##_schema(TYPE, GENERICS, object) const void *: (void)0)(instance, a)" >> "$INIT_HEADER"
        fi
        
        # Property assignment macros for various argument counts
        echo "#define _N_ARGS_${class_name}_2( TYPE, a,b) instance->a = TC_${class_name}(a,b);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_4( TYPE, a,b, c,d) _N_ARGS_${class_name}_2(TYPE, a,b) instance->c = TC_${class_name}(c,d);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_6( TYPE, a,b, c,d, e,f) _N_ARGS_${class_name}_4(TYPE, a,b, c,d) instance->e = TC_${class_name}(e,f);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_8( TYPE, a,b, c,d, e,f, g,h) _N_ARGS_${class_name}_6(TYPE, a,b, c,d, e,f) instance->g = TC_${class_name}(g,h);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_10(TYPE, a,b, c,d, e,f, g,h, i,j) _N_ARGS_${class_name}_8(TYPE, a,b, c,d, e,f, g,h) instance->i = TC_${class_name}(i,j);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_12(TYPE, a,b, c,d, e,f, g,h, i,j, l,m) _N_ARGS_${class_name}_10(TYPE, a,b, c,d, e,f, g,h, i,j) instance->l = TC_${class_name}(l,m);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_14(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o) _N_ARGS_${class_name}_12(TYPE, a,b, c,d, e,f, g,h, i,j, l,m) instance->n = TC_${class_name}(n,o);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_16(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q) _N_ARGS_${class_name}_14(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o) instance->p = TC_${class_name}(p,q);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_18(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s) _N_ARGS_${class_name}_16(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q) instance->r = TC_${class_name}(r,s);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_20(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s, t,u) _N_ARGS_${class_name}_18(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s) instance->t = TC_${class_name}(t,u);" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}_22(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s, t,u, v,w) _N_ARGS_${class_name}_20(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s, t,u) instance->v = TC_${class_name}(v,w);" >> "$INIT_HEADER"
        
        # Helper macros
        echo "#define _N_ARGS_HELPER2_${class_name}(TYPE, N, ...)  _COMBINE_${class_name}(_N_ARGS_${class_name}_, N)(TYPE, ## __VA_ARGS__)" >> "$INIT_HEADER"
        echo "#define _N_ARGS_${class_name}(TYPE,...)    _N_ARGS_HELPER2_${class_name}(TYPE, _ARG_COUNT_${class_name}(__VA_ARGS__), ## __VA_ARGS__)" >> "$INIT_HEADER"
        
        # Main constructor macro
        echo "#define ${class_name}(...) ({ \\" >> "$INIT_HEADER"
        echo "    ${class_name} instance = (${class_name})A_alloc(typeid(${class_name}), 1, true); \\" >> "$INIT_HEADER"
        echo "    _N_ARGS_${class_name}(${class_name}, ## __VA_ARGS__); \\" >> "$INIT_HEADER"
        echo "    A_initialize((object)instance); \\" >> "$INIT_HEADER"
        echo "    instance; \\" >> "$INIT_HEADER"
        echo "})" >> "$INIT_HEADER"
    done
    
    # Process struct declarations
    grep -o 'declare_struct[[:space:]]*([[:space:]]*[^,)]*' "$PROJECT_HEADER" | \
    $SED -E 's/declare_struct[[:space:]]*\([[:space:]]*([^,)]*)[[:space:]]*(,[[:space:]]*([^,)[:space:]]*))?/\1 \3/' | \
    while read struct_name arg; do
        if [ -z "${struct_name}" ]; then
            continue
        fi
        
        echo "#define ${struct_name}(...) structure(${struct_name} __VA_OPT__(,) __VA_ARGS__) _N_STRUCT_ARGS(${struct_name}, __VA_ARGS__);" >> "$INIT_HEADER"
    done
    
    # Close the include guard
    echo >> "$INIT_HEADER"
    echo "#endif /* _${UPROJECT}_INIT_H_ */" >> "$INIT_HEADER"
    
    echo "Successfully generated $INIT_HEADER"
fi