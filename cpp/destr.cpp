#include <stdio.h>

class A
{
 public:
    ~A(){printf("a\n");}
        
};

class B : public A
{
 public:
    ~B(){printf("b\n");};
};

class C
{
public:
    virtual ~C(){printf("c\n");}
};
class D : public C
{
public:
     ~D(){printf("d\n"); }
};

int main()
{
    B *b = new B();

    delete b;

    B *b2 = new B();
    A *a = (A*)b2;
    delete a;

    D *d = new D();
    C *c = (C*) d;
    delete c;
}
