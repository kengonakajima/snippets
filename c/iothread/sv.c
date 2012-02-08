#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>


#define MAXFD 1000
struct fuck
{
    int use;
    int fd;
};

struct fuck fk[MAXFD];

int
main( int argc, char **argv )
{
    int s;
    struct sockaddr_in sin;
    if( argc == 1 )return 1;
    s = socket( AF_INET, SOCK_STREAM, 0 );
    sin.sin_family = AF_INET;
    sin.sin_port = htons( atoi( argv[1]) );
    sin.sin_addr.s_addr = INADDR_ANY;

    if(bind( s, (struct sockaddr_in*) &sin, sizeof(sin))<0){
        exit(2);
    }
    listen(s,5 );

    while(1){
        fd_set rfds,wfds;
        int r,i;
        struct timeval t;
        
        t.tv_sec = t.tv_usec = 0;

        FD_ZERO( &rfds );
        FD_ZERO( &wfds );

        /* 最初にaccept */
        FD_SET( s, &rfds );
        r = select( MAXFD + 1, &rfds,(fd_set*)NULL,(fd_set*)NULL, &t);
        
        if( FD_ISSET( s , &rfds ) ){
            int ns;
            int clilen;
            struct sockaddr_in cliaddr;

            clilen = sizeof( cliaddr );
            ns = accept( s, (struct sockaddr*) &cliaddr, &clilen );
            if( ns < 0 ){
                fprintf( stderr, "accept error!\n" );
                exit(1);
            }
            fprintf( stderr, "accepted:%d\n", ns );
            for(i=0;i<MAXFD;i++){
                if( fk[i].use == 0 ){
                    fk[i].fd = ns;
                    fk[i].use = 1;
                    break;
                }
            }
        }

        /* つぎによみこみ */
        FD_ZERO( &rfds );
        for(i=0;i<MAXFD;i++){
            if( fk[i].use )FD_SET(fk[i].fd, &rfds );
        }
        
        t.tv_sec = t.tv_usec = 0;        
        r = select( MAXFD + 1, &rfds,(fd_set*)NULL,(fd_set*)NULL, &t);
        for(i=0;i<MAXFD;i++){
            if( fk[i].use && FD_ISSET( fk[i].fd, &rfds ) ){
                char rbuf[1000];
                /* 読みこんだデータはすて */
                memset( rbuf, 0 , sizeof( rbuf));
                read( fk[i].fd, rbuf,sizeof(rbuf)-2);
            }
        }

        /* つぎにかっきこみ */
        FD_ZERO( &wfds );
        for(i=0;i<MAXFD;i++){
            if( fk[i].use )FD_SET( fk[i].fd , &wfds );
        }            
        t.tv_sec = t.tv_usec = 0;        
        r = select( MAXFD + 1, (fd_set*)NULL,&wfds, (fd_set*)NULL, &t);
        for(i=0;i<MAXFD;i++){
            if( fk[i].use && FD_ISSET( fk[i].fd, &wfds ) ){
                char buf[1000];
                static int counter =0;
                snprintf( buf, sizeof(buf),
                          "%d abcdefgfasdfhasdkjfhasjdhfaksjdhgfkajhsdf"
                          "asdkfjhasdlkfjhalskdjfhalksdjhflkasjdhflkasjdhf"
                          "asdkfjhasdkjfhalskdjfhaklsdjhflakjsdhfklasjhdflkas"
                          "#asdkjhfalskdjfhlaskdjhflaksjdhflaksdhjflsadkfhja"
                          "asdkjfhasdkfjhalsdkjfhlaskdjfhlaskdjhflaksjdh"
                          "asdfkjashdkfjhalsdkfjahsldjkfhlaksdjhf"
                          "asdkjfhasdkjfhalskdjhflakjdhfklajdhflskajdhflkas"
                          "asdkfjhaskdjfhalksdjhflakdjhfaskdjhflkasjdh"
                          "asdkjfhasdlkhjfaslkdjfhlakdjhflakdjhflakjsdhf"
                          "asdkjfahsdkfjhasdklfjhalsdjkfhaklsdjfhasf"
                          "alskdjhfasldjkhfalkdjfhlasdjfhlaskdjfhlkj"
                          "akdjhfalsdjkfhalskjfasdjkhijklmnopqrstuvwxyz\n",
                          counter++ );
                if( write( fk[i].fd, buf,strlen(buf)) <= 0 ){
                    fprintf( stderr, "write error close\n" );
                    close(fk[i].fd );
                    fk[i].use = 0;
                }
            }
        }
    }
}
