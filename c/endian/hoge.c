#include <stdio.h>

int main() {
    long long x = 0x0102030405060708;
    char *p = (char*)&x;
    printf("%lld\n",x);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] );

    long long y = 0x0000010203040506;
    printf("%lld\n",y);
}
