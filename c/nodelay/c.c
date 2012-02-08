#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

int set_nodelay(int s )
{
    int flag = 1;
    int result = setsockopt( s, IPPROTO_TCP, TCP_NODELAY,
                             (char*)&flag, sizeof( int));
    if( result < 0 ){
        fprintf( stderr , "tcp_nodelay set error\n" );
        return -1;
    }
    fprintf( stderr , "NODELAY\n");
    return 0;
}

double time_diff(struct timeval subtrahend,
                       struct timeval subtractor)
{
    return( (subtrahend.tv_sec - subtractor.tv_sec)
            + (subtrahend.tv_usec
            - subtractor.tv_usec  ) / (double)1E6 );
}


int do_nodelay = 0;
int max_count = 20;

int main( int argc, char **argv )
{
    int s , ns , clilen;
    struct sockaddr_in svaddr;
    double total_time = 0.0;
    
    if( argc != 1 ) do_nodelay = 1;
        

    s = socket( AF_INET, SOCK_STREAM, 0 );
    if( s < 0 ){
        return 1;
    }

    memset( &svaddr, 0 , sizeof( svaddr ));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons( 9999 );
    if( inet_aton( "192.168.1.78" , &svaddr.sin_addr ) == 0 ){
        return 2;
    }
    
    if(connect( s, (struct sockaddr*)&svaddr, sizeof( svaddr ))<0){
        return 3;
    }

    if( do_nodelay  && ( set_nodelay( s ) < 0 ) ){
        return 10;
    }
    while(1){
        int l;
        char buf[1000];
        struct timeval t,et;
        static int counter = 0;
        double d;
        
        usleep(300*1000);
        fprintf( stderr , "." );

        if( ( ++counter ) == max_count )break;
        snprintf( buf, sizeof( buf), "%d" , counter );
        l= write( s, buf, strlen( buf));
        gettimeofday( &t, NULL );        
        if( l <= 0 ){
            break;
        }
        l = read( s, buf, sizeof( buf ));
        gettimeofday( &et , NULL );
        d = time_diff( et, t );
        total_time += d;
        fprintf( stderr , "elapsed time:%f\n", d );
        if( l <= 0 ){
            break;
        }
    }
    close(s);
    
    fprintf( stderr, "done: Total time:%f, %f\n" ,
             total_time, total_time / max_count );
    
    return 0;
}

        
