#include <A-public>
#include <tapestry.h>
#include <A-init>

#define line(...)       new(line,       __VA_ARGS__)
#define tapestry(...)   new(tapestry,   __VA_ARGS__)
#define build(...)      new(build,      __VA_ARGS__)
#define dep(...)        new(dep,        __VA_ARGS__)

static path   src;
static path   install;
static path   cwd;
static string platform;

static cstr ws(cstr p) {
    cstr scan = p;
    while (*scan && *scan != '\n' && isspace(*scan))
        scan++;
    return scan;
}

string token_eval(string input) {
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

string dep_cast_string(dep a) {
    string config = string(alloc, 128);
    each (a->config, string, t) {
        if (len(config))
            append(config, " ");
        concat(config, t);
    }
    return config;
}

none build_with_path(build a, path f) {
    a->import   = array(32);
    array lines = read_lines(f);
    dep im = null;
    string last_platform = null;

    each(lines, line, l) {
        // now we can go by the indent levels on each line
        if (l->indent == 0) {
            string name   = get(l->tokens, 0);
            string uri    = get(l->tokens, 1);
            string commit = get(l->tokens, 2);
            im = dep(name, name, uri, uri, commit, commit);
            push(a->import, im);
            last_platform = null;
        } else {
            string first = get(l->tokens, 0);
            i32    flen  = len(first);
            if (first->chars[flen - 1] == ':') {
                last_platform = mid(first, 0, flen - 1);
            } else if (!last_platform || cmp(platform, last_platform) == 0) {
                each(l->tokens, string, t)
                    push(im->config, t);
            }
        }
    }
}

bool cmake(bool debug, object* conf, object* build, object* install) {
    *conf = *build = *install = null;
    if (!file_exists("CMakeLists.txt")) return false;
    cstr build = debug ? "debug" : "release";
    /// configure if needed
    if (!dir_exists("%s", build)) {
        string cmd = form(string, "cmake -B %o -S . %o", build, im);
        int icmd = system(cstring(cmd));
        *conf = A_i32(icmd);
        if (cmd != 0) return false;
    }
    /// build if needed
    return true;
}

bool cmake(bool debug, object* conf, object* build, object* install) {
    *conf = *build = *install = null;
    if (!file_exists("CMakeLists.txt")) return false;
}

bool gn(bool debug, object* conf, object* build, object* install) {
    *conf = *build = *install = null;
    if (!file_exists("DEPS")) return false;
    
    cd ..
    python3 tools/git-sync-deps # may need to be a command in import if this differs between gclient projects
    BUILD_CONFIG="'$BUILD_CONFIG"
    BUILD_CONFIG="$BUILD_CONFIG'"
    echo "bin/gn gen $BUILD_FOLDER --args=$BUILD_CONFIG"
    eval bin/gn gen $BUILD_FOLDER --args=$BUILD_CONFIG
    cd $BUILD_FOLDER
}

none build_init(build a) {

    /// make checkouts or symlink, then build & install
    each (a->import, dep, im) {
        chdir(cstring(install));

        /// checkout or symlink
        if (!dir_exists("%o/checkout/%o", install, im->name))
            if (len(src) && dir_exists("%o/%o", src, im->name)) {
                string cmd = form(string, "ln -s %o/%o %o/checkout/%o",
                    src, im->name, install, im->name);
                verify (system(cstring(cmd)) == 0, "symlink");
            } else {
                string cmd = form(string, "git clone %o %o --no-checkout && cd %o && git checkout %o && cd ..",
                    im->uri, im->name, im->name, im->commit);
                verify (system(cstring(cmd)) == 0, "git clone");
            }
        
        string n = im->name;
        path   project_path = form(path, "%o/%o", install, im->name);

        /// build project with evaluated config
        chdir(cstring(project_path));

        /// build based on precedence
        if (gn()) {} else (cmake()) {} else if (rust()) {} else if (autoconfig()) {} else if (make()) {}
        else fault("%o: unsupported-build-type", n);
        

    }
}

int main(int argc, cstr argv[]) {
    A_start();
    path  default_src     = form  (path, "%s", getenv("SRC")     ? getenv("SRC")     : "");
    path  default_install = form  (path, "%s", getenv("INSTALL") ? getenv("INSTALL") : ".");
    map   args            = A_args(argc, argv, "install", default_install, "src", default_src, null);

if defined(__linux__)
    platform = "windows";
#elif defined(_WIN32)
    platform = "linux";
#elif defined(__APPLE__)
    platform = "darwin";
#else
    platform = "unknown";
#endif

    src     = get(args, string("src"));
    install = get(args, string("install"));
    cwd     = path_cwd(2048);
    print("src = %o, and install = %o, from path = %o", src, install, cwd);

    path  f = form(path, "%o/build", cwd);
    build b = build(f);
    
    // tapestry can be given any install base, with a default of . [self] 
    // useful to be relative in the file-system as we may promote stable versioning and hierarchy

    // this will make; we must simply install it, unless we can route 'make' into this.  clever if possible.
    // turn make args... into tapestry args... ; how hard can it be
    // so we write tapestry to build apps, and it should be 444 lines for everything
    return 0;
}
define_class(line)
define_class(dep)
define_class(build)
define_class(tapestry)