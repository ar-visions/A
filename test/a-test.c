#include <A>

/// @brief can reference the 'struct type' as inlay, and 'type' as ptr
typedef struct user {
    string a;
} *user;

int main(int n_args, char* v_args[]) {
    A_start();
    /*
    AF pool   = A_pool(1024);
    A  fields = A_header(pool);
    /// access the A-header direct, start of allocation, behind user memory

    user    u = A_struct(user);
    u->a      = new(string, chars, "Audrey");
    map     m = new(map, hsize, 16);
    set(m, u->a, A_bool(true));
    print("m = %o", m);
    
    A_free(pool);
    */
    print("a-test");
    return 0;
}
