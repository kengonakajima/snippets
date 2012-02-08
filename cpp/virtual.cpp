#include <stdio.h>
class A
{
 public:
    virtual void x(){ printf("hoge\n");};
};

class B : public A
{
 public:
    virtual void x(){
        printf("aho\n");
    }
};


int main()
{
    B *b = new B();
    b->x();
    
}
