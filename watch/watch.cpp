#include <watch/watch.hpp>

namespace ion {

watch watch::spawn(array paths, array exts, states<path::option> options, watch::fn watch_fn) {
    watch w;
    watch::state &s = *w.data;

    s.paths       = paths;
    s.watch_fn    = watch_fn;
    s.exts        = exts;
    s.options     = options;
    state*     ps = w.data;
    ///
    async { size_t(1), [ps, options] (runtime *p, int index) -> var { /// give w a ref and copy into lambda
        state& s = *ps; /// was being copied!
        while (!s.canceling) {
            s.ops = array(typeof(path_op), size_t(32 + s.largest * 2));
            
            /// flag for deletion
            for (field &f: s.path_states.fields()) {
                path_state &ps = f.value.mem->ref<path_state>();
                ps->found      = false;
            }
            
            /// populate modified and created
            size_t path_index = 0;
            for (path &res:s.paths.elements<path>()) {
                res.resources(s.exts, options, [&](path p) {
                    if (!p.exists())
                        return;
                    str       key = p;
                    i64     mtime = p.modified_at();
                    ///
                    if (s.path_states->count(key) == 0) {
                        path_op op;
                        op->path            = p;
                        op->path_index      = path_index;
                        op->op              = (s.iter == 0) ? path::op::none : path::op::created;
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
                        auto &ps            = s.path_states[key].ref<path_state::state>();
                        ps.found            = true;
                        if (ps.modified != mtime) {
                            path_op op;
                            op->path        = p;
                            op->path_index  = path_index;
                            op->op          = path::op::modified;
                            ///
                            s.ops          += op;
                            ps.modified     = mtime;
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
                for (auto &[k,v]: s.path_states.fields()) {
                    path_state &ps = v.mem->ref<path_state>();
                    if (!ps->found) {
                        path_op op;
                        op->path        = ps->res;
                        op->path_index  = ps->path_index;
                        op->op          = path::op::deleted;
                        ///
                        s.ops += op;
                        s.path_states->remove(k);
                        cont = true;
                        break;
                    }
                }
            } while (cont);
            
            /// notify user if ops
            size_t sz = s.ops.len();
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
    while (s.iter == 0) {
        usleep(1000);
        if (s.safe)
            break;
    }
    return w;
}

void watch::stop() {
    if (!data->safe) {
        data->canceling = true;
        while (!data->safe) { usleep(1); }
    }
}
}