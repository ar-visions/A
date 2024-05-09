#ifdef __cplusplus
#pragma once

#include <mx/mx.hpp>
#include <condition_variable>
#include <future>

namespace ion {

/// executables are different types, and require async
struct exec:str {
    exec(str s) : str(s) { }
    ion::path path() {
        return data;
    }
};

using FnFuture = lambda<void(M)>;

/// 
struct Completer:A {
    M               vdata;
    mutex           mtx;
    bool            completed;
    array           l_success;
    array           l_failure;

    Completer() : A(typeof(Completer)) { }

    Completer(lambda<void(M)>& fn_success,
              lambda<void(M)>& fn_failure) : Completer() {
        
        fn_success = [this](M arg) {
            completed = true;
            for (FnFuture &fn: l_success)
                fn(arg);
        };
        
        fn_failure = [this](M arg) {
            completed = true;
            for (FnFuture &fn: l_failure)
                fn(arg);
        };
    }

    operator bool() { return l_success->len() > 0; }
};

struct completer {
    A_decl(completer, Completer)
};

/// how do you create a future?  you give it completer memory
struct Future:A {
    completer cd;

    Future() : A(typeof(Future)) { }
    Future(completer cd) : A(typeof(Future)), cd(cd) { }
    
    int sync() {
        cd->mtx.lock();
        mutex mtx;
        if (!cd->completed) {
            mtx.lock();
            void *v_mtx = &mtx;
            FnFuture fn = [v_mtx] (M arg) { ((mutex*)v_mtx)->unlock(); };
            cd->l_success->push(fn);
            cd->l_failure->push(fn);
        }
        cd->mtx.unlock();
        mtx.lock();
        return 0;
    };

    ///
    Future& then(const FnFuture& fn) {
        cd->mtx.lock();
        cd->l_success += fn;
        cd->mtx.unlock();
        return *this;
    }

    ///
    Future& except(const FnFuture& fn) {
        cd->mtx.lock();
        cd->l_failure += fn;
        cd->mtx.unlock();
        return *this;
    }

    operator bool() { return bool(cd); }
};

struct future {
    A_decl(future, Future)
};

struct Proc;
typedef lambda<M(Proc*, int)> FnProcess;

typedef std::condition_variable ConditionV;
struct Proc:A {
protected:
    inline static bool init;
    
public:
    inline static ConditionV       cv_cleanup;
    inline static std::thread      th_manager;
    inline static mutex            mtx_global;
    inline static mutex            mtx_list;
    inline static list             procs;
    inline static int              exit_code = 0;

    mutex                      mtx_self; /// output the memory address of this mtx.
    lambda<void(M)>            on_done;  /// not the same prototype as FnFuture, just as a slight distinguisher we dont need to do a needless non-conversion copy
    lambda<void(M)>            on_fail;
    int                        count;
    FnProcess				   fn;
    array                      results;
    vector<std::thread>        threads;
    int                        done      = 0; /// 
    bool                       failure   = false;
    bool                       join      = false;
    bool                       stop      = false; /// up to the user to implement this effectively.  any given service isnt going to stop without first checking

    Proc() : A(typeof(Proc)), fn(fn), count(count) { }

    Proc(int count, FnProcess fn) : A(typeof(Proc)), fn(fn), count(count), results(array(count)), threads(count) {
        if(!init) {
            init       = true;
            th_manager = std::thread(manager);
        }
    }

    static void manager();
    static void  thread(Proc *data, int w);
    void            run(int w);
    bool        joining() const { return join; }

    operator bool() { return count > 0; }
};

struct proc {
    A_decl(proc, Proc)
};

/// async is out of sync with the other objects.
struct async {
    ///
    struct delegation {
        proc            ps;
        M               results;
        mutex           mtx; /// could be copy ctr
    } d;

    async();
    
    /// n processes
    async(int, FnProcess);

    /// path to execute, we dont host a bunch of environment vars. ar.
    async(exec);
    
    /// singleton process
    async(FnProcess);

    async& operator=(const async &c);

    array sync(bool force_stop = false);

    /// await all async processes to complete
    static int await_all();

    /// return future for this async
    operator future();
};

/// sync just performs sync on construction
struct sync:async {
    sync(int count, FnProcess fn) : async(count, fn) {
        async::sync();
    }
    sync(exec p) : async(p) {
        async::sync();
    }

    /// call Array<S> src -> T conversion
    operator array() { return async::sync(); }
    operator   int() { return async::sync(); }
};
}
#endif