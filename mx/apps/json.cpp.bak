#include <mx/mx.hpp>

using namespace ion;

static type_t t = typeof(Array<int>);

struct Test2:mx {
    struct M {
        int v2 = 1;
        Array<int> a2 = Array<int> { 1, 2 };
        properties meta() {
            return properties {
                prop { "v2", v2 },
                prop { "a2", a2 }
            };
        }
    };
    mx_basic(Test2);
};

struct Test1:mx {
    struct M {
        int         test1;
        Array<int>  test_array = Array<int> { 1, 2 };
        Test2       t2;
        properties meta() {
            return properties {
                prop { "test1", test1 },
                prop { "test_array", test_array },
                prop { "t2", t2 }
            };
        }
    };
    mx_basic(Test1);
};

int main(int argc, char *argv[]) {
    Test1 t1;
    assert(t1->test1 == 0);
    t1.set_meta("test1", int(2));
    assert(t1->test1 == 2);
    assert(t1->test_array.count() == 2);
    assert(t1->test_array[0] == 1);
    assert(t1->test_array[1] == 2);
    assert(t1->t2->a2 == 1);

    path  p  = "data/test.json";
    Test1 t2 = p.read<Test1>();
    printf("test_array addr 2: %p\n", &t2->test_array);
    assert(t2->test_array.count() == 2);
    assert(t2->test_array[0] == 10);
    assert(t2->test_array[1] == 22);
    assert(t2->test1 == 2);

    assert(t2->t2->v2 == 2);
    assert(t2->t2->a2[0] == 10);
    assert(t2->t2->a2[1] == 22);

    int test = 0;

}
