#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

/*
  新方式のサーバー。
 */


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
    sin.sin_port = htons( atoi(argv[1]) );
    sin.sin_addr.s_addr = INADDR_ANY;

    if(bind( s, (struct sockaddr_in*) &sin, sizeof(sin))<0){
        exit(2);
    }
    listen(s,5 );

    while(1){
        int r,i;
        struct timeval t;
        fd_set fds;                

        t.tv_sec = t.tv_usec = 0;
        FD_ZERO( &fds );
        /* 最初にaccept */
        FD_SET( s, &fds );
        r = select( MAXFD + 1, &fds,(fd_set*)NULL,(fd_set*)NULL, &t);
        if( FD_ISSET( s , &fds ) ){
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
            
        for(i=0;i<MAXFD;i++){
            if( !fk[i].use )continue;
            /* read */
            FD_ZERO( &fds );
            FD_SET(fk[i].fd, &fds );

            t.tv_sec = t.tv_usec = 0;        
            r = select( MAXFD + 1, &fds,(fd_set*)NULL,(fd_set*)NULL, &t);
            
            if( FD_ISSET( fk[i].fd, &fds ) ){
                char rbuf[1000];
                memset( rbuf, 0 , sizeof( rbuf));
                read( fk[i].fd, rbuf,sizeof(rbuf)-2);
            }
            /* write */
            FD_ZERO( &fds );
            FD_SET( fk[i].fd , &fds );
            t.tv_sec = t.tv_usec = 0;        
            r = select( MAXFD + 1, (fd_set*)NULL,&fds, (fd_set*)NULL, &t);
            if( FD_ISSET( fk[i].fd, &fds ) ){
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
                    continue;
                }
            }
            /* oob */
            FD_ZERO( &fds );
            FD_SET( fk[i].fd , &fds );
            t.tv_sec = t.tv_usec = 0;
            r = select( MAXFD + 1, (fd_set*)NULL,(fd_set*)NULL,&fds,&t);
            if( FD_ISSET( fk[i].fd, &fds ) ){
                close( fk[i].fd );
                fk[i].use =0;
                fprintf( stderr , "oob close\n" );
                continue;
            }
        }
    }
}
