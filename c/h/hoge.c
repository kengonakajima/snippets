#include <stdio.h>


    unsigned int hash(unsigned int x) {
        x = ((x >> 16) ^ x) * 0x119de1f3;
        x = ((x >> 16) ^ x) * 0x119de1f3;
        x = (x >> 16) ^ x;
        return x;
    }
int n[1024];

int main() {
    for(int i=0;i<100000000;i++) {
        unsigned int h=hash(i);
        n[h%1024]++;
    }
    for(int i=0;i<1024;i++) {
        printf("%d %d\n", i, n[i]);
    }
}
