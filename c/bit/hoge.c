#include <stdio.h>
int main() {
    int a= 0x10;
    int b= ~0x10;
    int c= a & b;
    int d= 0x1f & b;
    printf("%x %x %x %x\n",a,b,c,d);
}
