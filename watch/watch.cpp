#include <watch/watch.hpp>

namespace ion {

UA_impl(path_op, PathOp)
UA_impl(path_state, PathState)
UA_impl(watch, Watch)

Watch* Watch::spawn(array paths, array exts, states<Path::option> options, Watch::fn watch_fn) {
    Watch* s = new Watch();
    s->paths       = paths;
    s->watch_fn    = watch_fn;
    s->exts        = exts;
    s->options     = options;
    ///
    async { size_t(1), [ps=s, options] (Proc *p, int index) -> var { /// give w a ref and copy into lambda
        Watch& s = *ps;
        while (!s.canceling) {
            s.ops = array(32 + s.largest * 2);
            
            /// flag for deletion
            for (Field &f: s.path_states) {
                path_state  ps = f.value;
                ps->found      = false;
            }
            
            /// populate modified and created
            size_t path_index = 0;
            for (path res:s.paths) {
                res->resources(s.exts, options, [&](path p) {
                    if (!p->exists())
                        return;
                    str       key = p;
                    i64     mtime = p->modified_at();
                    ///
                    if (!s.path_states->contains(key)) {
                        path_op op;
                        op->path            = p;
                        op->path_index      = path_index;
                        op->op              = (s.iter == 0) ? Path::op::none : Path::op::created;
                        s.ops              += op;
                        ///
                        path_state st;
                        st->res             = p;
                        st->path_index      = path_index;
                        st->modified        = mtime;
                        st->found           = true;
                        ///
                        s.path_states[key]  = st;
                    } else {
                        path_state  ps      = s.path_states[key];
                        ps->found           = true;
                        if (ps->modified != mtime) {
                            path_op op;
                            op->path        = p;
                            op->path_index  = path_index;
                            op->op          = Path::op::modified;
                            ///
                            s.ops          += op;
                            ps->modified    = mtime;
                        }
                    }
                });
                path_index++;
            }
            
            /// populate deleted
            bool cont;
            do {
                cont = false;
                /// k = path, v = path_state
                for (Field &f: s.path_states) {
                    path_state ps = f.value;
                    if (!ps->found) {
                        path_op op;
                        op->path        = ps->res;
                        op->path_index  = ps->path_index;
                        op->op          = Path::op::deleted;
                        ///
                        s.ops += op;
                        s.path_states->remove(f.key);
                        cont = true;
                        break;
                    }
                }
            } while (cont);
            
            /// notify user if ops
            size_t sz = s.ops->len();
            if (sz)
                s.watch_fn(s.iter == 0, s.ops);
            
            /// track largest set for future reservation
            s.largest = math::max(s.largest, int(sz));
            
            /// wait for polling period
            s.iter++;
            usleep(1000 * s.polling);
        }
        
        ///
        s.safe = true;
        return null;
    }};
    /// wait for 1 iteration before returning
    while (s->iter == 0) {
        usleep(1000);
        if (s->safe)
            break;
    }
    return s;
}

void Watch::stop() {
    if (!safe) {
        canceling = true;
        while (!safe) { usleep(1); }
    }
}
}