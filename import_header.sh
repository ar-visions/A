#!/bin/bash

# usage: ./generate_import_header.sh <output_file> <uproject> <project> <lib_modules> <import1> [import2 ...]

OUTPUT="$1"
PROJECT_HEADER="$2"
PROJECT="$3"
UPROJECT="$4"

#LIB_MODULES_STR="$4"
#IFS=' ' read -r -a LIB_MODULES <<< "$LIB_MODULES_STR"

#echo "test2: str: $LIB_MODULES_STR"

shift 4
IMPORTS=("$@")

IMPORT_DIR="${IMPORT:-import}"
SRC_ROOT="${SRC_ROOT:-.}"

# regen check
if [ ! -f "$OUTPUT" ] || [ "$SRC_ROOT/import" -nt "$OUTPUT" ]; then
    rm -f "$OUTPUT"

    echo "/* generated import interface */" >> "$OUTPUT"
    echo "#ifndef _${UPROJECT}_IMPORT_${PROJECT}_" >> "$OUTPUT"
    echo "#define _${UPROJECT}_IMPORT_${PROJECT}_" >> "$OUTPUT"
    echo "" >> "$OUTPUT"

    echo "/* imports: ${IMPORTS[*]} */" >> "$OUTPUT"

    for import in "${IMPORTS[@]}"; do
        if [ "$import" != "$PROJECT" ]; then
            if [ -f "$IMPORT_DIR/include/${import}-methods" ]; then
                echo "#include <${import}-public> // this was from {import}" >> "$OUTPUT"
            else
                echo "// #include <${import}-public> // has no $IMPORT_DIR/include/${import}-methods" >> "$OUTPUT"
            fi
        fi
    done

    echo "#include <${PROJECT}-intern> // line 52 uses {PROJECT}" >> "$OUTPUT"
    echo "#include <${PROJECT}>" >> "$OUTPUT"

    #for name in $LIB_MODULES; do
    #    base=$(basename "$name")
    #    echo "#include <${base%.*}>" >> "$OUTPUT"
    #done

    echo "#include <${PROJECT}-methods>" >> "$OUTPUT"
    echo "#include <A-reserve>" >> "$OUTPUT"

    # must have PROJECT-init's last! ... these define macros that have the same name as the type, so we cannot process our types through macro expansion with these in place
    # it also makes it not possible to define macros for types within our modules without manual header sequencing between them
    for import in "${IMPORTS[@]}"; do
        if [ "$import" != "$PROJECT" ]; then
            if [ -f "$IMPORT_DIR/include/${import}-methods" ]; then
                echo "#include <${import}-init>" >> "$OUTPUT"
            fi
        fi
    done

    echo "#include <${PROJECT}-init>" >> "$OUTPUT"
    echo "" >> "$OUTPUT"
    echo "#endif" >> "$OUTPUT"
fi
