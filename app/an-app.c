#include <A>

int main(int n_args, char* v_args[]) {
    A_start();
    string a = new(string, chars, "Audrey");
    print("a = %o", a);
    return 0;
}
