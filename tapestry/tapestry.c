#include <A-public>
#undef link
#include <tapestry.h>
#include <A-init>
#include <sys/stat.h>
#include <utime.h>

#define line(...)       new(line,       __VA_ARGS__)
#define tapestry(...)   new(tapestry,   __VA_ARGS__)
#define build(...)      new(build,      __VA_ARGS__)
#define import(...)     new(import,     __VA_ARGS__)
#define flag(...)       new(flag,       __VA_ARGS__)

static path   src;
static path   install;
static string dbg;

#if defined(__linux__)
static symbol platform = "windows";
#elif defined(_WIN32)
static symbol platform = "linux";
#elif defined(__APPLE__)
static symbol platform = "darwin";
#endif

static cstr ws(cstr p) {
    cstr scan = p;
    while (*scan && *scan != '\n' && isspace(*scan))
        scan++;
    return scan;
}

static string token_eval(string input) {
    string cmd = form(string, "sh -c \"%o\"", input);
    FILE* pipe = popen(cstring(cmd), "r");
    if (!pipe) return string("");
    char   buf[1024];
    string result = string(alloc, 64);
    while (fgets(buf, sizeof(buf), pipe))
        append(result, buf);
    pclose(pipe);
    return trim(result);
}

static cstr read_token(cstr i, string* result) {
    i = ws(i);
    if (*i == '\n') {
        *result = null;
        return i;
    }
    string res = null;
    while (*i && !isspace(*i)) {
        if (!res)
             res = string(alloc, 64);
        append_count(res, i, 1);
        i++;
    }
    *result = token_eval(res);
    return i;
}

static cstr read_indent(cstr i, i32* result) {
    int t = 0;
    int s = 0;
    while (*i == ' ' || *i == '\t') {
        if (*i == ' ')
            s++;
        else if (*i == '\t')
            t++;
        i++;
    }
    *result = t + s / 4;
    return i;
}

static array read_lines(path f) {
    array  lines   = array(256);
    string content = read (f, typeid(string));
    cstr   origin  = cstring(content);
    cstr   scan    = origin;
    while (scan && *scan) {
        string t = null;
        scan = read_token(scan, &t);
        if (!t)
            break;
        i32 indent = 0;
        scan = read_indent(scan, &indent);
        line l = line(indent, indent, tokens, array(32));
        for (;;) {
            push(l->tokens, t);
            t = null;
            scan = read_token(scan, &t);
            if (!t)
                break;
        }
        push(lines, l);
    }
    return lines;
}

string import_cast_string(import a) {
    string config = string(alloc, 128);
    each (a->config, string, t) {
        if (len(config))
            append(config, " ");
        concat(config, t);
    }
    return config;
}

string flag_cast_string(flag a) {
    string res = string(64);

    if (a->is_cflag)
        concat(res, a->name);
    else {
        if (a->is_static)
            append(res, "-Wl,-Bstatic ");
        concat(res, form(string, "-l%o", a->name));
        if (a->is_static)
            append(res, " -Wl,-Bdynamic");
    }
    return res;
}

