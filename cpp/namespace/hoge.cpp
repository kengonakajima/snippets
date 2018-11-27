#include <stdio.h>

namespace foo {
    typedef enum {
                  A0=1,
                  A1=2
    } FooEnum;
};

namespace foo {
    namespace baz {
        typedef enum {
                      B0=1,
                      B1=2
        } BazEnum;

        void bazfunc() {
            printf("bazenum: %d %d\n", B1, A1 );
        }
    }
}


int main() {
    foo::baz::bazfunc();
    return 0;
}
