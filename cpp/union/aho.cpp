#include <stdio.h>

class X
{
 public:
    union {
        int aa;
        int bb;
    } a;

    union {
        int cc;
        int dd;
    } b;

    X(int v, int w ){
        a.aa = v;
        b.cc = w;
        a.bb = w;
    }
};

int main()
{
    X x(1,2);

    printf("%d\n", x.a );
    printf("%d\n", x.b );    
    
}