none build_with_path(build a, path f) {
    a->imports = array(32);
    a->project_path = directory(f);
    array lines = read_lines(f);
    import im = null;
    string last_platform  = null;
    string last_directive = null;
    string top_directive  = null;
    bool   is_import      = true;

    each(lines, line, l) {
        string first = get(l->tokens, 0);
        i32    flen  = len(first);

        // now we can go by the indent levels on each line
        if (l->indent == 0) {
            verify(first->chars[flen - 1] == ':', "expected base-directive and ':'");
            last_directive = mid(first, 0, flen - 1);
            verify(cmp(last_directive, "import") == 0 ||
                   cmp(last_directive, "link")   == 0, "expected import or link directive");
            is_import = cmp(last_directive, "import") == 0;
            top_directive = copy(last_directive);
        } else if (l->indent == 1) {
            if (is_import) {
                string name   = get(l->tokens, 0);
                string uri    = get(l->tokens, 1);
                string commit = get(l->tokens, 2);
                im = import(name, name, uri, uri, commit, commit,
                    config, array(32), commands, array(32));
                push(a->imports, im);
                last_platform = null;
            } else {
                /// we need pre-build scripts too, to generate headers and such
                array flags =
                    cmp(top_directive, "lib")  == 0 ? a->lib  :
                    cmp(top_directive, "app")  == 0 ? a->app  : 
                    cmp(top_directive, "test") == 0 ? a->test : null;
                verify(flags, "invalid directive: %o", top_directive);
                each (l->tokens, string, t) {
                    bool is_cflag  = t->chars[0] == '-';
                    bool is_static = t->chars[0] == '@';
                    push(flags, flag(name, t,
                        is_static, is_static, is_cflag, is_cflag));
                }
            }
        } else {
            if (first->chars[flen - 1] == ':') {
                last_platform = mid(first, 0, flen - 1);
            } else if (!last_platform || cmp(last_platform, platform) == 0) {
                if (cmp(first, ">") == 0) {
                    // combine tokens into singular string
                    string cmd = string(alloc, 64);
                    each(l->tokens, string, t) {
                        if (len(cmd))
                            append(cmd, " ");
                        concat(cmd, t);
                    }
                    push(im->commands, cmd);
                } else
                each(l->tokens, string, t)
                    push(im->config, t);
            }
        }
    }
}

bool is_debug(string name) {
    string f = form(string, ",%o,", dbg);
    string s = form(string, ",%o,", name);
    return strstr(f->chars, s->chars) != null;
}

/// handle autoconfig (if present) and resultant / static Makefile
bool import_make(import im) {
    i64         conf_status = INT64_MIN;
    struct stat build_token,
                installed_token;
    bool        debug      = is_debug(im->name);
    symbol      build_type = debug ? "debug" : "release";
    path        build_path = form(path, "%o/%s", im->import_path, build_type);
    path t0 = form(path, "tapestry-token");
    path t1 = form(path, "%o/tokens/%o", install, im->name);

    if (file_exists("%o", t0) && file_exists("%o", t1)) {
        /// get modified date on token, compare it to one in install dir
        int istat_build   = stat(cstring(t0), &build_token);
        int istat_install = stat(cstring(t1), &installed_token);

        if (istat_build == 0 && istat_install)
            conf_status = (i64)(build_token.st_mtime - installed_token.st_mtime);
    }

    if (conf_status == 0)
        return true;

    /// GN support
    if (file_exists("../DEPS")) {
        cstr   prev = getenv("PATH");

        /// clone depot tools if we need it
        if (!strstr(prev, "depot_tools")) {
            if (!dir_exists("%o/depot_tools", src)) {
                string cmd = form(string, "cd %o && git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git", src);
                verify(system(cstring(cmd)) == 0, "depot_tools");
            }
            /// it may just need to be set in the PATH again, because this wont persist if they user does not have it
            string new_path = form(string, "%o/depot_tools:%o", src, prev);
            setenv("PATH", cstring(new_path), 1);
        }
        cd(im->import_path);
    
        /// now call gclient sync in the project folder
        verify(system("gclient sync -D") == 0, "gclient");
        verify(system("python3 tools/git-sync-deps") == 0, "git-sync-deps");
        string cmd = form(string, "bin/gn gen %s --args='%o'",
            debug ? "debug" : "release", im, im->config);
        verify(system(cstring(cmd)) == 0, "gn config");

    } else if (file_exists("../Cargo.toml")) {
        string cmd = form(string, "cargo build --%s --manifest-path ../Cargo.toml --target-dir .",
            debug ? "debug" : "release");
        verify(system(cstring(cmd)) == 0, "rust compilation");

    } else if (file_exists("../CMakeLists.txt")) {

        /// cmake configure
        if (!file_exists("CMakeCache.txt")) {
            string cmd = form(string, "cmake -B . -S .. %o", im);
            int  iconf = system(cstring(cmd));
            verify(iconf == 0, "%o: configure failed", im->name);
        }
    
        /// build if needed
        string  cmd = form(string, "cmake --build .");
        int    icmd = system(cstring(cmd));
        
        /// install if needed
        string inst = form(string, "cmake --install .");
        int   iinst = system(cstring(inst));

    } else {

        if (file_exists("../configure.ac") || file_exists("../configure") || file_exists("../config")) {
            /// generate configuration scripts if available
            if (!file_exists("../configure") && file_exists("../configure.ac")) {
                verify(system("autoupdate ..")    == 0, "autoupdate");
                verify(system("autoreconf -i ..") == 0, "autoreconf");
            }

            /// prefer our pre/generated script configure, fallback to config
            cstr configure = file_exists("../configure") ? "../configure" : "../config";
            if (file_exists("%s", configure)) {
                string cmd_conf = form(string, "%s%s --prefix=%o %o",
                    configure,
                    debug ? " --enable-debug" : "",
                    install,
                    im);
                verify(system(cstring(cmd_conf)) == 0, configure);
            }
        }

        if (file_exists("../Makefile")) {
            /// now run make install
            string make = form(string, "make -f ../Makefile install");
            verify(system(cstring(make)) == 0, "make install");
        }
    }

    /// create token file here, as long as no errors
    cstr both[2] = { cstring(t0), cstring(t1) };
    for (int i = 0; i < 2; i++) {
        FILE* ftoken = fopen(both[i], "wb");
        fwrite("im-a-token", 10, 1, ftoken);
        fclose(ftoken);
    }
    int istat_build   = stat(cstring(t0), &build_token);
    int istat_install = stat(cstring(t1), &installed_token);
    struct utimbuf times;
    times.actime  = build_token.st_atime;  // access time
    times.modtime = build_token.st_mtime;  // modification time
    utime(cstring(t1), &times);
    return true;
}

