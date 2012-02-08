#include <stdio.h>

class A
{
 public:
 A( int a ) {
        this.A(a,2);
    }

    int _a, _b;
    
    A( int a, int b ){
        _a = a;
        _b = b;
        printf("b:%d\n", _b );
    }
};

int main() {
    A *a = new A(1);

    printf("%d\n", a->_b  );
}

