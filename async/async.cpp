#include <mx/mx.hpp>
#include <async/async.hpp>

namespace ion {
async::async() { }

///
async::async(size_t count, FnProcess fn) : async() {
    ///
    std::unique_lock<std::mutex> lock(process::mtx_list);
    
    /// create empty results [dynamic] vector [we need to couple the count into process, or perhaps bring it into async]
    d.proc       = process { count, fn };
    runtime  *ps = d.proc.data;
    ps->handle   = d.proc.mem->grab();
    
    /// measure d.proc.rt address here
    ps->threads = new std::vector<std::thread>();
    ps->threads->reserve(count);
    for (size_t i = 0; i < count; i++)
        ps->threads->emplace_back(std::thread(process::run, ps, int(i)));
    ///
    process::procs->push(ps);
}

async::async(exec command) : async(1, [&](runtime *proc, int i) -> mx {
    console.log("shell > {0}", { command });
    
    char cmd[256];
    sprintf(cmd, "%s > stdout.txt", command.cs());
    int exit_code = int(std::system(command.cs()));

    if (exit_code != 0)
        std::cout << std::ifstream("stdout.txt").rdbuf();

    return exit_code;
}) { }

async::async(lambda<mx(runtime *, int)> fn) : async(1, fn) {}

array<mx> async::sync() {
    /// wait for join to complete, set results internal and return
    for (;;) {
        d.mtx.lock();
        if (d.proc.joining()) {
            d.mtx.unlock();
            break;
        }
        d.mtx.unlock();
        yield();
    }
    return d.proc->results;
}

/// await all async processes to complete
int async::await_all() {
    for (;;) {
        process::mtx_global.lock();
        if (!process::procs->len()) {
            process::mtx_global.unlock();
            break;
        }
        process::mtx_global.unlock();
        sleep(u64(1000));
    }
    process::th_manager.join();
    return process::exit_code;
}

/// return future for this async
async::operator future() {
    lambda<void(mx)>   s, f;
    completer    c = { s, f };
    assert( d.proc);
    assert(!d.proc->on_done);
    d.proc->on_done = [s, f](mx v) {
        s(v);
    };
    d.proc->on_fail = [s, f](mx v) {
        f(v);
    };
    return future(c);
}
}
