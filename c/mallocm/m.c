#include <stdio.h>
#include <stdlib.h>


int main(){
    int i;
    for(i=0;i<10000000;i++){
        char *m = malloc(100);
        free(m);
    }
}
