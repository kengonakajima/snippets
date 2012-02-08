#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>


#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>		/* マルチスレッド関連 */
#include <sched.h>

//#if SOLARIS25				/* Solaris特有のヘッダ類 */
#include <sys/file.h>
#include <listen.h>
#include <sys/sockio.h>
#include <sys/socketvar.h>
#include <arpa/nameser.h>
#include <resolv.h>

//#endif




int dg_echo( int sockfd , struct sockaddr *pcli_addr , int maxclilen )
{
	int n , clilen;
	char msg[1000];

	for(;;){
		fprintf( stderr, "M" );
		clilen = maxclilen;
		n = recvfrom( sockfd , msg , 1000 , 0 , pcli_addr , &clilen );
		if( n < 0 ){
			//fprintf( stderr , "recvfrom error\n" );
			continue;
		}
		if( sendto( sockfd, msg , n , 0 , pcli_addr , clilen ) != n ){
			fprintf( stderr , "sendto error\n" );
		}
		
			
	}
}
int main()
{
	int sockfd;
	struct sockaddr_in serv_addr , cli_addr;

	if( ( sockfd = socket( AF_INET , SOCK_DGRAM , 0 )) < 0 ){
		fprintf( stderr , "fuck\n" );

	}
	bzero( (char*) &serv_addr , sizeof( serv_addr ) );

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	serv_addr.sin_port = htons( 10001 );

	if( bind( sockfd , ( struct sockaddr *) &serv_addr , sizeof( serv_addr ) ) < 0 ){
		fprintf( stderr , "shit\n" );
	}

	fcntl( sockfd , F_SETFL , FNDELAY );
	dg_echo( sockfd, (struct sockaddr *) & cli_addr , sizeof( cli_addr ) );
	
	
}
