#include "stdio.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "sys/types.h"


#define BCASTADDR "192.168.1.255"

struct sockaddr_in cli_addr;
struct sockaddr_in serv_addr;


int main()
{
    int sockfd = socket( AF_INET , SOCK_DGRAM , 0 );
    
    if( sockfd <0 ){
        fprintf( stderr , "socket error\n" );
        return 1;
    }
    
    bzero((char*) &serv_addr , sizeof( serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    serv_addr.sin_port = htons(9999);
    if( bind(sockfd , ( struct sockaddr*) &serv_addr , sizeof( serv_addr)) < 0 ){
        fprintf( stderr , "bind error\n" );
        return 12;
    }

    while(1){
        int ret;
        char buf[100];
        int len = sizeof( cli_addr) ;
        ret = recvfrom( sockfd ,buf , sizeof(buf), 0 ,
                        (struct sockaddr*)&cli_addr , &len );
        if( ret<0)continue;
        
        buf[ret]= 0;
        fprintf( stderr , "recvfrom:%d %s\n" , ret , buf );
    }
    close(sockfd);
    
}
