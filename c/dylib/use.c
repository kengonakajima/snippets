#include <stdio.h>

int hello(void);

int main(int argc, char **argv ) {
    int x = hello();
    printf( "hello returns %d\n",x);
    return 0;
}
