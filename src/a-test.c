#include <A>

int main(int n_args, char* v_args[]) {

    AR pool = alloc(AR); // alloc and free are how you use these without auto-release
    AR pool2 = alloc(AR);
    string   key = ctr(string, cstr, "a-string", -1);
    A fields = A_fields(key);
    i64*     val = A_i64(100);
    hashmap    m = ctr(hashmap, sz, 8);
    call(m, set, key, val);

    /// test the map, iterate and such...
    print("hashmap = %o, int = %i, char = %i, float = %.2f, hashmap-pointer = %p\n",
        m, 1024, 512, 0.5f, m);
    i64*    val2 = idx_1(m, A, key);
    A_free(pool2);
    A_free(pool);
    return 0;
}
