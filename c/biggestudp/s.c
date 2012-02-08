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

    if(bind( s, (struct sockaddr_in *)&sin, sizeof( sin ))<0){
        fprintf( stderr , "bind error : %s\n",
                 strerror( errno ));
        return 1;
    }


    while(1){
        char buf[65536];
        int r,l,lprev;
        struct sockaddr_in cliaddr;
        l = sizeof( cliaddr );
        fprintf(stderr, "waiting for..." );
        r = recvfrom( s, buf, sizeof( buf),0,&cliaddr, &l );
        if( r > (lprev+1)){
            fprintf(stderr,"SKIPPPPPPPP\n" );
        }
        lprev = r;
        if(r>0){
            fprintf(stderr,"recv %d\n", r );
        } else {
            fprintf(stderr,"recvfrom error:%s\n",
                    strerror(errno));
        }
    }
}
