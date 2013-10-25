#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>


#define error_exit(msg) { perror(msg); exit(1); }

int main( int argc, char **argv ) {
    int ret;
    
    int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if(sockfd<0) error_exit("socket");

    struct sockaddr_in svaddr;

    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons( 18181 );
    svaddr.sin_addr.s_addr = htonl( INADDR_ANY );


    ret = bind( sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr));
    if(ret<0) error_exit("bind");

    ret = listen(sockfd,5);
    if(ret<0) error_exit("listen");

    while(1) {
        printf("loop top\n");
        struct sockaddr_in cliaddr;
        int cliaddr_sz = sizeof(cliaddr);
        
        int cli_sockfd = accept( sockfd, (struct sockaddr*) &cliaddr, &cliaddr_sz );
        if( cli_sockfd < 0 ) error_exit( "accept");

        printf("client accepted: %d\n", cli_sockfd );

        while(1){
            char buf[100];
            int readsz = read( cli_sockfd, buf, sizeof(buf));
            if( readsz <= 0 ) {
                printf("close client: %d\n", cli_sockfd );
                close(cli_sockfd);
                break;
            } else {
                ret = write( cli_sockfd, buf, readsz );
                if( ret < 0 ) error_exit( "write");
            }
        }
    }
    return 0;
}

    
