#include <iostream>
#include <assert.h>

class A
{
    int aho[10000];
 public:
 A()  {
        std::cerr << ".";
    }
    void scan(){
        for(int i=0;i<10000;i++){
            assert( aho[i]==0);
        }
    }
};

int main( int argc, char **argv )
{
    A a;
    a.scan();
    std::cerr << "end" << std::endl;
    return 0;
}
