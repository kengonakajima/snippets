#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    char hoge[] = "a,b,c";
    char *p0=hoge+3;
    char *p1 = strstr(p0,",");
    printf("p1:%s",p1);
    return 0;
}
