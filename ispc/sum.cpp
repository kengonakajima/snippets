#include <stdio.h>
#include <stdlib.h>




void doit(int n ) {
    float *vin = (float*)malloc( sizeof(float)*n);
    float tot=0;
    for(int i=0;i<n;i++){
        vin[i] = (float)i;
    }

    for(int i=0;i<n;i++){
        tot += vin[i];
    }
    printf("tot:%f\n", tot );
    free(vin);
}


          
int main() {
    for(int i=0;i<10;i++){
        doit(1024*1024*32);
    }
    return 0;
}
