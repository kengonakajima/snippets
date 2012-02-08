#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>



/*
  read /dev/random as a source of random integers

  
 */
int
main( int argc, char **argv )
{
    char b[10];

    int i;
    int fd;

    fd = open( "/dev/random" , O_RDONLY );

    read( fd, b , 10 );

    for(i=0;i<sizeof(b);i++){
        printf( "%d\n", b[i] );
    }
    close( fd );

    return 0;
}
