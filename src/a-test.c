#include <A>



/// this is render, its odd but what is rendering other than an object being created
/// compose is just lower down but fulfilling the property syntax

int main(int n_args, char* v_args[]) {
    AF      pool = A_pool(1024); // alloc and free are how you use these without auto-release
    A         a1 = A_i32(1);
    A         a2 = A_i64(2);
    pair     mi2 = new(pair, key, str("a-key"), value, str("a-val"));
    /// we need to hold members with a value
    string   key = new(string, chars, "a-string");
    A     fields = A_fields(key);
    i64*   value = A_i64(100);
    hashmap    m = new(hashmap, alloc, 8);
    call(m, set, key, value);

    print("hashmap ptr = %p\n", m);
    print("hashmap cereal = %o\n", m);

    /// test the map, iterate and such...
    print("hashmap = %o, int = %i, char = %i, float = %.2f, hashmap-pointer = %p\n",
        m, 1024, 512, 0.5f, m);

    i64*    val2 = idx_1(m, A, key);
    A_free(pool);
    return 0;
}
