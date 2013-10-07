#include <stdio.h>

class A {
public:
    int aa,ab;
    A() : aa(10), ab(11) {

    }
};

class B {
public:
    int ba,bb;
    B() : ba(20), bb(21) {
    }
};

class C : public A, B {
public:
    int ca,cb;
    C() : A(), B(), ca(30), cb(31) {
        printf("%d %d %d %d %d %d",aa, ab, ba, bb, ca,cb );
    }
};
int main() {
    C *cp = new C();

    
}
    
