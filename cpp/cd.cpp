#include <stdlib.h>

void * MALLOC( size_t s ){
    void *p = malloc(s);
    memset(p,0xcd,s);
    return p;
}

int main() {

}

    
