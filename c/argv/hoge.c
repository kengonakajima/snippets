#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main( int argc, char **argv ) {
    int argv0size = strlen(argv[0]); //Take note of how many chars have been allocated
     strncpy(argv[0],"main-thread-name",argv0size); //replace argv[0][0..argv0size]
     strncpy(argv[1],"pppp",5); //replace argv[0][0..argv0size]     
     sleep(100);
}
