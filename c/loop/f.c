#include <stdio.h>

int main() {
    float u = 1.0;
    float t = 0;
    unsigned long long i=0;
    while(1){
        i++;
        t += u;
        if( (i%1000000) <= 2 ){
            fprintf(stderr, "i:%lld t:%f\n", i, t );
        }
    }
}
