#include <stdio.h>
#include <stdlib.h>
#include "sum_ispc.h"
using namespace ispc;

/*
float fsum( float *vin, int n ) {
    float tot=0;
    for(int i=0;i<n;i++){
        tot += vin[i];
    }
    return tot;    
}
*/

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


//#include <stdio.h>
//#include <stdlib.h>

// Include the header file that the ispc compiler generates
//#include "simple_ispc.h"
//using namespace ispc;

/*
  int main() {
    float vin[16], vout[16];

    // Initialize input buffer
    for (int i = 0; i < 16; ++i)
        vin[i] = (float)i;

    // Call simple() function from simple.ispc file
    simple(vin, vout, 16);

    // Print results
    for (int i = 0; i < 16; ++i)
        printf("%d: simple(%f) = %f\n", i, vin[i], vout[i]);

    return 0;
}
*/
