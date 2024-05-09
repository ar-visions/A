#include <mx/mx.hpp>
#include <async/async.hpp>

namespace ion {

A_impl(future,    Future)
A_impl(completer, Completer)
A_impl(proc,      Proc)

void Proc::manager() {
    std::unique_lock<mutex> lock(mtx_list);
    for (bool quit = false; !quit;) {
        cv_cleanup.wait(lock);
        bool cycle    = false;
        do {
            cycle     = false;
            num index = 0;
            ///
            for (Proc &ps: procs) {
                ps.mtx_self.lock();
                int len = ps.threads->len();
                if (ps.done == len) {
                    ps.mtx_self.unlock();
                    lock.unlock();

                    /// join threads
                    for (int i = 0; i < len; i++)
                        ps.threads[i].join();
                    
                    /// manage process
                    ps.mtx_self.lock();
                    ps.join = true;

                    /// 
                    procs->remove(index); /// remove -1 should return the one on the end, if it exists; this is a bool result not some integer of index to treat as.
                    (procs->len() == 0) ?
                        (quit = true) : (cycle = true);
                    lock.lock();
                    ps.mtx_self.unlock();
                    break;
                }
                index++;
                ps.mtx_self.unlock();
            }
        } while (cycle);
        /// dont unlock here because of the implicit behaviour of condition_variable
    }
    lock.unlock();
}

void Proc::thread(Proc* data, int w) {
    data->run(w);
}

void Proc::run(int w) {
    /// run (fn) the work (p) on this thread (i)
    M r = fn(this, w);
    mtx_self.lock();

    failure |= !r;
    results[w] = r;
    
    /// wait for completion of one (we coudl combine c check inside but not willing to stress test that atm)
    mtx_global.lock();
    bool im_last = (++done >= count);
    mtx_global.unlock();

    /// if all complete, notify condition var after calling completer/failure methods
    if (im_last) {
        if (on_done) {
            if (failure)
                on_fail(results);
            else
                on_done(results);
        }
        mtx_global.lock();
        cv_cleanup.notify_all();
        mtx_global.unlock();
    }
    mtx_self.unlock();

    /// wait for job set to complete
    for (; done != count;)
        yield();
}

async::async() { }

async &async::operator=(const async &b) {
    d.ps = b.d.ps;
    return *this;
}

async::async(int count, FnProcess fn) : async() {
    std::unique_lock<std::mutex> lock(Proc::mtx_list);
    
    /// create empty results [dynamic] vector [we need to couple the count into process, or perhaps bring it into async]
    d.ps     = proc { count, fn };
    Proc &ps = d.ps;
    for (int i = 0; i < count; i++)
        ps.threads->emplace(std::thread(Proc::thread, &ps, i));
    
    Proc::procs->push(&ps);
}

async::async(exec command) : async(1, [&](Proc &ps, int i) -> M {
    console.log("shell > {0}", { command });
    
    char cmd[256];
    snprintf(cmd, 256, "%s > stdout.txt", command->cs());
    int exit_code = int(std::system(command->cs()));

    if (exit_code != 0)
        std::cout << std::ifstream("stdout.txt").rdbuf();

    return exit_code;
}) { }

async::async(lambda<M(Proc*, int)> fn) : async(1, fn) {}

array async::sync(bool force_stop) {
    /// wait for join to complete, set results internal and return
    for (;;) {
        d.mtx.lock();
        d.ps->stop = force_stop;
        if (d.ps->joining()) {
            d.mtx.unlock();
            break;
        }
        d.mtx.unlock();
        yield();
    }
    return d.ps->results;
}

/// await all async processes to complete
int async::await_all() {
    for (;;) {
        Proc::mtx_global.lock();
        if (!Proc::procs->len()) {
            Proc::mtx_global.unlock();
            break;
        }
        Proc::mtx_global.unlock();
        sleep(u64(1000));
    }
    Proc::th_manager.join();
    return Proc::exit_code;
}

/// return future for this async
async::operator future() {
    lambda<void(M)>   s, f;
    completer    c = { s, f };
    assert(!d.ps->on_done);
    d.ps->on_done = [s, f](M v) {
        s(v);
    };
    d.ps->on_fail = [s, f](M v) {
        f(v);
    };
    return future(c);
}
}
