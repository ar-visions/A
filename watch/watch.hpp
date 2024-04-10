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
struct path_op:mx {
    struct M {
        ion::path path;
        size_t    path_index;
        path::op  op;
    };
    mx_object(path_op, mx, M);
    bool operator==(path::op op) const { return op == data->op; }
};

struct path_state:mx {
    struct state {
        path        res;
        size_t      path_index;
        i64         modified;
        bool        found;
        void reset() { found = false; }
    };
    mx_object(path_state, mx, state)
};

struct watch:mx {
    using fn = lambda<void(bool, array &)>; // path_op

    struct state {
        bool             safe;
        bool        canceling;
        const int     polling = 1000;
        array           paths;
        states<path::option> options;
        fn           watch_fn;
        array             ops; // path_op
        int              iter;
        int           largest;
        array            exts;
        map              path_states;
    };

    mx_object(watch, mx, state);

    static watch spawn(array paths, array exts, states<path::option> options, watch::fn watch_fn);
    
    void stop();
};
}