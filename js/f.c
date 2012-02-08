#include <stdio.h>
#include <stdlib.h>

int f(int x)
{
    return(x + 2 );
}
int g(int x)
{
    int *p = malloc(4);
    *p = x;
    free(p);
    return x;
}

int main()
{
    double t=0;
    int i;
    for(i=0;i<100000000;i++){
        t += g(i);
    }
    printf("%f\n", t );
}
