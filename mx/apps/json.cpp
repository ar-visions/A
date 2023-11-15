#include <mx/mx.hpp>

struct Test3:mx {
    struct M {
        int t3_int;
        register(M)
        properties meta() {
            return {
                prop { "t3_int", t3_int }
            };
        }
    };
    mx_basic(Test3);
};

struct Test2:mx {
    struct M {
        int test2_int;
        array<Test3> test3_values;
        register(M)
        properties meta() {
            return {
                prop { "test2_int",    test2_int    },
                prop { "test3_values", test3_values }
            };
        }
    };
    mx_basic(Test2);
};

struct Test1:mx {
    struct M {
        str   str_value;
        int   int_value;
        short short_value;
        bool  bool_value;
        Test2 test2_value;
        register(M)
        properties meta() {
            return {
                prop { "str_value",   str_value   },
                prop { "int_value",   int_value   },
                prop { "short_value", short_value },
                prop { "bool_value",  bool_value  },
                prop { "test2_value", test2_value }
            };
        }
    };
    mx_basic(Test1);
};

int main(int argc, char *argv[]) {
    array<int> i_array;
    Test1 t1;

    t1.set_meta("str_value", mx(str("test")));

    str s0 = t1->str_value;

    path  p  = "test1.json";
    Test1 t11 = p.read<Test1>(); /// test2

    Test2 t2a;
    Test3 &t3v = t11->test2_value->test3_values[0];
}
