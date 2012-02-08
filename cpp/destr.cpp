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

int main()
{
    B *b = new B();

    delete b;
}
