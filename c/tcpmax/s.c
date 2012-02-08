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


int sockfd , newsockfd;
struct sockaddr_in bindaddr;
struct sockaddr_in cliaddr;
int clilen;
char buf[1024 * 1024];
double total_read=0.0f;
double total_syscall_time=0.0f;
double total_syscall_count=0.0f;
time_t start_time;

int
main(int argc, char **argv )
{

    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    memset( &bindaddr, 0 , sizeof( bindaddr ) );

    bindaddr.sin_family = AF_INET;
    bindaddr.sin_port = htons( 10000 );
    bindaddr.sin_addr.s_addr = htonl( INADDR_ANY );

    if( bind( sockfd, (struct sockaddr *) &bindaddr,
              sizeof( bindaddr ))<0){
        fprintf( stderr, "%s\n", strerror(errno ));
        return 1;
    }

    listen( sockfd, 5 );

    clilen = sizeof( cliaddr );
    newsockfd = accept( sockfd, (struct sockaddr *)&cliaddr, &clilen );
    if( newsockfd < 0 ){
        fprintf( stderr, "accept: %s\n", strerror( errno ));
        return 1;

    }


    fprintf( stderr ,"%d\n", newsockfd );

    start_time = time(NULL);
    
    while(1){
        struct timeval st, et,timeout;
        static time_t ttt;
        int r;
        fd_set fds;
        

        timeout.tv_usec = 0;
        timeout.tv_sec = 0;
        FD_ZERO( &fds );
        FD_SET( newsockfd, &fds );
        select( newsockfd+1, &fds, (fd_set*)NULL,(fd_set*)NULL,
                    &timeout );

        if( FD_ISSET( newsockfd, &fds ) ){
            double d;
            gettimeofday( &st, NULL );
            r = read( newsockfd , buf, sizeof( buf ));
            gettimeofday( &et, NULL );
            d = time_diff( et,st );
            total_syscall_time += d;
            total_syscall_count += 1.0;
            total_read += r;

            if( ttt != time(NULL )){
                ttt = time(NULL);
                fprintf( stderr,
                         "SPEED %10.0f b/s SYSAV:%0.6f \n",
                         total_read / ( time(NULL)-start_time ) ,
                         total_syscall_time / total_syscall_count );
            }
        }
#if 0
        FD_SET( newsockfd, &fds );
        select( newsockfd+1, (fd_set*)NULL, &fds, (fd_set*)NULL,
                    &timeout );
        if( FD_ISSET( newsockfd, &fds ) ){
            
        }
#endif
    }

    
    close( newsockfd );
    return 0;
}
