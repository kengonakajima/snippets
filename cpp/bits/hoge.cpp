#include <stdio.h>

int main() {
    struct Cell {
        unsigned char lt:4;
        unsigned char sunlt:4;
    };

    struct Cell c;
    c.lt=3;
    c.sunlt=9;
    printf("hoge:%d,%d sz:%lu\n",c.lt,c.sunlt,sizeof(c));
}
