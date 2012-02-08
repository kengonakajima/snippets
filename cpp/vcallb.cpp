#include <stdio.h>

class A
{
 public:
    virtual void bcast(){
        printf("bcast A\n");
    };
};

class B : public A
{
 public:
    virtual void bcast(){
        printf("bcast B\n");
    }
};

class C : public A
{
 public:
    virtual void bcast(){
        printf("bcast C\n");        
    }
};

int main()
{
    A *aho[] = { new A(), new B(), new C() };

    aho[0]->bcast();
    aho[1]->bcast();
    aho[2]->bcast();
}
