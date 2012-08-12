#include <stdio.h>
#include <stdlib.h>
#include "sum_ispc.h"
using namespace ispc;


void doit(int n ) {
    float *vin = (float*)malloc( sizeof(float)*n);
    for(int i=0;i<n;i++){
        vin[i] = (float)i;
    }
    for(int i=0;i<10;i++){    
        float out = fsum(vin, n);
        printf("tot:%f iter:%d\n", out,i );
    }
    free(vin);
}


          
int main() {
    doit(1024*1024*32);
    return 0;
}