static bool is_dbg(cstr name) {
    cstr dbg = getenv("DBG");
    return dbg ? strstr(dbg, name) != 0 : false;
}

none build_init(build a) {
    /// make checkouts or symlink, then build & install
    each (a->imports, import, im) {
        cd(install);

        /// checkout or symlink
        if (!dir_exists("%o/checkout/%o", install, im->name)) {
            if (A_len(src) && dir_exists("%o/%o", src, im->name)) {
                string cmd = form(string, "ln -s %o/%o %o/checkout/%o",
                    src, im->name, install, im->name);
                verify (system(cstring(cmd)) == 0, "symlink");
            } else {
                string cmd = form(string, "git clone %o %o --no-checkout && cd %o && git checkout %o && cd ..",
                    im->uri, im->name, im->name, im->commit);
                verify (system(cstring(cmd)) == 0, "git clone");
            }
        }
        
        string n = im->name;
        im->import_path = form(path, "%o/%o", install, im->name);

        /// build project with evaluated config
        i32* c = null, *b = null, *i = null;
        bool debug = is_dbg(cstring(im->name));
        im->build_path = form(path, "%o/%s", im->import_path, debug ? "debug" : "release");
        cd(im->build_path);

        /// make is implicit install in our modeling
        make(im);
    }

    /// goto project path
    cd(a->project_path);
    symbol      build_type = debug ? "debug" : "release";
    a->build_path = form(path, "%o/%s", a->project_path, build_type);
    cstr CC     = getenv("CC");     if (!CC)     CC     = "gcc";
    cstr CXX    = getenv("CXX");    if (!CXX)    CXX    = "g++";
    cstr CFLAGS = getenv("CFLAGS"); if (!CFLAGS) CFLAGS = "";
    cstr CXXFLAGS = getenv("CXXFLAGS"); if (!CXXFLAGS) CXXFLAGS = "";
    string name = filename(a->project_path);
    bool cpp    = false;
    cstr compilers[2] = { CC, CXX };
    /// iterate through directives
    struct directive {
        array list;
        cstr  dir;
    } directives[3] = {
        {a->lib, "lib"}, {a->app, "app"}, {a->test, "test"}
    };
    for (int i = 0; i < sizeof(directives) / sizeof(directive); i++) {
        struct directive* flags = &directives[i];
        bool is_lib = strcmp(flags->dir, "lib") == 0;
        path  dir = form(path, "%o/%s", a->project_path, flags->dir);
        if (!dir_exists("%o", dir)) continue;
        array c   = ls(dir, ".c",   false); // returns absolute paths
        array cc  = ls(dir, ".cc",  false);
        array src = array(32);
        cpp      |= len(cc) > 0;
        array obj    = array(64);
        array obj_c  = array(64);
        array obj_cc = array(64);
        path href = form(path, "%o/%o", dir, name);
        if (!file_exists("%o", href))
             href = form(path, "%o/%o.h", dir, name);
        
        array files = ls(dir, null, false);
        i64   htime = 0;

        /// get latest header modified (including project header)
        each (files, path, f) {
            string e = ext(f);
            string s = stem(f);
            if (cmp(s, "h") == 0 || (len(e) == 0 && cmp(s, name->chars) == 0)) {
                i64 mt = modified_time(f);
                if (mt > htime)
                    htime = mt;
            }
        }

        string cflags = string(alloc, 64);
        string cxxflags = string(alloc, 64);
        each(flags->list, flag, fl) {
            if (fl->is_cflag) {
                concat(cflags, cast(string, fl));
                append(cflags, " ");
            }
        }

        /// compile C with cflags
        struct lang {
            array source;
            array objs;
            cstr  compiler;
            cstr  std;
        } langs[2] = {{c, obj_c, compilers[0], "c11"}, {cc, obj_cc, compilers[1], "c++17"}};
        for (int l = 0; l < 2; l++) {
            struct lang* lang = &langs[l];
            if (!lang->std) continue;
            string compiler = form(string, "%s -std=%s %s %o",
                lang->compiler, lang->std, l == 0 ? CFLAGS : CXXFLAGS, l == 0 ? cflags : cxxflags);
            each(lang->source, path, src) {
                path o_path  = form(path, "%o.o", src);
                push(lang->objs, o_path);

                /// recompile if newer / includes differ
                i64 mtime = modified_time(src);
                if (mtime > modified_time(o_path) || (htime && htime > mtime)) {
                    string cmd = form(string, "%o %o -o %o", compiler, src, o_path);
                    verify(system(cstring(cmd)) == 0, "compilation");
                }
            }
        }

        // link
        string lflags = string(alloc, 64);
        each(flags->list, flag, fl) {
            if (!fl->is_cflag) {
                concat(lflags, cast(string, fl));
                append(lflags, " ");
            }
        }

        if (is_lib) {
            string cmd = form(string, "%o -shared %o -o %o", cpp ? CXX : CC,
                obj, name);
            verify(system(cstring(cmd)) == 0, "lib");
        } else {
            each (obj_c, path, obj) {
                string cmd = form(string, "%s %o -o %o", CC, obj, name);
                verify(system(cstring(cmd)) == 0, "app");
            }
            each (obj_cc, path, obj) {
                string cmd = form(string, "%s %o -o %o", CXX, obj, name);
                verify(system(cstring(cmd)) == 0, "app");
            }
        }
    }
}

int main(int argc, cstr argv[]) {
    A_start();
    cstr  SRC = getenv("SRC"), INSTALL = getenv("INSTALL");
    cstr  DBG = getenv("DBG");
    path  default_loc     = form  (path, "%s", ".");
    path  default_src     = form  (path, "%s", SRC     ? SRC     : "");
    path  default_install = form  (path, "%s", INSTALL ? INSTALL : ".");
    map   args            = A_args(argc, argv,
        "build", default_loc, "install", default_install, "src", default_src, null);

    path loc_unrel     = get(args, string("build"));
    path src_unrel     = get(args, string("src"));
    path install_unrel = get(args, string("install"));

    dbg      = DBG ? string(DBG) : string("");
    src      = absolute(src_unrel);
    install  = absolute(install_unrel);
    path loc = absolute(loc_unrel);
    path f   = null;

    if (dir_exists("%o", loc) && file_exists("%o/build", loc))
        f  = form(path, "%o/build", loc);
    else if (file_exists("%o", loc))
        f  = form(path, "%o", loc);
    else
        fault("build: not-found at location %o", loc);
    
    path abs = absolute(f);
    build  b = build(abs);
    return 0;
}
define_class(line)
define_class(import)
define_class(build)
define_class(flag)