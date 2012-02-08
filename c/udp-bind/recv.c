#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

int s;
int main(int argc , char **argv )
{
    struct sockaddr_in sin;
    static int i=0;
    memset( &sin,0,sizeof(sin));

    if( argc != 3 ){
        fprintf(stderr , "Usage: recv A P\n" );
        exit(0);
    }

    sin.sin_port = htons( atoi( argv[2] ));
    sin.sin_family = AF_INET;
    inet_aton( argv[1] , &sin.sin_addr );

    s = socket( AF_INET , SOCK_DGRAM , 0 );

    if( bind( s , &sin , sizeof( sin )) <0 ){
        fprintf( stderr ," bind() fucks. err is : %s\n",  strerror(errno));
        exit(1);
    }


    while(1){
        int r;
        char buf[100];
        struct sockaddr_in cli;
        int l = sizeof(cli);
        r = recvfrom(s , buf,sizeof(buf), 0 , &cli , &l );
        fprintf( stderr , "recvfrom:%d PORT:%d i:%d\n", r ,
                 ntohs(cli.sin_port),i
                 );
        if( r > 0 ){
            fprintf( stderr , "sendto:%d\n",
                     sendto(s , buf,r ,  0 , &cli , l ));
        }
        i++;
    }
    close(s);
    
}
