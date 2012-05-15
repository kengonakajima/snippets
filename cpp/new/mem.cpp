#include <iostream>
#include "a.h"

void *operator new(size_t size){
    printf("new:sz:%u\n",(unsigned int)size);
    return malloc(size);
}
void operator delete(void *pv){
    printf("freeing %p\n",pv);
    free(pv);
}
