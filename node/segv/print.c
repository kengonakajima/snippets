#include <stdio.h>
int main() {
    for(int i=0;i<100;i++){
        printf("hoge:%d\n",i);
    }
    int *x=0;
    *x=0;
    return 0;
}
