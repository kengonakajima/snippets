#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>


/*
  
  Bind an sock_dgram socket and send to each other with the
  socket.

  This program compiles well on red hat 5.2 or later.
  
 */

int s;
char remoteaddr[100];
int remoteport ;


void sendmesg( int s , char *adr , int port , char *msg )
{
    struct sockaddr_in sa;
    int r;
    
    inet_aton( adr , & sa.sin_addr );
    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );

    errno = 0;
    r = sendto( s , msg , strlen( msg ) , 0 , &sa , sizeof( sa ));
    fprintf( stderr , "sendto returns: %d : %s\n", r , strerror(errno ));
    
}
void usr1( int a )
{
    printf( "%d\n", a);
    sendmesg( s , remoteaddr , remoteport  , "aho" );    
}

int main(int argc , char **argv )
{
    struct sockaddr_in sin;

    
    memset( &sin , 0, sizeof( sin ));
    
    if(argc != 5 ){
        fprintf( stderr , "argumenet fucks: Usage: a.out A P RA RP\n" );
        exit(0);
    }
    if( inet_aton( argv[1] , &sin.sin_addr ) == 0 ){
        fprintf( stderr , "IP address fucked: %s\n" ,argv[1] );
        exit(1);
    }
    
    sin.sin_port = htons( atoi( argv[2] ));
    sin.sin_family = AF_INET;
    
    s = socket( AF_INET , SOCK_DGRAM , 0 );
    if( s <0 ){
        fprintf( stderr, "socket() fucks\n");
        exit(1);
    }

    if( bind( s , &sin , sizeof( sin ))< 0){
        fprintf( stderr ," bind() fucks. err is : %s\n",  strerror(errno));
        exit(1);
    }

    remoteport = atoi( argv[4] );
    strcpy( remoteaddr , argv[3] );
    
    {
        struct sigaction a , oa;
        a.sa_handler = usr1;
        sigemptyset( &a.sa_mask );
        a.sa_flags = 0;
        a.sa_restorer = NULL;

        a.sa_flags |= SA_RESTART;
        
        if( sigaction( SIGUSR1 , &a , &oa ) < 0 ){
            fprintf( stderr ,"sigaction error %s\n", strerror( errno ));
            exit(1);
        }
    }

    while(1){
        fprintf( stderr , "%d " , getpid() );
        sleep(1);
    }


    close(s);
}
