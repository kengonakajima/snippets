#include <stdio.h>

#define XXX 0x20203040L

#define MAJ_V (XXX & 0x10000000L)
#define MIN_V (XXX & 0x00100000L)
#define PATCH_V (XXX & 0x00001000L)

#if MAJ_V == 0x10000000L
int main() {
    printf("match\n");
}
#else
int main() {
    printf("mismatch\n");    
}
#endif

