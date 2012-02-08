/*
 *
 * bcast
 *
 *
 *
 *
 *
 */

#include "stdio.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "sys/types.h"


#define BCASTADDR "192.168.1.255"

struct sockaddr_in cli_addr;
struct sockaddr_in send_addr;

int main()
{

    /* make a socket */
    int sockfd = socket( AF_INET , SOCK_DGRAM , 0 );
    
    if( sockfd <0 ){
        fprintf( stderr , "socket error\n" );
        return 1;
    }


    /* set socket option  */
    {
        int aho;
        int flag;
        int len;
        flag = 1;
        aho = setsockopt( sockfd ,SOL_SOCKET , SO_BROADCAST , (char*) &flag , 4 );
        aho = getsockopt( sockfd ,SOL_SOCKET , SO_BROADCAST , (char*)&flag , &len );
        fprintf( stderr , "getopt: aho:%d flag:%d len:%d\n" , aho  , flag ,len ); 
    }

    
    /* bind to an addr */
    bzero((char*) &cli_addr , sizeof( cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    cli_addr.sin_port = htons(0);
    if( bind(sockfd , ( struct sockaddr*) &cli_addr , sizeof( cli_addr)) < 0 ){
        fprintf( stderr , "bind error\n" );
        return 12;
    }
    
    /* make an destination address */
    bzero( (char*) &send_addr , sizeof( send_addr));
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = inet_addr( BCASTADDR );
    send_addr.sin_port = htons(9999);
    
    /* write to this socket */
    {
        int i;
        for(i=0;i<1000;i++){
            char buf[100];
            int ret;
            sprintf( buf , "hoge:%d" , i );
            ret = sendto( sockfd , buf , strlen( buf ) , 0,(struct sockaddr*)&send_addr,sizeof( send_addr));
            fprintf( stderr, "sendto return:%d %s\n" , ret ,buf );
            sleep(1);
        }
    }

    

    
     

    close(sockfd);
    
}



