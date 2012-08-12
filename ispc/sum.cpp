#include <stdio.h>
#include <stdlib.h>


float fsum( float *vin, int n ) {
    float tot=0;
    for(int i=0;i<n;i++){
        tot += vin[i];
    }
    return tot;    
}


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
