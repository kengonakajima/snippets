#include <stdio.h>

class A
{
 public:
    int a;

    A(){
    }

    int to_i(){
        return 1;
    }
    
};

class B : public A
{
 public:
    int b;
    B(){
    }
    int to_i(){
        return 1 + A::to_i();
    }
    
};


int main()
{
    A *pa = new A();
    B *pb = new B();

    printf("%d\n", pb->
           to_i());

}
