#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_aton
#include <stdlib.h>



#define error_exit(msg) { perror(msg); exit(1); }

int main( int argc, char **argv ) {
    int ret;

    if( argc != 3 ) {
        fprintf(stderr, "Usage: cli IPADDR PORTNUMBER\n");
        return 1;
    }
    
    int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if(sockfd<0) error_exit("socket");


    
    struct sockaddr_in cliaddr;

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons( atoi( argv[2]) );
    inet_aton( argv[1], &cliaddr.sin_addr );
    if( cliaddr.sin_addr.s_addr == 0 ) error_exit( "address" );

    ret = connect( sockfd, (struct sockaddr*) &cliaddr, sizeof(cliaddr) );
    if(ret<0) error_exit( "connect" );

    printf("connected\n");
    int cnt = 0;
    long long total_sent = 0;
    while(1){
        cnt++;
        char buffer[1024*16];
        for(int i=0;i<sizeof(buffer);i++) buffer[i] = cnt % 0xff;
        
        ret = send( sockfd, buffer, sizeof(buffer), 0 );
        if(ret<0) error_exit( "send" );
        total_sent += ret;

        ret = recv( sockfd, buffer, sizeof(buffer), 0 );
        if( ret < 0 ) error_exit( "recv" );

        if( (cnt % 10000 ) == 0 ) {
            fprintf(stderr, "sent: %lld Mbytes cnt:%d\n", total_sent/1024/1024, cnt );
        }
    }
    return 0;
}
