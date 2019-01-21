#include <stdio.h>

#if defined(__linux) || defined(__APPLE__)
int a=0;
#else
int a=1;
#endif
int main() {
    printf("%d\n",a);
}
