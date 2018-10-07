#include <stdio.h>
void hoge() {
    printf("crashing!");
    fflush(stdout);
    int*x=0;
    *x=4;
}
int main() {
    hoge();
}
