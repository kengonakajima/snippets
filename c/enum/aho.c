#include <stdio.h>

typedef enum {
    A = 1,
    B = 5
} EEE;

int main(){
    printf("%d\n", (int)sizeof(EEE));
}
