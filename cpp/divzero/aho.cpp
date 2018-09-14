#include <stdio.h>

int main() {
    for(int i=0;i<10;i++) {
        float a=1/(5-i);
        float b=-1/(5-i);
        printf("%f %f\n",a,b);
    }
}
