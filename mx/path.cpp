#include <mx/mx.hpp>

namespace ion {

static path path::cwd() {
    return dir::cwd();
}


memory* path::M::to_string() {
    static std::string str;
    str = p.string();
    return memory::string(str);
}


path::path(str      s) : path() {
    data->p = fs::path((symbol)s.cs());
}

path::path(symbol  cs) : path() {
    data->p = fs::path(cs);
}

bool path::get_modified_date(struct tm *res);
str  path::get_modified_string();

/// utility for knowing if you are trying to go up beyond a relative dir
/// without any dir analysis is more reduced
bool path::backwards(cstr cs) {
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

fs::path *path::pdata() const {
    return &data->p;
}

str  path::      mime_type();
str  path::           stem() const { return !pdata()->empty() ? str(pdata()->stem().string()) : str(null);    }
bool path::     remove_all() const { std::error_code ec;          return fs::remove_all(*pdata(), ec); }
bool path::         remove() const { std::error_code ec;          return fs::remove(*pdata(), ec); }
bool path::      is_hidden() const { auto st = pdata()->stem().string(); return st.length() > 0 && st[0] == '.'; }
bool path::         exists() const {                              return fs::exists(*pdata()); }
bool path::         is_dir() const {                              return fs::is_directory(*pdata()); }
bool path::       make_dir() const { std::error_code ec;          return !pdata()->empty() ? fs::create_directories(*pdata(), ec) : false; }
path path::remove_filename()       {
    fs::path p = pdata()->remove_filename();
    return path(p.string().c_str());
}
bool path::   has_filename() const { return pdata()->has_filename(); }
path path::           link() const { return fs::is_symlink(*pdata()) ? path(pdata()->string().c_str()) : path(); }
bool path::        is_file() const { return !fs::is_directory(*pdata()) && fs::is_regular_file(*pdata()); }
char path::            *cs() const {
    static std::string static_thing;
    static_thing = pdata()->string();
    return (char*)static_thing.c_str();
}
path::operator cstr() const {
    return cstr(cs());
}

str  path::       ext () const { return str(pdata()->extension().string()); }
str  path::       ext4() const { return pdata()->extension().string(); }
path path::       file() const { return fs::is_regular_file(*pdata()) ? path(cs()) : path(); }
bool path::copy(path to) const {
    assert(!pdata()->empty());
    assert(exists());
    if (!to.exists())
        (to.has_filename() ?
            to.remove_filename() : to).make_dir();

    std::error_code ec;
    path p = to.is_dir() ? to / path(str(pdata()->filename().string())) : to;
    fs::copy(*pdata(), *p.pdata(), ec);
    return ec.value() == 0;
}

path &path::assert_file() {
    assert(fs::is_regular_file(*pdata()) && exists());
    return *this;
}

/// create an alias; if successful return its location
path path::link(path alias) const {
    fs::path &ap = *pdata();
    fs::path &bp = *alias.pdata();
    if (ap.empty() || bp.empty())
        return {};
    std::error_code ec;
    is_dir() ? fs::create_directory_symlink(ap, bp) : fs::create_symlink(ap, bp, ec);
    return fs::exists(bp) ? alias : path();
}

/// operators
operator      path::  bool()         const {
    return cs()[0];
}
operator      path::   str()         const { return str(cs()); }
path          path::parent()         const {
    std::string s = pdata()->parent_path().string();
    return  s.c_str();
}

path path::operator / (path       s) const { return path((*pdata() / *s.pdata()).string().c_str()); }
path path::operator / (symbol     s) const { return path((*pdata() /  s).string().c_str()); }
path path::operator / (const str& s) const { return path((*pdata() / fs::path(s.data)).string().c_str()); }
path path::relative   (path    from) const { return path(fs::relative(*pdata(), *from.pdata()).string().c_str()); }

bool  path::operator==(path&      b) const { return  *pdata() == *b.pdata(); }
bool  path::operator!=(path&      b) const { return !(operator==(b)); }

bool  path::operator==(symbol     b) const { return *pdata() == b; }
bool  path::operator!=(symbol     b) const { return !(operator==(b)); }

///
static path path::uid(path b) {
    auto rand = [](num i) -> str { return rand::uniform('a', 'z'); };
    fs::path p { };
    do { p = fs::path(str::fill(6, rand)); }
    while (fs::exists(*b.pdata() / p));
    std::string s = p.string();
    return  s.c_str();
}

static path  path::format(str t, array args) {
    return t.format(args);
}

int64_t path::modified_at() const {
    using namespace std::chrono_literals;
    std::string ps = pdata()->string();
    auto        wt = fs::last_write_time(*pdata());
    const auto  ms = wt.time_since_epoch().count(); // offset needed atm
    return int64_t(ms);
}

bool path::read(size_t bs, lambda<void(symbol, size_t)> fn) {
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

bool path::append(const array &bytes) {
    assert(bytes.type() == typeof(u8));
    fs::path *pdata = this->pdata();
    try {
        size_t sz = bytes.len();
        std::ofstream f(*pdata, std::ios::out | std::ios::binary | std::ios::app);
        if (sz)
            f.write((symbol)bytes.data<u8>(), sz);
    } catch (std::ofstream::failure e) {
        std::cerr << "read failure on resource: " << pdata->string() << std::endl;
    }
    return true;
}

/// visual studio code should use this
bool path::same_as  (path b) const { std::error_code ec; return fs::equivalent(*pdata(), *b.pdata(), ec); }

void path::resources(array exts, states<option> states, Fn fn) {
    fs::path *pdata     = this->pdata();
    bool use_gitignore	= states[ option::use_git_ignores ];
    bool recursive		= states[ option::recursion       ];
    bool no_hidden		= states[ option::no_hidden       ];
    array ignore        = states[ option::use_git_ignores ] ? path((*pdata / ".gitignore").string().c_str()).read<str>().split("\n") : array();
    lambda<void(path)> res;
    map        fetched_dir;  /// this is temp and map needs a hash lambdas pronto
    fs::path   parent   = *pdata; /// parent relative to the git-ignore index; there may be multiple of these things.

    ///
    res = [&](path a) {
        auto fn_filter = [&](ion::path p) -> bool {
            str      ext = p.ext4();
            bool proceed = (!exts || exts.contains(ext)) ? (!no_hidden || !is_hidden()) : false;

            /// proceed if proceeding, and either not using git ignore,
            /// or the file passes the git ignore collection of patterns
            if (proceed && use_gitignore) {
                path    pp = path(parent.string().c_str());
                path   rel = pp.relative(p); // original parent, not resource parent
                str   srel = rel;
                ///
                for (str& i: ignore.elements<str>())
                    if (i && srel.has_prefix(i)) {
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
        if (a.is_dir()) {
            if (!no_hidden || !a.is_hidden())
                for (fs::directory_entry e: fs::directory_iterator(*a.pdata())) {
                    std::string str = e.path().string();
                    path p  = str.c_str();
                    path li = p.link();
                    //if (li)
                    //    continue;
                    path pp = li ? li : p;
                    if (recursive && pp.is_dir()) {
                        if (fetched_dir.lookup(mx(pp)))
                            return;
                        fetched_dir[pp] = mx(true);
                        res(pp);
                    }
                    ///
                    if (pp.is_file())
                        fn_filter(pp);
                }
        } /// single resource
        else if (a.exists())
            fn_filter(a);
    };
    std::string str = pdata->string();
    symbol sym = str.c_str();
    return res(sym);
}

array path::matching(array exts) {
    auto ret = array();
    resources(exts, { }, [&](path p) { ret += p; });
    return ret;
}

path::operator str() { return str(cs(), memory::autolen); }


}