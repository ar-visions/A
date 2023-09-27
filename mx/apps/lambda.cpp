#include <mx/mx.hpp>

using namespace ion;

enums(EnumTest, first, 
       first, second);

int main(int argc, char *argv[]) {
    usleep(10000);
    lambda<void(int)> fn_test = [](EnumTest arg) {
        str arg_str = arg;
        printf("arg_str = %s\n", arg_str.cs());
    };

    invoke(fn_test, { "second" });

    /*
    
    use case for introspective lambda:
        using x and y args together in css

    using _ or - as same char now

    lambdas embedded in service tree make sense

    */

    return 0;
}