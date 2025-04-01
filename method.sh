#!/bin/bash
# Script to generate methods interface from project header files

if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <methods_header> <project_header> <project_name>"
    exit 1
fi

METHODS_HEADER=$1
PROJECT_HEADER=$2
PROJECT=$3
UPROJECT=$4
SED=${SED:-sed}

# Only regenerate if necessary
if [ ! -f "$METHODS_HEADER" ] || [ "$PROJECT_HEADER" -nt "$METHODS_HEADER" ]; then
    rm -f "$METHODS_HEADER"
    echo "/* generated methods interface */" > "$METHODS_HEADER"
    echo "#ifndef _${UPROJECT}_METHODS_H_" >> "$METHODS_HEADER"
    echo "#define _${UPROJECT}_METHODS_H_" >> "$METHODS_HEADER"
    echo >> "$METHODS_HEADER"
    # Process method declarations (these apply to classes only, not structs; structs also cannot be poly)
    grep -o 'i_method[[:space:]]*([^,]*,[^,]*,[^,]*,[^,]*,[[:space:]]*[[:alnum:]_]*' "$PROJECT_HEADER" | \
    $SED 's/i_method[[:space:]]*([^,]*,[^,]*,[^,]*,[^,]*,[[:space:]]*\([[:alnum:]_]*\).*/\1/' | \
    while read method; do
        if [ -n "$method" ]; then
            echo "#undef $method" >> "$METHODS_HEADER"
            echo "#define $method(I,...) ({ __typeof__(I) _i_ = I; ftableI(_i_)->$method(_i_, ## __VA_ARGS__); })" >> "$METHODS_HEADER"
        fi
    done
    # Close the include guard
    echo >> "$METHODS_HEADER"
    echo "#endif /* _${UPROJECT}_METHODS_H_ */" >> "$METHODS_HEADER"
    
    echo "Successfully generated $METHODS_HEADER"
fi