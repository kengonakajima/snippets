#include <stdio.h>
#include <stdlib.h>


int main()
{
    double t=0;
    int i;
    for(i=0;i<100000000;i++){
        t += rand();
    }
    printf("%f\n", t );
}
