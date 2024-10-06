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
                DEBUG_PROJECTS="$2"
                shift 2
                ;;
            *)
                BUILD_ROOT="$1"
                shift
                ;;
        esac
    done

    if [ -z "$DEPS" ]; then
        exit
    else
        mapfile -t projects < "$DEPS"
    fi

    SRC_DIR="${SRC_DIR:-$SRC}"
    BUILD_ROOT="${BUILD_ROOT:-silver-import}"

    #projects=(
    #    "A https://github.com/ar-visions/A.git main"
    #    "llvm https://github.com/llvm/llvm-project 3b5b5c1 -S ../llvm -G Ninja -DLLVM_ENABLE_ASSERTIONS=ON   -DLLVM_ENABLE_PROJECTS='clang;lld' -DLLVM_ENABLE_FFI=ON -DLLVM_ENABLE_RTTI=OFF -DLLVM_USE_LINKER=gold -DLLVM_BINUTILS_INCDIR=/usr/include -DCLANG_DEFAULT_CXX_STDLIB=libstdc++ -DCLANG_DEFAULT_PIE_ON_LINUX=ON -DCLANG_CONFIG_FILE_SYSTEM_DIR=/etc/clang -DLLVM_ENABLE_LIBCXX=OFF -DBUILD_SHARED_LIBS=ON -DLLVM_BUILD_LLVM_DYLIB=OFF -DLLVM_LINK_LLVM_DYLIB=OFF -DLLVM_TARGETS_TO_BUILD='host;X86'"
    #)

    # we can imagine multiple projects sharing the same build root
    # thats possible if its already set, it can filter
    # down from cmake's binary elder directory
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
        if [[ "$REPO_URL" == *.tar.* ]]; then
            IS_TAR=1
        else
            IS_TAR=0
        fi

        if [[ ",$DEBUG_PROJECTS," == *",$PROJECT_NAME,"* ]]; then
            CMAKE_BUILD="-DCMAKE_BUILD_TYPE=Debug"
            CMAKE_FOLDER="silver-debug"
        else
            CMAKE_BUILD="-DCMAKE_BUILD_TYPE=Release"
            CMAKE_FOLDER="silver-build"
        fi

        # while 4 and on is not blank, append -f X + whitespace
        CMAKE_CONFIG=""
        # loop through remaining fields and append to CMAKE_CONFIG
        for i in $(seq 4 $(echo "$project" | wc -w)); do
            field=$(echo "$project" | cut -d ' ' -f $i)
            [ -n "$field" ] && CMAKE_CONFIG+="$field "
        done

        # Trim trailing whitespace from CMAKE_CONFIG
        CMAKE_CONFIG="${CMAKE_CONFIG% }"

        echo "PROJECT_NAME=${PROJECT_NAME}"
        echo "REPO_URL=${REPO_URL}"
        echo "COMMIT=${COMMIT}"
        echo "CMAKE_CONFIG=${CMAKE_CONFIG}"
        if [ -z "$CMAKE_CONFIG" ]; then
            CMAKE_CONFIG="-S .."
        fi

        # Check if --src directory and project exist, then symlink
        if [ -n "$SRC_DIR" ] && [ -d "$SRC_DIR/$TARGET_DIR" ]; then
            rm -rf "$TARGET_DIR"
            echo "symlinking $TARGET_DIR to $SRC_DIR/$TARGET_DIR..."
            ln -s "$SRC_DIR/$TARGET_DIR" "$TARGET_DIR" || exit 1
            cd "$TARGET_DIR"
        else
            if [ -d "$TARGET_DIR" ]; then
                cd "$TARGET_DIR" || exit 1
                if [ "$PULL" == "1" ] && [ "$IS_TAR" -ne 1 ]; then
                    echo "pulling latest changes for $TARGET_DIR..."
                    PULL_HASH_0=$(git rev-parse HEAD)
                    git pull || { echo "git pull failed, exiting." >&2; exit 1; }
                    PULL_HASH_1=$(git rev-parse HEAD)
                    if [ "$PULL_HASH_0" != "$PULL_HASH_1" ]; then
                        rm -f "$CMAKE_FOLDER/silver-token" || { echo "silver-token failed to rm" >&2; exit 1; }
                    fi
                fi
            else
                if [ -n "$IS_TAR" ]; then
                    src_tar="${TARGET_DIR}.tar.gz"
                    echo "getting current dir..."
                    pwd
                    if [ ! -f "${src_tar}" ]; then
                        wget -O "${src_tar}" "$REPO_URL"
                        if [ $? -ne 0 ]; then
                            echo "wget failed for $TARGET_DIR"
                            exit 1
                        fi
                    fi

                    mkdir $TARGET_DIR
                    tar xf ${src_tar} -C $TARGET_DIR --strip-components=1

                    if [ $? -ne 0 ]; then
                        echo "tar failed for $TARGET_DIR"
                        exit 1
                    fi
                else
                    echo "cloning repository $REPO_URL into $TARGET_DIR..."
                    git clone --no-checkout "$REPO_URL" "$TARGET_DIR"
                    if [ $? -ne 0 ]; then
                        echo "clone failed for $TARGET_DIR"
                        exit 1
                    fi
                    git fetch origin "$COMMIT"
                    if [ $? -ne 0 ]; then
                        echo "fetch failed for $TARGET_DIR"
                        exit 1
                    fi
                fi
                
                cd "$TARGET_DIR"
            fi
            # check out the specific commit, branch, or tag if provided
            if [ -n "$COMMIT" ] && [ "$IS_TAR" -ne 1 ]; then
                echo "checking out $COMMIT for $TARGET_DIR... $IS_TAR"
                git checkout "$COMMIT"
                if [ $? -ne 0 ]; then
                    echo "checkout failed for $TARGET_DIR at $COMMIT"
                    exit 1
                fi
            fi
        fi

        mkdir -p $CMAKE_FOLDER
        cd $CMAKE_FOLDER

        if [ ! -f "silver-token" ] || find .. -type f -newer "silver-token" | grep -q . ; then
            echo cmake -B . $CMAKE_BUILD $CMAKE_CONFIG -DCMAKE_INSTALL_PREFIX="$BUILD_ROOT" 
            cmake -B . $CMAKE_BUILD $CMAKE_CONFIG -DCMAKE_INSTALL_PREFIX="$BUILD_ROOT" 
            if [ $? -ne 0 ]; then
                echo "cmake gen failed for $TARGET_DIR"
                exit 1
            fi

            cmake --build .
            if [ $? -ne 0 ]; then
                echo "build failure for $TARGET_DIR"
                exit 1
            fi

            cmake --install .
            if [ $? -ne 0 ]; then
                echo "install failure for $TARGET_DIR"
                exit 1
            fi
            
            echo "im a token" >> silver-token  # only create this if all steps succeed
        fi

        # Move back to checkout directory before starting the next project
        cd ../../
    done
)
