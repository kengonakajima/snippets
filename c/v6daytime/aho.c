/*
  daytime server v6
  
 */

#include <netinet/in.h>
#include <errno.h>

int
main( int argc, char **argv )
{
    int sockfd;

    sockfd= socket( AF_INET6, SOCK_STREAM, 0 );
    printf( "socket(): %d %s\n", sockfd,strerror(errno) );

    return 0;
}
