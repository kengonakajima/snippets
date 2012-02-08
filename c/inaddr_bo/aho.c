#include <netinet/in.h>
#include <arpa/inet.h>

int
main()
{
    struct sockaddr_in s;

    inet_aton( "192.168.1.0", &s.sin_addr );

    printf( "%x\n", s.sin_addr.s_addr );
    
}
