/// who throws away a perfectly good Watch?!
#pragma once

#include <mx/mx.hpp>
#include <async/async.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#if !defined(_WIN32)
#include <unistd.h>
#endif

namespace ion {
///
struct PathOp:A {
    ion::path path;
    size_t    path_index;
    Path::op  op;
    bool operator==(const Path::op &op) const { return this->op == op; }
    operator bool() { return path; }
    PathOp() : A(typeof(PathOp)) { }
};

struct path_op {
    A_decl(path_op, PathOp)
};

struct PathState:A {
    path        res;
    size_t      path_index;
    i64         modified;
    bool        found;
    void reset() { found = false; }
    operator bool() { return res; }
    PathState() : A(typeof(PathState)) { }
};

struct path_state {
    A_decl(path_state, PathState)
};

struct Watch:A {
    using fn = lambda<void(bool, array &)>; // path_op

    bool             safe;
    bool        canceling;
    const int     polling = 1000;
    array           paths;
    states<Path::option> options;
    fn           watch_fn;
    array             ops; // path_op
    int              iter;
    int           largest;
    array            exts;
    map              path_states;

    static Watch* spawn(array paths, array exts, states<Path::option> options, Watch::fn watch_fn);
    
    void stop();

    operator bool() { return !canceling; }
    Watch() : A(typeof(Watch)) { }
};

struct watch {
    A_decl(watch, Watch)
};

}