#!/bin/bash
# Script to generate intern header from project header files

# Check for required arguments
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <methods_header> <project_header> <project_name>"
    exit 1
fi

INTERN_HEADER=$1
PROJECT_HEADER=$2
PROJECT=$3
UPROJECT=$4

SED=${SED:-sed}

# Only regenerate if necessary
if [ ! -f "$INTERN_HEADER" ] || [ "$PROJECT_HEADER" -nt "$INTERN_HEADER" ]; then

    # Start the file with header comments and include guards
    rm -f "$INTERN_HEADER"
    echo "/* generated methods interface */" > "$INTERN_HEADER"
    echo "#ifndef _${UPROJECT}_INTERN_H_" >> "$INTERN_HEADER"
    echo "#define _${UPROJECT}_INTERN_H_" >> "$INTERN_HEADER"
    echo >> "$INTERN_HEADER"
    
    # Iterate through all three types using a single loop
    for type in "class" "mod" "meta"; do
        echo "# Processing $type declarations..."
        
        grep -o "declare_${type}[[:space:]]*([[:space:]]*[^,)]*" "$PROJECT_HEADER" | \
        $SED "s/declare_${type}[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/" | \
        while read class_name; do
            if [ -n "$class_name" ]; then
                echo "#undef ${class_name}_intern" >> "$INTERN_HEADER"
                echo "#define ${class_name}_intern(AA,YY,...) AA##_schema(AA,YY, __VA_ARGS__)" >> "$INTERN_HEADER"
            fi
        done
        
        # Add a newline after each type section
        echo >> "$INTERN_HEADER"
    done
    
    echo "#endif /* _${UPROJECT}_INTERN_H_ */" >> "$INTERN_HEADER"
    
    echo "Successfully generated $INTERN_HEADER"
fi