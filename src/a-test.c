#include <A>

int main(int n_args, char* v_args[]) {
    A_start();
    AF pool = A_pool(1024);
    A fields = A_fields(pool);

    string audrey;
    audrey = new(string, chars, "Audrey");
    
    A_free(pool);
    return 0;
}
