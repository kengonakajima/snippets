#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "isum_ispc.h"
using namespace ispc;


void doit(int n ) {
    int *vin = (int*)malloc( sizeof(int)*n);
    for(int i=0;i<n;i++){
        vin[i] = i;
    }
    for(int i=0;i<10;i++){    
        long long out = isum(vin, n);
        printf("tot:%lld iter:%d\n", out,i );
    }
    free(vin);
}


          
int main() {
    doit(1024*1024*32);
    return 0;
}
