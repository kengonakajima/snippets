#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main( int argc, char **argv )
{
    char sendbuffer[100000];
    int s,i;
    int loopcounter;
    struct sockaddr_in sin;

    if( argc <= 2 ){
        fprintf(stderr ,"usage: a.out loopnum ipaddr\n" );
        return 1;
    }
    
    s = socket( AF_INET , SOCK_STREAM , 0 );

    sin.sin_family = AF_INET;
    sin.sin_port = htons( 10000 );
    sin.sin_addr.s_addr = inet_addr( argv[2] );

    if( connect( s, ( struct sockaddr* )&sin , sizeof( sin )) < 0 ){
        fprintf( stderr,  "cannot connect to server\n" );
        return 1;
    }


    
    loopcounter = atoi( argv[1] );

    for( i = 0; i < loopcounter; i++ ){
        int w;
        
        w = write( s, sendbuffer , sizeof( sendbuffer ));
        if( w <= 0 ){
            fprintf( stderr , "write error\n" );
            break;
        }
    }
    close( s );

    return 0;
}
