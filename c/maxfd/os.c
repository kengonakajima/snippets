#include <unistd.h>


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int
main( int argc, char **argv )
{

    int i,fd;

    for(i=0;i<atoi(argv[1]);i++){

        fd = socket( AF_INET, SOCK_STREAM, 0 );
        if( fd < 0 ){
            fprintf(stderr,strerror(errno));
            break;
        }
        else fprintf( stderr,"opened %d\n", fd );
    }
    fprintf( stderr, "fin\n" );

    sleep(100);
    
    return 0;
}

        

