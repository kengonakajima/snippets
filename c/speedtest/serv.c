#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main( int argc, char **argv )
{

    char readbuffer[100000];
    int ns;
    int s = socket ( AF_INET , SOCK_STREAM , 0 );

    struct sockaddr_in sin, cliaddr;
    int clilen;

    if( argc <=1 ){
        fprintf( stderr , "usage: serv ipaddr\n" );
        return 1;
    }
    
    memset( &sin , 0 , sizeof( sin ));
    sin.sin_family = AF_INET;
    sin.sin_port = htons( 10000 );
    sin.sin_addr.s_addr = inet_addr( argv[1] );

    if ( bind ( s , (struct sockaddr* ) &sin , sizeof( sin )) < 0 ){
        fprintf( stderr , "cannot bind to the localhost addr\n" );
        return 1;
    }
    listen( s, 5 );

    while(1){      /* accept-loop */
        int total=0;
        
        clilen = sizeof( cliaddr );
        ns = accept(  s, ( struct sockaddr* ) &cliaddr ,  &clilen );
        if( ns < 0 ){
            fprintf( stderr, "accept error\n" );
            return 1;
        }
        system( "date" );

        fprintf( stderr,  "start reading...\n" );
        while(1){

            int c;

            c = read( ns, readbuffer, sizeof( readbuffer ));
        
            if( c <= 0 ){
                break;
            } else {
                total += c;
            }
        }
        system( "date" );
        fprintf( stderr ,"read %d M bytes.\n" , total/1000000 );
        close(ns );
    }
    
    close(s);
    
    fprintf( stderr , "loop end.\n" );
    return 0;
}

