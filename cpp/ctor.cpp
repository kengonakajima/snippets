#include "stdio.h"

class A
{
 public:
    int aaa;
    
    A(){
        delete this;
        return;
    }
};


int main(){
    A *a = new A();

    printf("%p\n", a );
}


