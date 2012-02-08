/*


  TCP_NODELAY のテスト

  1 client , 1 server


  Send packets from client and server echoes it to client.


 */
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int set_nodelay(int s )
{
    int flag = 1;
    int result = setsockopt( s, IPPROTO_TCP, TCP_NODELAY,
                             (char*)&flag, sizeof( int));
    if( result < 0 ){
        fprintf( stderr , "tcp_nodelay set error\n" );
        return -1;
    }
    fprintf( stderr , "NODELAY\n");    
    return 0;
}

int do_nodelay=0;
int main( int argc, char **argv )
{
    int s , ns , clilen;
    struct sockaddr_in sin , cliaddr;

    if( argc != 1 ) do_nodelay = 1;
        
    
    s = socket( AF_INET, SOCK_STREAM, 0 );
    if( s<0){
        fprintf(stderr,"socket() error\n" );
        return 1;
    }

    memset( &sin, 0 , sizeof( sin ));
    sin.sin_family = AF_INET;
    sin.sin_port = htons( 9999 );
    sin.sin_addr.s_addr = htonl( INADDR_ANY );

    if( bind( s, (struct sockaddr*) &sin, sizeof( sin )) <0){
        fprintf( stderr , "bind() error\n" );
        return 1;
    }

    listen( s, 5 );
    fprintf( stderr , "listening.......\n" );
    clilen = sizeof( cliaddr );
    ns = accept( s , (struct sockaddr*) &cliaddr, &clilen );
    if( ns < 0 ){
        fprintf( stderr, "accept() error\n ");
        return 1;
    }
 
    if( do_nodelay && ( set_nodelay( ns ) < 0 ) ){
        fprintf( stderr , "nodelay error\n" );
        return 1;
    }
    
    /* read/write loop */
    while(1)
    {
        char buf[100];
        int l;

        l = read( ns, buf, sizeof(buf));
        if( l <= 0)break;

        if( write( ns, buf, l )<0)break;
    }
    close( ns );
    fprintf( stderr , "done\n" );
    return 0;
}

