#include <stdio.h>

class A
{
 public:
    virtual void hoge(){
        printf("adsf\n");
    }
    
};

class B: public A
{
 public:
    virtual void hoge(){
        printf(" hage\n" );
    }
};
class C: public B
{
 public:
    //    virtual void hoge()=0;
    
};

int main(){
    A *a = new A();
    B *b = new B();
    C *c = new C();
    A *aa = c;
    b->hoge();
    b->A::hoge();
    c->hoge();
    aa->hoge();
}
