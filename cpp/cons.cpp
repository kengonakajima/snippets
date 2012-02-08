#include <iostream>

class A
{
 public:
    int ma;
    A(){}
    A( int a ){
        std::cerr << "Acons" << std::endl;
        ma = a;
    }
};

class B : public A
{
 public:
    int mb;
    B(){}
 B(int b) : A(b){
        std::cerr << "Bcons" << std::endl;        
        mb = b;
    }
};


int main()
{
    B b(10);
    std::cerr << "ma:" << b.ma << std::endl;
}
