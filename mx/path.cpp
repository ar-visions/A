#include <mx/mx.hpp>
#include <sys/stat.h>

namespace ion {

Path::Path() : A(typeof(Path)) { }

String* Path::to_string() {
    std::string str = p.string();
    return new String(str.c_str(), str.length());
}

Path *Path::cwd() {
    return path(dir::cwd());
}

Path::operator str() { return str(cs()); }

Path::Path(const str& s) : Path() {
    p = fs::path((symbol)s->cs());
}

Path::Path(symbol cs) : Path() {
    p = fs::path(cs);
}

bool Path::get_modified_date(struct tm *res) {
    const char* filename = cs();
    struct stat file_stat;

    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        return false;
    }
    static mutex mx;
    mx.lock();
    *res = *gmtime(&file_stat.st_mtime);
    mx.unlock();
    return true;
}

str Path::get_modified_string() {
    str result(128);
    struct tm gm_time;
    ///
    if (get_modified_date(&gm_time)) {
        // RFC 1123 format (used with http)
        if (strftime(result->chars, 128, "%a, %d %b %Y %H:%M:%S GMT", &gm_time) == 0)
            fprintf(stderr, "strftime failed\n");
        result->length = strlen(result->chars);
    }
    return result;
}


/// utility for knowing if you are trying to go up beyond a relative dir
/// without any dir analysis is more reduced
bool Path::backwards(cstr cs) {
    cstr v = cs;
    if (v[0] == '/') v++;
    double b = 0;
    ///
    for (; v[0];) {
        if (v[0] == '.' && v[1] == '.') {
            if (v[2] != 0 && v[2] != '/')
                return true; /// this is backwards enough
            v++;
            b--;
        } else if (v[0] != '/') {
            // double slash is same dir, we 'ignore' that
            b++;
            do {
                v++;
            } while (v[0] && v[0] != '/');

            if (v[0] == '/')
                v++;
        }
        if (v[0])
            v++;
    }
    return b < 0;
}

fs::path *Path::pdata() const {
    return (fs::path*)&p;
}

str Path::mime_type() {
    str ext_str = ext()->mid(1);
    M e = symbolize(ext_str->cs());
    static Path *data = new Path("data/mime-type.json");
    static var  vmap = data->read<var>();
    static map  js   = vmap.a->hold();
    Field      *find = js->fetch(e);
    return find ? find->value : js["default"];
}

str  Path::stem() const {
    if (!pdata()->empty()) {
        std::string stem_str = pdata()->stem().string();
        return str(stem_str.c_str(), stem_str.length());
    }
    return str();
}

bool Path::     remove_all() const { std::error_code ec;          return fs::remove_all(*pdata(), ec); }
bool Path::         remove() const { std::error_code ec;          return fs::remove(*pdata(), ec); }
bool Path::      is_hidden() const { auto st = pdata()->stem().string(); return st.length() > 0 && st[0] == '.'; }
bool Path::         exists() const {                              return fs::exists(*pdata()); }
bool Path::         is_dir() const {                              return fs::is_directory(*pdata()); }
bool Path::       make_dir() const { std::error_code ec;          return !pdata()->empty() ? fs::create_directories(*pdata(), ec) : false; }
Path *Path::remove_filename()       {
    fs::path p = pdata()->remove_filename();
    return new Path(p.string().c_str());
}
bool Path::   has_filename() const { return pdata()->has_filename(); }
Path *Path::          link() const { return fs::is_symlink(*pdata()) ? new Path(pdata()->string().c_str()) : new Path(); }
bool Path::        is_file() const { return !fs::is_directory(*pdata()) && fs::is_regular_file(*pdata()); }

char *Path::cs() const {
    static std::string static_thing;
    static_thing = pdata()->string();
    return (char*)static_thing.c_str();
}

Path::operator cstr() const {
    return cstr(cs());
}

str   Path::       ext () const {
    std::string s = pdata()->extension().string();
    return str(s.c_str(), s.length());
}
str   Path::       ext4() const {
    return ext();
}

Path* Path::       file() const { return fs::is_regular_file(*pdata()) ? new Path(cs()) : new Path(); }
bool Path::copy(Path *_to) const {
    path to(_to->hold());
    assert(!pdata()->empty());
    assert(exists());
    if (!to->exists())
        (to->has_filename() ?
            path(to->remove_filename()) : to)->make_dir();
    std::error_code ec;
    path p = to->is_dir() ? to / ext() : to;
    fs::copy(*pdata(), *p->pdata(), ec);
    _to->drop();
    return ec.value() == 0;
}

Path* Path::assert_file() {
    assert(fs::is_regular_file(*pdata()) && exists());
    return this;
}

/// create an alias; if successful return its location
Path* Path::link(Path* alias) const {
    fs::path &ap = *pdata();
    fs::path &bp = *alias->pdata();
    if (ap.empty() || bp.empty())
        return {};
    std::error_code ec;
    is_dir() ? fs::create_directory_symlink(ap, bp) : fs::create_symlink(ap, bp, ec);
    if (fs::exists(bp))
        return (Path*)alias;
    alias->drop();
    return new Path();
}

/// operators
Path::operator bool() const {
    return cs()[0];
}

Path::operator str() const { return str(cs()); }

