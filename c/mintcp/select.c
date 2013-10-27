#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h> // close

#include <stdlib.h>

#define SOCKETNUM 100
int g_sockets[SOCKETNUM];

int clear_sockets(){
    for(int i=0;i<SOCKETNUM;i++) g_sockets[i] = -1;
}
    
void add_socket( int fd ) {
    for(int i=0;i<SOCKETNUM;i++){
        if( g_sockets[i] == -1 ) {
            g_sockets[i] = fd;
            break;
        }
    }
}
void del_socket( int fd ) {
    for(int i=0;i<SOCKETNUM;i++){
        if( g_sockets[i] == fd ) g_sockets[i] = -1;
    }
}
void fdset_update( fd_set *fds ) {
    for(int i=0;i<SOCKETNUM;i++){
        if( g_sockets[i] >= 0 ) {
            FD_SET( g_sockets[i], fds);
        }
    }
}
int max_sockfd() {
    int maxfd = 0;
    for(int i=0;i<SOCKETNUM;i++){
        if( g_sockets[i] > maxfd ) maxfd = g_sockets[i];
    }
    return maxfd;
}

#define error_exit(msg) { perror(msg); exit(1); }

int main( int argc, char **argv ) {

    clear_sockets();
    
    int ret;
    
    int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if(sockfd<0) error_exit("socket");

    struct sockaddr_in svaddr;

    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons( 18182 );
    svaddr.sin_addr.s_addr = htonl( INADDR_ANY );


    ret = bind( sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr));
    if(ret<0) error_exit("bind");

    ret = listen(sockfd,5);
    if(ret<0) error_exit("listen");

    while(1) {
        printf("loop\n");

        fd_set fds;

        FD_ZERO( &fds );
        FD_SET( sockfd, &fds );
        fdset_update( &fds );

        struct timeval tmv;
        tmv.tv_sec =0;
        tmv.tv_usec = 500 * 1000;
        int maxfd = max_sockfd();
        if( sockfd > maxfd ) maxfd = sockfd;
        int ret = select( maxfd + 1, &fds, NULL, NULL, &tmv );
        if(ret>0){
            printf( "readable: %d\n", ret );
            if( FD_ISSET( sockfd, &fds ) ) {
                struct sockaddr_in cliaddr;
                int cliaddr_sz = sizeof(cliaddr);
        
                int cli_sockfd = accept( sockfd, (struct sockaddr*) &cliaddr, &cliaddr_sz );
                if( cli_sockfd < 0 ) error_exit( "accept");
                printf("client accepted: %d\n", cli_sockfd );
                add_socket( cli_sockfd );
            }
            for(int i=0;i<SOCKETNUM;i++){
                if( g_sockets[i] >= 0 ) {
                    if( FD_ISSET( g_sockets[i], &fds ) ) {
                        printf("readable: %d\n", g_sockets[i] );
                        char buf[1000];
                        ret = recv( g_sockets[i], buf, sizeof(buf), 0);
                        if(ret<=0){
                            close( g_sockets[i] );
                            del_socket( g_sockets[i] );
                        } else {
                            if( ret < sizeof(buf)-1 ) {
                                buf[ret] = '\0';
                                printf( "echo: '%s'\n", buf );
                                send( g_sockets[i], buf, ret, 0 );
                            }
                        }
                            
                    }
                }
            }
        }

    }
    return 0;
}

    
