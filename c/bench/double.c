#include <stdio.h>

int main() {
    double tot = 0;
    for(int i=0;i<1000000000;i++){
        tot += i;
    }
    printf("%f",tot);
}
