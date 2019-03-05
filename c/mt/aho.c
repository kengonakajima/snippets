#include <stdio.h>
#include "MT.h"

int main(void){
    int i;
    init_genrand(10);
    for(i=0;i<100;i++){ printf("%ld\n",genrand_int32()); }
}
