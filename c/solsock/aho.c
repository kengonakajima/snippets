
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


#include <sys/file.h>
#include <listen.h>

#include <sys/sockio.h>
#include <sys/socketvar.h>

#include <arpa/nameser.h>
#include <resolv.h>



int str_echo( int sockfd )
{

	int n;
#define MAXLINE 1000
	char line[MAXLINE];

	for(;;){
		n = read( sockfd , line , MAXLINE );
		if( n == 0 ){
			return;		/* connection closed by foreign host */
		}
		else if( n < 0 ){
			printf("socket is fucked up.\n");

		}
		if( write( sockfd , line , n ) != n ){
			printf("sockfd write failed.\n");
			return;
		}
	}
}

int main(int argc , char **argv )
{
	int mainsockfd;
    int newsockfd , clilen , childpid , servlen;
    struct sockaddr_in cli_addr , serv_addr;
	int select_any_connection;	
    int i;
	int port;
	
	struct timeval tm;		/* プロファイル用 */

	port = atoi( argv[1] );

	printf("Use port: %d\n" , port );
	
	mainsockfd = socket( PF_INET , SOCK_STREAM , IPPROTO_TCP );
	printf( "mainsockfd: %d\n" , mainsockfd );

    bzero((char *) &serv_addr , sizeof( serv_addr ));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);


  	i = bind( mainsockfd , (struct sockaddr *) &serv_addr , sizeof( struct sockaddr)) ;
	printf("bind returns: %d\n" , i );
	
	if( i < 0 ){
		printf("dataserv: bind error.\n");
		exit(1);
    }

    listen( mainsockfd , 5 );
	
    printf( "start loop. main socketFD=%d\n" , mainsockfd );
	printf( "Port: %d\n" , port );

	for(;;){

		clilen = sizeof( cli_addr );
		newsockfd = accept( mainsockfd , (struct sockaddr *) &cli_addr , &clilen );

		printf("accept!! newsockfd:%d\n" , newsockfd );
		
		if( newsockfd < 0 ){
			printf("fucked up.\n");
			exit(1);
		}
		
		str_echo( newsockfd );

		/* not reached */
	}
	
}
