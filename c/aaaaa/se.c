/* unsync I/O server test */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

#define ON 1
#define OFF 0

int fdlist[256];

int main(int argc , char **argv)
{
    int sockfd ,newsockfd , clilen , servlen;
    struct sockaddr_in cli_addr , serv_addr;
    int i,n;
    
    for( i = 0 ; i < 256 ; i++) fdlist[i] = OFF;

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );
    if( sockfd < 0 ){
        perror("socket error");
        exit(0);
    }

    fdlist[sockfd] = ON;

    bzero((char *) &serv_addr , sizeof( serv_addr ));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = atoi( argv[1] );
    serv_addr.sin_addr.s_addr  =  INADDR_ANY;
    if( bind( sockfd , (struct sockaddr *)&serv_addr , sizeof( struct sockaddr )
) < 0 ){
        perror("bind error");
        exit(0);
    }

    listen( sockfd , 5 );

    fprintf( stderr,"start loop. main socket=%d\n" , sockfd );
    for(;;){
        struct timeval tm;
        fd_set fdv;
        int fdnum;
        
        fprintf(stderr,"." );
        
        tm.tv_sec =  0;
        tm.tv_usec = 200*1000;
        FD_ZERO( & fdv);
        
        
        for(i = 0 , fdnum = 0 ; i < 256 ; i++){
            if( fdlist[i] == ON ){
                FD_SET( i , &fdv);
                fdnum++;
            }
        }
        select( fdnum + 10 , &fdv , (fd_set *)NULL , (fd_set *)NULL , &tm );
        for(i = 0 ; i < 256 ; i++){
            if( FD_ISSET(i , &fdv )){
                fprintf(stderr,"/");
                if( i == sockfd ){
 		    clilen = sizeof( cli_addr );
                    newsockfd = accept( sockfd , ( struct sockaddr *)&cli_addr ,
			 &clilen );
                    if( newsockfd < 0  ){
                        perror("accept error");
                    }
                    fdlist[newsockfd] = ON;
                    fprintf(stderr,"new connection. fd = %d\n" , newsockfd );
                } else {
                    char data;
                    n = read( i , &data , 1 );

                    if( n <= 0 ){
                        fdlist[i] = OFF;
                        close(i);
                        fprintf(stderr,"close! fd = %d\n" , i );
                        continue;
                    }
                    fprintf(stderr,"Data input: %d\n" , (int)data );
                }
                
            }
        }

        
               
                        
        
    }
    
    
}

