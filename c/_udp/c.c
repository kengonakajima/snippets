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

#include <netinet/in.h>
#include <arpa/inet.h>


int dg_cli( int sockfd, struct sockaddr *pserv_addr , int servlen )
{
	int n , dummylen = 0,i;
	char sendline[100] , recvline[100];

	for(i=0;i<34523;i++){
		
		sleep(1);
		sprintf( sendline  , "%d " , i );
		n = strlen( sendline );
		if( sendto( sockfd, sendline , n , 0 , pserv_addr , servlen )  != n ){
			fprintf( stderr , "sendto error\n" );
		}
		n = recvfrom( sockfd , recvline , 99 , 0 , (struct sockaddr *)0 , &dummylen );
		if( n < 0 ){
			fprintf( stderr , "recvfrom error\n" );
		}
		recvline[n] = 0;
		fprintf( stderr , "[%s]\n" , recvline );
		
			
	}
	
}

int main()
{
	int sockfd;
	struct sockaddr_in cli_addr , serv_addr;

	bzero( (char* ) &serv_addr , sizeof( serv_addr ));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr( "192.168.1.61" );
	serv_addr.sin_port = htons( 10001 );

	if( ( sockfd = socket( AF_INET , SOCK_DGRAM , 0 )) < 0 ){
		fprintf( stderr ,"fuck\n" );
	}

	bzero( ( char * ) &cli_addr , sizeof( cli_addr ));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr= htonl( INADDR_ANY );
	cli_addr.sin_port = htons( 0 );
	if( bind( sockfd , (struct sockaddr *) &cli_addr , sizeof( cli_addr )) < 0 ){
		fprintf( stderr  , "shit\n" );

	}

	dg_cli( sockfd , (struct sockaddr *) & serv_addr , sizeof( serv_addr ));
	close( sockfd);
	


	

}
