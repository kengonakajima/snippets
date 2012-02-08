#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int
main( int argc, char **argv )
{
    int s;
    struct sockaddr_in sin;
    
    s = socket( AF_INET, SOCK_DGRAM, 0 );

    memset( &sin, 0 , sizeof( sin ));
    sin.sin_port = htons(10000);
    inet_aton( argv[1] , & sin.sin_addr );
    sin.sin_family = AF_INET;


    while(1){
        char buf[65536];
        int r;
        static int i=1;
        r = sendto( s, buf,i,0,&sin, sizeof(sin));
        i++;
        if(r>0){
            fprintf(stderr,"send %d\n", r );
        } else {
            fprintf(stderr,"sendto fuck: %s\n",
                    strerror(errno));
        }
    }
}
