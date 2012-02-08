#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>

int
main()
{
    int s, sinlen;
    struct sockaddr_in sin, singot;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(7);
    inet_aton( "192.168.1.24", &sin.sin_addr );

    s = socket( AF_INET, SOCK_STREAM, 0 );
    if( connect( s, (struct sockaddr*)&sin, sizeof(sin)) < 0 ){
        printf( "aho\n" );
        return 1;
    }

    sinlen = sizeof( sin);
    if( getsockname( s, (struct sockaddr*)&singot, &sinlen ) < 0 ){
        printf( "hoge\n" );
        return 1;
    }

    printf( "%s %d\n", inet_ntoa(singot.sin_addr), ntohs(singot.sin_port));

    return 0;
}
