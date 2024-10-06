#!/bin/bash
(
    # parse args
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --src)
                SRC_DIR="$2"
                shift 2
                ;;
            --deps)
                DEPS="$2"
                shift 2
                ;;
            --debug)
                DEBUG_FILE="$2"
                shift 2
                ;;
            *)
                BUILD_ROOT="$1"
                echo "got build root set to $1"
                shift
                ;;
        esac
    done

    if [ -z "$DEPS" ]; then
        echo '--deps required argument'
        exit
    else
        mapfile -t projects < "$DEPS"
    fi

    SRC_DIR="${SRC_DIR:-$SRC}"
    BUILD_ROOT="${BUILD_ROOT:-silver-import}"

    echo "BUILD_ROOT = $BUILD_ROOT"

    #projects=(
    #    "A https://github.com/ar-visions/A.git main"
    #    "llvm https://github.com/llvm/llvm-project 3b5b5c1 -S ../llvm -G Ninja -DLLVM_ENABLE_ASSERTIONS=ON   -DLLVM_ENABLE_PROJECTS='clang;lld' -DLLVM_ENABLE_FFI=ON -DLLVM_ENABLE_RTTI=OFF -DLLVM_USE_LINKER=gold -DLLVM_BINUTILS_INCDIR=/usr/include -DCLANG_DEFAULT_CXX_STDLIB=libstdc++ -DCLANG_DEFAULT_PIE_ON_LINUX=ON -DCLANG_CONFIG_FILE_SYSTEM_DIR=/etc/clang -DLLVM_ENABLE_LIBCXX=OFF -DBUILD_SHARED_LIBS=ON -DLLVM_BUILD_LLVM_DYLIB=OFF -DLLVM_LINK_LLVM_DYLIB=OFF -DLLVM_TARGETS_TO_BUILD='host;X86'"
    #)

    # debugging switch: --debug file.txt     [contents: project1,another]  would debug those two.
    # the idea here is DEBUG_PROJECTS will only be set when it hasnt been set yet.
    # so we set configuration at top level

    # other projects may want their own debug info set, 
    # which may need to append into this one, but thats 
    # only when there is a conflict between two where 
    # top wants release and middle wants debug

    if [ -z "$DEBUG_PROJECTS" ]; then
        if [ -f "$DEBUG_FILE" ]; then
            export DEBUG_PROJECTS=$(cat $DEBUG_FILE)
        else
            export DEBUG_PROJECTS=","
        fi
    fi

    SCRIPT_DIR=$(dirname "$(realpath "$0")")
    cd      $SCRIPT_DIR || exit 1
    mkdir -p "$BUILD_ROOT" || exit 1
    cd       "$BUILD_ROOT"
    mkdir -p checkout
    cd       checkout

    # iterate through projects, cloning and building
    for project in "${projects[@]}"; do
        PROJECT_NAME=$(echo "$project" | cut -d ' ' -f 1)
        REPO_URL=$(    echo "$project" | cut -d ' ' -f 2)
        COMMIT=$(      echo "$project" | cut -d ' ' -f 3)
        TARGET_DIR="${PROJECT_NAME}"

        # set build folder based on release/debug
        if [[ ",$DEBUG_PROJECTS," == *",$PROJECT_NAME,"* ]]; then
            CMAKE_FOLDER="silver-debug"
        else
            CMAKE_FOLDER="silver-build"
        fi

        # while 4 and on is not blank, append -f X + whitespace
        BUILD_CONFIG=""
        # loop through remaining fields and append to BUILD_CONFIG
        for i in $(seq 4 $(echo "$project" | wc -w)); do
            field=$(echo "$project" | cut -d ' ' -f $i)
            [ -n "$field" ] && BUILD_CONFIG+="$field "
        done

        # Check if --src directory and project exist, then symlink
        if [ -n "$SRC_DIR" ] && [ -d "$SRC_DIR/$TARGET_DIR" ]; then
            rm -rf "$TARGET_DIR"
            echo "symlinking $TARGET_DIR to $SRC_DIR/$TARGET_DIR..."
            ln -s "$SRC_DIR/$TARGET_DIR" "$TARGET_DIR" || exit 1
            cd "$TARGET_DIR"
        else
            if [ -d "$TARGET_DIR" ]; then
                cd "$TARGET_DIR" || exit 1
                if [ "$PULL" == "1" ]; then
                    echo "pulling latest changes for $TARGET_DIR..."
                    PULL_HASH_0=$(git rev-parse HEAD)
                    git pull || { echo "git pull failed, exiting." >&2; exit 1; }
                    PULL_HASH_1=$(git rev-parse HEAD)
                    if [ "$PULL_HASH_0" != "$PULL_HASH_1" ]; then
                        rm -f "$CMAKE_FOLDER/silver-token" || { echo "silver-token failed to rm" >&2; exit 1; }
                    fi
                fi
            else
                echo "cloning repository $REPO_URL into $TARGET_DIR..."
                git clone "$REPO_URL" "$TARGET_DIR"
                if [ $? -ne 0 ]; then
                    echo "clone failed for $TARGET_DIR"
                    exit 1
                fi
                if [ $? -ne 0 ]; then
                    echo "fetch failed for $TARGET_DIR"
                    exit 1
                fi
                cd "$TARGET_DIR"
            fi
            # check out the specific commit, branch, or tag if provided
            if [ -n "$COMMIT" ]; then
                echo "checking out $COMMIT for $TARGET_DIR"
                git checkout "$COMMIT"
                if [ $? -ne 0 ]; then
                    echo "checkout failed for $TARGET_DIR at $COMMIT"
                    exit 1
                fi
            fi
        fi

        # check if this is a cmake project, otherwise use autotools
        # or if there is "-S " in BUILD_CONFIG
        if [ -f "CMakeLists.txt" ] || [[ "$BUILD_CONFIG" == *-S* ]]; then
            cmake="1"
            if [[ ",$DEBUG_PROJECTS," == *",$PROJECT_NAME,"* ]]; then
                BUILD_TYPE="-DCMAKE_BUILD_TYPE=Debug"
            else
                BUILD_TYPE="-DCMAKE_BUILD_TYPE=Release"
            fi
        else
            cmake=""
            if [[ ",$DEBUG_PROJECTS," == *",$PROJECT_NAME,"* ]]; then
                BUILD_TYPE="--enable-debug"
            else
                BUILD_TYPE=""
            fi
        fi

        mkdir -p $CMAKE_FOLDER
        cd $CMAKE_FOLDER

        if [ "$REBUILD" == "all" ] || [ "$REBUILD" == "$PROJECT_NAME" ]; then
            echo "rebuilding ${TARGET_DIR}"
            if [ -n "$cmake" ]; then
                cmake --build . --target clean
            else
                make clean
            fi
            rm -rf silver-token
        fi

        # proceed if there is a newer file than silver-token, or no silver-token
        if [ ! -f "silver-token" ] || find .. -type f -newer "silver-token" | grep -q . ; then
            #echo cmake -B . $BUILD_TYPE $BUILD_CONFIG -DCMAKE_INSTALL_PREFIX="$BUILD_ROOT" 

            if [ -n "$cmake" ]; then
                BUILD_CONFIG="${BUILD_CONFIG% }"
                if [ -z "$BUILD_CONFIG" ]; then
                    BUILD_CONFIG="-S .."
                fi
                cmake -B . $BUILD_TYPE $BUILD_CONFIG -DCMAKE_INSTALL_PREFIX="$BUILD_ROOT" 
            else
                if [ ! -f "../configure" ]; then
                    echo "running autoreconf -i in $PROJECT_NAME ($cmake)"
                    autoupdate ..
                    autoreconf -i ..
                fi
                if [ ! -f "../configure" ]; then
                    echo "configure script not available after running autoreconf -i"
                    exit 1
                fi
                ../configure $BUILD_TYPE --prefix=$BUILD_ROOT $BUILD_CONFIG
            fi

            # works for both cmake and configure
            if [ $? -ne 0 ]; then
                echo "generate failed for $TARGET_DIR"
                exit 1
            fi

            # choose j size
            repo_size=$(du -sm .. | cut -f1)
            if [ "$repo_size" -gt 500 ]; then
                j=8 # llvm takes up a lot of memory/swap when linking (70GB with debug on), as does many large projects
            else
                j=$(nproc)
            fi

            # build
            echo "building $TARGET_DIR with -j$j in $CMAKE_FOLDER"
            if [ -n "$cmake" ]; then
                cmake --build . -- -j$j
            else
                make
            fi
            if [ $? -ne 0 ]; then
                echo "build failure for $TARGET_DIR"
                exit 1
            fi

            # install to silver-import
            echo "installing ${TARGET_DIR}"
            if [ -n "$cmake" ]; then
                cmake --install .
            else
                make install
            fi
            if [ $? -ne 0 ]; then
                echo "install failure for $TARGET_DIR"
                exit 1
            fi

            echo "installed ${TARGET_DIR}"
            echo "im a token" >> silver-token  # only create this if all steps succeed
        fi
        cd ../../
    done
)
