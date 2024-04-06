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
    Test1 t1;
    t1.set_meta("test1", int(2));
    assert(t1->test1 == 2);

    path  p  = "data/test.json";
    Test1 t2 = p.read<Test1>();
    assert(t2->test1 == 2);

}
