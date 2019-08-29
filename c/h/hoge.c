#include <stdio.h>
unsigned int hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = (x >> 16) ^ x;
    return x;
}
unsigned int hash2(unsigned int i) {
    unsigned long long il=i;
    il *=2654435761;
    il = il % 4294967296;
    return (unsigned int)il;
}
#define N 5000
int n[N];
int main() {
    for(unsigned int i=0;i<1000000;i++) {
        unsigned int h=hash2( random());
        n[h%N]++;
    }
    for(int i=0;i<N;i++) {
        printf("%d %d\n", i, n[i]);
    }
}
