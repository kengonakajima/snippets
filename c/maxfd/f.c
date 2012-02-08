#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int
main( int argc, char **argv )
{

    int i,fd;

    for(i=0;i<atoi(argv[1]);i++){

        fd = open( "f.c" , O_RDONLY );
        if( fd < 0 ){
            fprintf(stderr,strerror(errno));
            break;
        }
        else fprintf( stderr,"opened %d\n", fd );
    }
    fprintf( stderr, "fin\n" );

    return 0;
}

        
