#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>


#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>



/*

  network にたいする write のバッファリングのテスト
  
 */
int main( int argc , char ** argv )
{
    int s = socket( AF_INET , SOCK_STREAM , 0 );
    struct sockaddr_in sin;
    int i;
    int w;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr( "192.168.1.78" );
    sin.sin_port = htons( 10000 );

    if(connect( s, (struct sockaddr*) & sin , sizeof( sin ))<0){
        return 4;
    }

    w = 0;
    for(i=0;i<1000000;i++){
        char buf[10];
        buf[i] = 1;
        if(write( s , buf , 1 )==1){
            w++;
            if( ( w % 1000 ) == 0 )fprintf( stderr , "." );
        }
    }
}
