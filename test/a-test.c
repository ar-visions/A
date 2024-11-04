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

    int sz1 = sizeof(struct string);
    int sz2 = sizeof(struct vector);
    AType str_type = typeid(string);
    string s1 = new(string, alloc, 64);
    print("a-test");

    /// convert string to vector string
    /// cs macro may use data(v)
    string s = new(string, chars, "test1-");
    append(s, "test2");
    print("str = %o", s);

    string m = mid(s, 1, len(s) - 2);
    
    A header = A_header(s);
    print("string2: %i, refs: %i, data: %p", s->alloc, header->refs, header->data);

    concat(s, "this is a test", 14); 
    return 0;
}
