#include <stdio.h>
int main() {
    double d = 1;
    char *p = (char*)&d;
    fwrite(p,1,8,stdout);
    return 0;
}
