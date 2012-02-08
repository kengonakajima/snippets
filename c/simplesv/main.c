#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

int main( )
{

    int ret;
    int sockfd = socket( AF_INET , SOCK_STREAM , 0 );
    struct sockaddr_in sockaddr , peer;

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons( 9009 );
    sockaddr.sin_addr.s_addr = INADDR_ANY;

    ret = bind( sockfd , ( struct sockaddr * ) &sockaddr , sizeof( struct sockaddr_in ) );
    if( ret < 0 ){
        fprintf( stderr , "bind error\n" );
        return 1;
    }

    ret = listen( sockfd, 5 );

    fprintf( stderr , "wait for new connection" );
    while( 1){
        int peerlen = sizeof( peer );
        int newsockfd = accept( sockfd , (struct sockaddr*)&peer , &peerlen );
        if( newsockfd < 0){
            fprintf( stderr , "accept error\n" );
            break;
        } else {
            char buf[1999];
            int readlen;
            fprintf( stderr , "new connection: fd:%d\n" , newsockfd );
            fprintf( stderr , "wait for a line\n" );
            readlen = read( newsockfd , buf , 4 );
            fprintf( stderr , "readlen:%d first 4 bytes are: %d %d %d %d\n" ,
                     buf[0] , buf[1] , buf[2] , buf[3] );
        }
    }
    fprintf( stderr , "program end\n" );
    
    
}
