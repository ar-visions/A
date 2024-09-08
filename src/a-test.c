#include <A>

int main(int n_args, char* v_args[]) {
    printf("1\n");
    AF pool  = A_pool(1024); // alloc and free are how you use these without auto-release

    map imap = new(map);
    printf("2\n");
    fflush(stdout);
    str("key");
    printf("22\n");
    fflush(stdout);
    A_i32(1);
    printf("24\n");
    fflush(stdout);
    A_i32(1);
    printf("28\n");
    fflush(stdout);
    M(imap, set, str("key"), A_i32(1));
    item e;
    printf("3\n");
    fflush(stdout);
    print("enumerating?");
    fflush(stdout);
    printf("4\n");
    fflush(stdout);
    map_item mi   = (map_item)imap->refs->first->val;
    i32*     test = mi->val;
    int test2 = 1;

    /// in clang llvm you dont have to emit stuff like this just to do what you want (and hope it gets O2'd)
    if (imap && cast(imap, bool)) for (item _i = imap->refs->first, e = null; _i; _i = _i->next) \
        for (map_item e = (map_item)_i->val, e0 = e; e == e0; e0++) {
            print("key = %o, val = %o", e->key, e->val);
        }

    /// silver is about producing the operations you visually see
    /// its also about molding the design time
    enumerate(imap, e) {
        print("key = %o, val = %o", e->key, e->val);
    }

    printf("4\n");
    AF pool2 = A_pool(1024);

    string   key = ctr(string, cstr, "a-string", -1);
    A     fields = A_fields(key);
    i64*     val = A_i64(100);
    hashmap    m = ctr(hashmap, sz, 8);
    M(m, set, key, val);

    print("hashmap ptr = %p\n", m);
    print("hashmap cereal = %o\n", m);

    /// test the map, iterate and such...
    print("hashmap = %o, int = %i, char = %i, float = %.2f, hashmap-pointer = %p\n",
        m, 1024, 512, 0.5f, m);

    i64*    val2 = idx_1(m, A, key);
    A_free(pool2);
    A_free(pool);
    return 0;
}
