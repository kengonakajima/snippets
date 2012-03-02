#include <stdio.h>

int  main()
{
    float x;
    int i;
    
    x = 60000.0f;
    for(i=0;i<10;i++){
        x = x + 0.001;
        fprintf(stderr, "x:%f\n", x );
    }
    x = 2000.0f;
    for(i=0;i<10;i++){
        x = x + 0.001;
        fprintf(stderr, "x:%f\n", x );
    }
}
