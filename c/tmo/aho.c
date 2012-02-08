#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int
main()
{
    int s;
    struct sockaddr_in sin;
        

    s = socket( AF_INET, SOCK_STREAM, 0 );

    memset( &sin,0,sizeof(sin));
    sin.sin_port = htons(9002 );
    sin.sin_family = AF_INET;
    inet_aton( "192.168.1.63", &sin.sin_addr );

    if( connect( s, (struct sockaddr*)&sin, sizeof( sin ) ) < 0 ){
        fprintf( stderr, "connect fuck\n" );
        return 1;
    }


    while(1){
        fd_set fds;
        struct timeval t;
        FD_ZERO( &fds );
        FD_SET( s, &fds );
        t.tv_sec = t.tv_usec = 0;
        select( s+1,&fds,0,0,&t );
        if( FD_ISSET( s, &fds )){
            int r;
            char buf[10];
            fprintf( stderr, "readabale\n" );
            r = read( s, buf,sizeof(buf));
            fprintf( stderr, "return:%d %s\n", r , strerror(errno) );
        }
    }
    
    close(s);
    fprintf( stderr,"done\n" );
}
