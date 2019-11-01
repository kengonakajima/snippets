#include <stdio.h>
#include <stdlib.h>

#define N (8LL * 1024LL * 1024LL * 1024LL )
#define L 7
int main() {
    char *buf = malloc(N*L);
    if(!buf) {
        fprintf(stderr,"malloc failed");
        return 1;
    }
    for(int j=0;j<1000;j++) {
        for(int i=0;i<1000000;i++) {
            long long ind = (long long)(rand()) * L;
            buf[ind]++;
        }
        fprintf(stderr,"j:%d\n",j);
    }
}
