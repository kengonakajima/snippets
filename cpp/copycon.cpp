#include <stdio.h>

class A
{
 public:
    int ma;
    A(){}
    A( int a ){
        ma = a;
    }
    A( const A &copy ){
        *this = copy;
    }
    void operator=(const A &copy ){
    };
};

int main()
{
    A a(10);
    A aa;

    aa = a;
    printf("%d\n", aa.ma );
}
