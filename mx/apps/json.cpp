#include <mx/mx.hpp>

using namespace ion;

struct Test1:mx {
    struct M {
        int test1;
        properties meta() {
            return properties {
                prop { "test1", test1 }
            };
        }
    };
    mx_basic(Test1);
};


int main(int argc, char *argv[]) {
    Test1 t22;
    t22.set_meta("test1", int(22));

    path  p  = "data/test.json";
    Test1 t2 = p.read<Test1>(); /// test2

    printf("t2.test1 = %d\n", t2->test1);
}
