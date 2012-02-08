#include <iostream>

class Y
{
 public:
    int yy;
};

class X : public Y
{
 public:
    int a,p,b,c;
    
};

int main()
{
    X *x;

    std::cerr << "aho:" << (int)( (char*)(& x->c)  - (char*)(& x->yy) ) << std::endl;
    
}
