#!/bin/bash
(
    # built projects from environment
    BUILT_PROJECTS="${BUILT_PROJECTS:-}"
    CALLER_BUILD_DIR=""

    # parse args
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --src)
                SRC_DIR="$2"
                shift 2
                ;;
            --b)
                CALLER_BUILD_DIR="$2"
                shift 2
                ;;
            --i)
                IMPORT="$2"
                shift 2
                ;;
            *)
                BUILD_ROOT="$1"
                shift
                ;;
        esac
    done

    print() {
        if [ "${VERBOSE}" = "1" ]; then
            echo "$@"
        fi
    }

    if [ -z "$CALLER_BUILD_DIR" ]; then
        echo '--b required argument (project build dir)'
        exit
    fi

    if [ ! -f "$CALLER_BUILD_DIR/.rebuild" ]; then
        touch $CALLER_BUILD_DIR/.rebuild
    fi

    if [ -z "$IMPORT" ]; then
        echo '--i required argument (import file)'
        exit
    else
        projects=()  # Initialize an empty array
        found_deps=1
        ws_count=0
        current_line=""

        while IFS= read -r raw_line || [ -n "$raw_line" ]; do
            line=$(echo "$raw_line" | tr '\t' ' ' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
            if [ -z "$line" ]; then
                continue
            fi
            if [[ $line =~ ^[[:alnum:]][[:alnum:]_-]*: ]]; then
                found_deps=0
            fi
            if [[ $found_deps -eq 1 ]]; then
                ws=$(echo "$raw_line" | tr '\t' ' ' | sed -E 's/^( *).*/\1/' | wc -c)
                if [ $ws -le 1 ]; then
                    if [ ! -z "$current_line" ]; then
                        projects+=("$current_line")
                        print "import: $current_line"
                    fi
                    current_line="$line"
                elif [ $ws -ge 2 ]; then
                    current_line="$current_line $line"
                fi
                
            fi
        done < $IMPORT

        # Add the last current_line if it exists
        if [ ! -z "$current_line" ]; then
            projects+=("$current_line")
            #echo "import: $current_line"
        fi
    fi

    SRC_DIR="${SRC_DIR:-$SRC}"
    BUILD_ROOT="${BUILD_ROOT:-silver-import}"

    export SILVER_IMPORT="$BUILD_ROOT"

    if [ "${VERBOSE}" = "1" ]; then
        MFLAGS=""
    else
        MFLAGS="-s"
    fi

    if [ -z "$DEB" ]; then
        DEB=","
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

        # check if project already built during this command stack
        if [[ ",$BUILT_PROJECTS," == *",$PROJECT_NAME,"* ]]; then
            print "skipping $PROJECT_NAME - already built"
            continue
        fi

        REPO_URL=$(    echo "$project" | cut -d ' ' -f 2)
        COMMIT=$(      echo "$project" | cut -d ' ' -f 3)
        BUILD_CONFIG=$(echo "$project" | cut -d ' ' -f 4-)
        TARGET_DIR="${PROJECT_NAME}"
        A_MAKE="0" # A-type projects use Make, but with a build-folder and no configuration; DEBUG=1 to enable debugging
        IS_DEBUG=
        # set build folder based on release/debug
        if [[ ",$DEB," == *",$PROJECT_NAME,"* ]]; then
            BUILD_FOLDER="silver-debug"
            IS_DEBUG=1
        else
            BUILD_FOLDER="silver-build"
        fi
        
        # Check if --src directory and project exist, then symlink
        if [ -n "$SRC_DIR" ] && [ -d "$SRC_DIR/$TARGET_DIR" ]; then
            SYMLINK_TARGET=$(readlink "$TARGET_DIR")
            if [ "$SYMLINK_TARGET" != "$SRC_DIR/$TARGET_DIR" ]; then
                rm -rf "$TARGET_DIR"
                echo "symlinking $TARGET_DIR to $SRC_DIR/$TARGET_DIR..."
                ln -s "$SRC_DIR/$TARGET_DIR" "$TARGET_DIR" || exit 1
            fi
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
                        rm -f "$BUILD_FOLDER/silver-token" || { echo "silver-token failed to rm" >&2; exit 1; }
                    fi
                fi
            else
                echo "cloning repository $REPO_URL into $TARGET_DIR..."
                echo git clone "$REPO_URL" "$TARGET_DIR"
                git clone "$REPO_URL" "$TARGET_DIR"
                #echo git clone --recursive "$REPO_URL" "$TARGET_DIR"
                #git clone --recursive "$REPO_URL" "$TARGET_DIR"
                
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
            if [ -f silver-init.sh ]; then
                echo 'running silver-init.sh'
                bash silver-init.sh
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
        if [ -n "$IS_DEBUG" ]; then
            build="debug"
        else
            build="release"
        fi
        rust=""
        cmake=""
        BUILD_TYPE=""
        silver_build=""
        # check if this is a cmake project, otherwise use autotools
        # or if there is "-S " in BUILD_CONFIG
        if [ -f "silver-build.sh" ]; then
            silver_build="1"
            echo "[$PROJECT_NAME] silver-build"
        elif [ -f "CMakeLists.txt" ] || [[ "$BUILD_CONFIG" == *-S* ]]; then
            cmake="1"
            if [[ ",$DEB," == *",$PROJECT_NAME,"* ]]; then
                print "selecting DEBUG for $PROJECT_NAME"
                BUILD_TYPE="-DCMAKE_BUILD_TYPE=Debug"
            else
                print "selecting RELEASE for $PROJECT_NAME"
                BUILD_TYPE="-DCMAKE_BUILD_TYPE=Release"
            fi
        elif [ -f "Cargo.toml" ]; then
            rust="1"
        else
            if [ -f "configure.ac" ] || [ -f "config" ] ||[ -f "configure.in" ] || [ -f "configure" ]; then
                if [[ ",$DEB," == *",$PROJECT_NAME,"* ]]; then
                    BUILD_TYPE="--enable-debug"
                fi
            else
                A_MAKE="1"
                BUILD_TYPE="-g"
            fi
        fi

        mkdir -p $BUILD_FOLDER
        cd $BUILD_FOLDER

        if [ -n "$silver_build" ]; then
            # arbitrary build process is sometimes needed; some repos have no Make, etc
            echo 'running silver-build.sh'
            (cd .. && bash silver-build.sh)
        else
            if [ "$REBUILD" == "all" ] || [ "$REBUILD" == "$PROJECT_NAME" ]; then
                echo "rebuilding ${TARGET_DIR}"
                if [ -n "$cmake" ]; then
                    cmake --build . --target clean
                elif [ -n "$rust" ]; then
                    cargo clean
                elif [ "$A_MAKE" = "1" ]; then
                    make $MFLAGS -f ../Makefile clean
                else
                    make clean
                fi
                rm -rf silver-token
            fi

            # proceed if there is a newer file than silver-token, or no silver-token
            if [ ! -f "silver-token" ] || find .. -type f -newer "silver-token" | grep -q . ; then
                # we need more than this: it needs to also check if a dependency registered to this project changes
                # to do that, we should run the import on the project without the make process
                BUILD_CONFIG=$(echo "$BUILD_CONFIG" | sed "s|\$SILVER_IMPORT|$SILVER_IMPORT|g")

                if [ -n "$cmake" ]; then
                    if [ ! -f "CMakeCache.txt" ] || [ "$IMPORT" -nt "silver-token" ]; then
                        BUILD_CONFIG="${BUILD_CONFIG% }"
                        if [ -z "$BUILD_CONFIG" ]; then
                            BUILD_CONFIG="-S .."
                        fi
                        cmake -B . -S .. $BUILD_TYPE $BUILD_CONFIG -DCMAKE_INSTALL_PREFIX="$BUILD_ROOT" 
                    fi
                elif [ -n "$rust" ]; then
                    rust="$rust"
                elif [ "$A_MAKE" = "1" ]; then
                    print "$PROJECT_NAME: simple Makefile [ no configuration needed ]"
                else
                    if [ ! -f "../configure" ] || [ "$IMPORT" -nt "silver-token" ]; then
                        if [ -f "../configure.ac" ]; then
                            if [ ! -f "../configure" ]; then
                                echo "running autoreconf -i in $PROJECT_NAME ($cmake)"
                                autoupdate ..
                                autoreconf -i ..
                            fi
                            if [ ! -f "../configure" ]; then
                                echo "configure script not available after running autoreconf -i"
                                exit 1
                            fi
                            echo ../configure $BUILD_TYPE --prefix=$BUILD_ROOT $BUILD_CONFIG
                            ../configure $BUILD_TYPE --prefix=$BUILD_ROOT $BUILD_CONFIG
                        else
                            if [ -f "../config" ]; then
                                if [ ! -f "Makefile" ]; then
                                    echo ../config $BUILD_TYPE --prefix=$BUILD_ROOT $BUILD_CONFIG
                                    ../config $BUILD_TYPE --prefix=$BUILD_ROOT $BUILD_CONFIG
                                fi
                            else
                                echo "error: no configure.ac found for $PROJECT_NAME"
                                exit 1
                            fi
                        fi
                    fi
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
                    j=$(($(nproc) / 2))
                fi

                export MAKEFLAGS="-j$j --no-print-directory"

                # build
                print "building $TARGET_DIR with -j$j in $BUILD_FOLDER"

                ts0=$(find . -type f -exec stat -c %Y {} + | sort -n | tail -1)
                if [ -n "$cmake" ]; then
                    cmake --build . -- -j$j
                elif [ -n "$rust" ]; then
                    cargo build --$build --manifest-path ../Cargo.toml --target-dir .
                elif [ "$A_MAKE" = "1" ]; then
                    make $MFLAGS -f ../Makefile
                else
                    #1/2/4/1/"a"
                    make $MFLAGS -j$j
                fi
                ts1=$(find . -type f -exec stat -c %Y {} + | sort -n | tail -1)
                echo "project $PROJECT_NAME = $ts0 $ts1"
                if [ $? -ne 0 ]; then
                    echo "build failure for $TARGET_DIR"
                    exit 1
                fi

                export MAKEFLAGS="-j1 --no-print-directory"

                # install to silver-import
                if [ "$ts0" != "$ts1" ]; then
                    touch $CALLER_BUILD_DIR/.rebuild
                    echo "$PROJECT_NAME modified: (you should see a rebuild!)"
                    print "install ${TARGET_DIR}"
                    if [ -n "$cmake" ]; then
                        cmake --install .
                    elif [ -n "$rust" ]; then
                        cp -r ./$build/*.so $SILVER_IMPORT/lib/
                    elif [ "$A_MAKE" = "1" ]; then
                        make $MFLAGS -f ../Makefile install
                    else
                        make $MFLAGS install
                    fi
                    if [ $? -ne 0 ]; then
                        echo "install failure for $TARGET_DIR"
                        exit 1
                    fi
                    if [ -f ../silver-post.sh ]; then
                        echo "running silver-post.sh $BUILD_FOLDER"
                        (cd .. && bash silver-post.sh $BUILD_FOLDER)
                    fi
                fi
            fi
            echo "im-a-token" >> silver-token  # only create this if all steps succeed
        fi

        BUILT_PROJECTS+=",${PROJECT_NAME}"
        export BUILT_PROJECTS

        cd ../../

    done
)
