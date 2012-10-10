#include "stdio.h"

void hoge( int a[10]) {
    printf("%d\n",a[9]);
}

int main() {
    int aho[5];
    hoge(aho);
}

