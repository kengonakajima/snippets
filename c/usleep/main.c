#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    while(1) {
        fprintf(stderr,"time:%u\n", (unsigned int) time(NULL));
        for(int i=0;i<10;i++) {
            usleep( 100 * 1000 );
        }
    }
}
