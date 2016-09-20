#include <unistd.h>
#include <stdio.h>
int main() {
    while(1) {
        usleep(1000);
        fprintf(stderr,".");
    }
}
