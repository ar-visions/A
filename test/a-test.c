#include <import>

/// @brief can reference the 'struct type' as inlay, and 'type' as ptr
typedef struct user {
    string a;
} *user;





int main(int n_args, char* v_args[]) {
    A_start(n_args, v_args);
    auto_free();

    AType map_t    = &map_type; // 1
    AType string_t = &string_type; // 223
    AType item_t   = &item_type; // 0
    int test2 = 2;
    test2    += 2;
    test2    += 2;
    test2    += 2;
    test2    += 2;
    test2    += 2;

    for (int base = A_alloc_count(), count = base;
            verify(base == count, "too many alloc");
            count = A_alloc_count()) {

        string k = string("key");
        object b = A_bool(true);
        map    m = map(16);
        
        A      m_header = A_header(m);
        A      b_header = A_header(b);
        A      k_header = A_header(k);
        set(m, k, b);
        auto_free();
        count = A_alloc_count();
        count = A_alloc_count();
    }

    print("A_start ran");

    /// lets init an object, and then enumerate which fields were given by checking afterwards.
    /// this method will let us know primitives and structs; thats great!
    /// no extra storage on the instance, either.  uses the same 'f' fields data; its used for this purpose!
    object       obj = null;
    subprocedure sub = subprocedure(
        target, null, // <-- null value explicitly and it passes required.  so thats useful for design members
        addr,   main);

    AType sub_type = isa(sub);

    int sz1 = sizeof(struct _string);
    int sz2 = sizeof(struct _vector);
    AType str_type = typeid(string);
    string s1 = new(string, alloc, 64);
    print("a-test");

    /// convert string to vector string
    /// cs macro may use data(v)
    string s = new(string, chars, "test1-");
    append(s, "test2");
    print("str = %o", s);

    string m = mid(s, 1, len(s) - 2);
    
    A header = A_header((object)s);
    print("string2: %i, refs: %i, data: %p", s->alloc, header->refs, header->data);

    return 0;
}
