#include <stdio.h>
int main() {
    int a[3]={1,2,3};
    int b[3];
    *b=*a;
    printf("%d %d %d",b[0],b[1],b[2]);
}
