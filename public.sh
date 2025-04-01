#!/bin/bash
# Script to generate public header from project header files

# Check for required arguments
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <methods_header> <project_header> <project_name>"
    exit 1
fi

PUBLIC_HEADER=$1
PROJECT_HEADER=$2
PROJECT=$3
UPROJECT=$4

SED=${SED:-sed}

# Only regenerate if necessary
if [ ! -f "$PUBLIC_HEADER" ] || [ "$PROJECT_HEADER" -nt "$PUBLIC_HEADER" ]; then

    # Start the file with header comments and include guards
    rm -f "$PUBLIC_HEADER"
    echo "/* generated methods interface */" > "$PUBLIC_HEADER"
    echo "#ifndef _${UPROJECT}_PUBLIC_"         >> "$PUBLIC_HEADER"
    echo "#define _${UPROJECT}_PUBLIC_"         >> "$PUBLIC_HEADER"
    echo >> "$PUBLIC_HEADER"


    # Iterate through all three types using a single loop
    for type in "class" "mod" "meta" "struct"; do
        grep -o "declare_${type}[[:space:]]*([[:space:]]*[^,)]*" "$PROJECT_HEADER" | \
        $SED "s/declare_${type}[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/" | \
        while read class_name; do
            if [ -n "$class_name" ]; then
                echo "#ifndef ${class_name}_intern" >> "$PUBLIC_HEADER"
                echo "#define ${class_name}_intern(AA,YY,...) AA##_schema(AA,YY##_EXTERN, __VA_ARGS__)" >> "$PUBLIC_HEADER"
                echo "#endif" >> "$PUBLIC_HEADER"
            fi
        done
        echo >> "$PUBLIC_HEADER"
    done

    echo "#include <${PROJECT}>"                >> "$PUBLIC_HEADER" ; \
    #echo "#include <${PROJECT}-init>"           >> "$PUBLIC_HEADER" ; \
    echo "#include <${PROJECT}-methods>"        >> "$PUBLIC_HEADER" ; \
    echo "#endif /* _${UPROJECT}_PUBLIC_ */"    >> "$PUBLIC_HEADER"

    echo "Successfully generated $PUBLIC_HEADER"
fi