Path* Path::parent()         const {
    std::string s = pdata()->parent_path().string();
    return new Path(s.c_str());
}


Path* Path::operator / (const Path&s) const { return path((*pdata() / *s.pdata()).string().c_str()); }
Path* Path::operator / (symbol     s) const { return path((*pdata() /  s).string().c_str()); }
Path* Path::operator / (const str& s) const { return path((*pdata() / fs::path(s->cs())).string().c_str()); }
Path* Path::relative   (Path*   from) const { return path(fs::relative(*pdata(), *from->pdata()).string().c_str()); }

bool  Path::operator==(const Path& b) const { return  *pdata() == *b.pdata(); }
bool  Path::operator!=(const Path& b) const { return !(operator==(b)); }

bool  Path::operator==(symbol      b) const { return *pdata() == b; }
bool  Path::operator!=(symbol      b) const { return !(operator==(b)); }

///
Path* Path::uid(Path* b) {
    fs::path p { };
    do {
        str r(8);
        for (int i = 0; i < 6; i++)
            r->append(rand::uniform('a', 'z'));
        p = fs::path(r);
    } while (fs::exists(*b->pdata() / p));
    std::string s = p.string();
    return new Path(s.c_str());
}

Path*  Path::format(str t, array args) {
    return new Path(t->format(args));
}

int64_t Path::modified_at() const {
    using namespace std::chrono_literals;
    std::string ps = pdata()->string();
    auto        wt = fs::last_write_time(*pdata());
    const auto  ms = wt.time_since_epoch().count(); // offset needed atm
    return int64_t(ms);
}

bool Path::read(size_t bs, lambda<void(symbol, size_t)> fn) {
    cstr  buf = new char[bs];
    fs::path *pdata = this->pdata();
    try {
        std::error_code ec;
        size_t rsize = fs::file_size(*pdata, ec);
        /// dont throw in the towel. no-exceptions.
        if (!ec)
            return false;
        std::ifstream f(*pdata);
        for (num i = 0, n = (rsize / bs) + (rsize % bs != 0); i < n; i++) {
            size_t sz = i == (n - 1) ? rsize - (rsize / bs * bs) : bs;
            fn((symbol)buf, sz);
        }
    } catch (std::ofstream::failure e) {
        std::cerr << "read failure: " << pdata->string();
    }
    delete[] buf;
    return true;
}

bool Path::append(const vector<u8> &bytes) {
    fs::path *pdata = this->pdata();
    try {
        size_t sz = bytes->len();
        std::ofstream f(*pdata, std::ios::out | std::ios::binary | std::ios::app);
        if (sz)
            f.write((symbol)bytes->data(), sz);
    } catch (std::ofstream::failure e) {
        std::cerr << "read failure on resource: " << pdata->string() << std::endl;
    }
    return true;
}

/// visual studio code should use this
bool Path::same_as  (Path* b) const { std::error_code ec; return fs::equivalent(*pdata(), *b->pdata(), ec); }

void Path::resources(array exts, states<option> states, Fn fn) {
    fs::path *pdata     = this->pdata();
    bool use_gitignore	= states[ option::use_git_ignores ];
    bool recursive		= states[ option::recursion       ];
    bool no_hidden		= states[ option::no_hidden       ];
    array ignore        = states[ option::use_git_ignores ] ?
        array(path((*pdata / ".gitignore").string().c_str())->read<str>()->split("\n")) :
        array();
    lambda<void(path)> res;
    map        fetched_dir;  /// this is temp and map needs a hash lambdas pronto
    fs::path   parent   = *pdata; /// parent relative to the git-ignore index; there may be multiple of these things.

    ///
    res = [&](path a) {
        auto fn_filter = [&](ion::path p) -> bool {
            str      ext = p->ext4();
            bool proceed = (!exts || exts->contains(ext)) ? (!no_hidden || !is_hidden()) : false;

            /// proceed if proceeding, and either not using git ignore,
            /// or the file passes the git ignore collection of patterns
            if (proceed && use_gitignore) {
                path    pp = path(parent.string().c_str());
                path   rel = pp->relative(p); // original parent, not resource parent
                str   srel = rel;
                ///
                for (String& i: ignore)
                    if (i && srel->has_prefix(i)) {
                        proceed = false;
                        break;
                    }
            }
            
            /// call lambda for resource
            if (proceed) {
                fn(p);
                return true;
            }
            return false;
        };

        /// directory of resources
        if (a->is_dir()) {
            if (!no_hidden || !a->is_hidden())
                for (fs::directory_entry e: fs::directory_iterator(*a->pdata())) {
                    std::string str = e.path().string();
                    path p  = str.c_str();
                    path li = p->link();
                    //if (li)
                    //    continue;
                    path pp = li ? li : p;
                    if (recursive && pp->is_dir()) {
                        if (fetched_dir->fetch(pp))
                            return;
                        fetched_dir[pp] = M(true);
                        res(pp);
                    }
                    ///
                    if (pp->is_file())
                        fn_filter(pp);
                }
        } /// single resource
        else if (a->exists())
            fn_filter(a);
    };
    std::string str = pdata->string();
    symbol sym = str.c_str();
    return res(sym);
}

array Path::matching(array exts) {
    array ret;
    resources(exts, { }, [&](path p) { ret += p; });
    return ret;
}

}