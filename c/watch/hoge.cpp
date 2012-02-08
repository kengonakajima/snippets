#include <stdio.h>

class X
{
 public:
    int xxx;
 X() : xxx(0) {
    }
};

int main()
{
    X *x = new X();
    
       
    printf("hoge\n");

    x->xxx = 1;

    printf("fuga\n");
}
