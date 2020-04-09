#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    size_t sz=1024*1024*16;
    char *p=(char*)malloc(sz);
    memset(p,0,sz);
    int i=0;
    while(1) {
        fprintf(stderr,".");
        sleep(1);
        i++;
        if(i==10) {
            free(p);
        }
    }
    return 0;
}
