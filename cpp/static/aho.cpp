#include <stdio.h>

class A
{
 public:
    static int aho(int x){
        printf("asdf%d\n",x);
    }
};


int main()
{
    A::aho(10);
}
