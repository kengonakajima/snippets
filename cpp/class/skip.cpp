#include <stdio.h>

class A {
public:
    virtual void hoge() {
        printf("aaaaa\n");
    }
};

class B : public A {
public:
    B(){}
};

class C : public B {
public:
    C(){}
    virtual void hoge() {
        printf("cccccccccc\n");        
    }
};

    
int main() {
    C *c = new C();
    c->hoge();

    A *a = c;
    a->hoge();
    
}
     
