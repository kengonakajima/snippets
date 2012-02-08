#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>


#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>



int main( int argc , char ** argv )
{

    struct sockaddr_in sin , cliaddr;
    int cliaddrlen;
    int s ;
    int ns;
    
    s = socket( AF_INET , SOCK_STREAM , 0 );
    sin.sin_addr.s_addr= INADDR_ANY;
    sin.sin_port = htons( 10000);
    if(bind( s , (struct sockaddr*) &sin , sizeof( sin ))<0){
        fprintf( stderr ,"bind error\n");
        return 1;
    }

    listen( s, 05 );

    cliaddrlen = sizeof( cliaddr );
    ns = accept( s , (struct sockaddr*) &cliaddr , &cliaddrlen );
    if(ns < 0 ){fprintf(stderr , "accept error\n");exit(91);}
    while(1){
        int rs;
        char buf[10000];
        rs = read( ns , buf , sizeof(buf ));
        if(rs < 0 ) continue;
        if(rs == 0 )break;
        fprintf( stderr, "%d " , rs );
    }
    close(ns);
    close(s);
    return 0;
}
