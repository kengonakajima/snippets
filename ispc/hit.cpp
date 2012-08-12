#include <stdio.h>

static const int N=1024*1024;


float vin[N];
          
int main() {
    float tot=0;
    for(int i=0;i<N;i++){
        vin[i] = (float)i;
    }

    for(int i=0;i<N;i++){
        tot += vin[i];
    }
    printf("tot:%f\n", tot );
    return 0;    
}
