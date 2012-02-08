#include <netinet/tcp.h>
#include <sys/socket.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

int
main()
{
    int s;
    int i,l;
    struct sockaddr_in sin;

    s = socket(AF_INET,SOCK_STREAM,0 );

    memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(7);
    inet_aton( "127.0.0.1", &sin.sin_addr );
    if(connect( s, (struct sockaddr*) &sin, sizeof(sin))<0){
        fprintf(stderr,"fuck\n");
        return 1;
    }

    while (1){
        sleep(1);
        write(s, "aho", 3 );
        
        l=sizeof(i);
        if( getsockopt( s, SOL_SOCKET,  SO_RCVBUF, &i,&l)<0) return 1;
        fprintf(stderr, "SO_RCVBUF:%d\n", i );

        l=sizeof(i);
        if( getsockopt( s, SOL_SOCKET,  SO_SNDBUF, &i,&l)<0) return 1;
        fprintf(stderr, "SO_SNDBUF:%d\n", i );

        l=sizeof(i);
        if( getsockopt( s, SOL_SOCKET,  SO_RCVLOWAT, &i,&l)<0) return 1;
        fprintf(stderr, "SO_RCVLOWAT:%d\n", i );

        l=sizeof(i);
        if( getsockopt( s, SOL_SOCKET,  SO_SNDLOWAT, &i,&l)<0) return 1;
        fprintf(stderr, "SO_SNDLOWAT:%d\n", i );        

    }
}
