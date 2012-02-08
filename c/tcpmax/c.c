#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>


static double
time_diff(struct timeval subtrahend,
          struct timeval subtractor)
{
    return( (subtrahend.tv_sec - subtractor.tv_sec)
            + (subtrahend.tv_usec
            - subtractor.tv_usec  ) / (double)1E6 );
}


int wsize;
int sockfd;
struct sockaddr_in svaddr;
char buf[1024*1024];
int
main(int argc, char **argv )
{
    sockfd = socket( AF_INET, SOCK_STREAM , 0 );

    memset( &svaddr, 0 , sizeof( svaddr ));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons( 10000 );
    if( inet_aton( argv[1],&svaddr.sin_addr )==0){
        fprintf( stderr , "inet_aton fail\n" );
        return 1;
    }

    if( connect( sockfd, (struct sockaddr*) &svaddr , sizeof( svaddr ))<0){
        fprintf( stderr,  "fuck\n" );
        return 1;
    }

    /* write to the socket in max speed */
    wsize = 1;
    while(1){
        int r;
        fd_set fds;
        struct timeval st,et ,tmo;
        FD_ZERO( &fds );
        FD_SET( sockfd, &fds );
        tmo.tv_sec= tmo.tv_usec = 0;
        select( sockfd + 1, (fd_set*)NULL,&fds,(fd_set*)NULL,&tmo );
        if( FD_ISSET( sockfd, &fds ) ){
            gettimeofday( &st, NULL );
            r = write( sockfd, buf, 14096);
            gettimeofday( &et, NULL );
            fprintf( stderr , "%d:%f\n", r, time_diff( et,st ));
        }
        if( wsize == sizeof(buf ))break;
    }

    close(sockfd);

    return 0;
}
