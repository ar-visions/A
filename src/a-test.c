#include <A>

int main(int n_args, char* v_args[]) {
    string   key = ctr(string, cstr, "a-string", -1);
    i64*     val = A_i64(100);
    hashmap    m = ctr(hashmap, sz, 8);
    call(m, set, key, val);

    print("hashmap = %o, int = %i, char = %i, float = %.2f, hashmap-pointer = %p\n", m, 1024, 512, 0.5f, m);
    return 0;
}