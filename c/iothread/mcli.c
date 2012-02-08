/*

  たくさん同時にconnectするのだ。
  
 */
#include <stdio.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

struct so
{
    int use;
    int fd;
    double read_total , write_total;
};

#define MAXCON 1000
struct so so[MAXCON];
static double write_total = 0.0F;
static double read_total = 0.0F;

static int new_so( void )
{
    int i;
    for(i=0;i<MAXCON;i++){
        if( so[i].use == 0 ){
            return i;
        }
    }
    return -1;
}

static void
close_all( void )
{
    int i;
    for(i=0;i<MAXCON;i++){
        if( so[i].use ){
            close(so[i].fd );
        }
    }
    
}

double time_diff(struct timeval subtrahend,
                       struct timeval subtractor)
{
    return( (subtrahend.tv_sec - subtractor.tv_sec)
            + (subtrahend.tv_usec
            - subtractor.tv_usec  ) / (double)1E6 );
}


struct timeval time_store , now_time , time_store_sec;
struct timeval start_time;
static void
do_it( void )
{
    int i;
    double d;
    gettimeofday( &now_time , NULL );
    d = time_diff( now_time , time_store_sec );
    if( d >= 1.0 ){
        fprintf( stderr, "W:%9.0f(%9.0f/sec) R:%9.0f(%9.0f/sec)\n",
                 write_total ,
                 write_total / time_diff( now_time , time_store_sec ),
                 read_total ,
                 read_total / time_diff( now_time, time_store_sec )
                 );
        write_total = read_total = 0.0F;
        time_store_sec = time_store;
    }
    time_store = now_time;
    
    for(i=0;i<MAXCON;i++){

        fd_set fds;
        struct timeval t;

        if(!so[i].use )continue;

        /*read */

        FD_ZERO( &fds );
        FD_SET( so[i].fd, &fds );
        t.tv_sec = t.tv_usec = 0;
        select(MAXCON+1,&fds, (fd_set*)NULL, (fd_set*)NULL,&t);
        if( FD_ISSET( so[i].fd, &fds ) ){
            char buf[4096];
            int ret;
            ret = read( so[i].fd, buf,sizeof( buf));
            if(ret <= 0 ){
                close(so[i].fd );
                so[i].use =0;
                fprintf( stderr, "read error!closed\n" );
                exit(1);
            } else {
                so[i].read_total += ret;
                read_total += ret;
            }
        }
#if 1
        /* write */
        FD_ZERO( &fds );
        FD_SET( so[i].fd, &fds );
        t.tv_sec = t.tv_usec = 0;
        select( MAXCON +1, (fd_set*)NULL, &fds, (fd_set*)NULL,&t );
        if( FD_ISSET( so[i].fd, &fds )){
            int wlen = 5;
            int ret;
            char buf[4096];
            memset(buf,0,sizeof(buf));
            ret = write( so[i].fd, buf, wlen );
            if( ret <= 0 ){
                fprintf( stderr, "write error close\n ");
                close(so[i].fd );
                so[i].use=0;
            } else {
                so[i].write_total += ret;
                write_total += ret;
            }
        }
#endif
        /* ex */
        FD_ZERO( &fds );
        FD_SET( so[i].fd, &fds );
        t.tv_sec = t.tv_usec = 0;
        select( MAXCON +1, (fd_set*)NULL, (fd_set*)NULL, &fds,&t );
        if( FD_ISSET( so[i].fd, &fds )){
            close(so[i].fd );
            so[i].use=0;
            fprintf( stderr, "EXception close\n ");
            exit(1);
        }
    }
}


int
main(int argc,char **argv )
{

    int port,i;
    int count;
    if( argc != 4 ){
        fprintf( stderr, "Usage: mcli host port count\n" );
        return 1;
    }
    
    port = atoi( argv[2] );
    count = atoi( argv[3] );
    

    gettimeofday( &start_time, NULL );
    
    /* 最初に一挙にconnect */
    for(i=0;i<count;i++){
        int s , nso;
        struct sockaddr_in sin;
        memset( &sin, 0 , sizeof( sin ));
        sin.sin_family = AF_INET;
        sin.sin_port = htons( port );
        if( inet_aton( argv[1], &sin.sin_addr ) ==0 ){
            return 1;
        }
        s = socket( AF_INET, SOCK_STREAM, 0 );
        if( s < 0 ){
            fprintf( stderr , "socket() error:%s\n", strerror(errno ) );
            return 1;
        }

        if( connect( s, (struct sockaddr*)&sin, sizeof( sin )) < 0 ){
            fprintf( stderr ,"connect error\n" );
            return 1;
        }

        nso = new_so();
        if( nso < 0 ){
            fprintf( stderr , "new_so fail\n" );
            return 1;
        }

        so[nso].use = 1;
        so[nso].fd = s;
        so[nso].read_total = so[nso].write_total = 0.0F;
    }

    while(1){
        do_it();
    }

    close_all();

    return 0;
    
}
