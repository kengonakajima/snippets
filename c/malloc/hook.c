#include <stdlib.h>
#include <malloc/malloc.h>
#include <stdio.h>

void *alloc_hook (size_t size, const void *caller) {
    void *p = malloc(size);
    fprintf(stderr, "alloc. caller:%p\n", caller );
    return p;
}

              
int main(int argc, char **argv ) {
    __malloc_hook = alloc_hook;
}
