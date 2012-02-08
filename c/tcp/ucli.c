#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>


#define PORT 7777

static double getUTimeDouble(void);

int main()
{
	double starttime = getUTimeDouble();
	int sockfd;
	int i=0;
	struct sockaddr_in sin;
	int ret;
	FILE *fp;
	
	sockfd = socket( AF_INET , SOCK_DGRAM ,0 );
	

	sin.sin_family = AF_INET;
	sin.sin_port = htons( PORT );
	sin.sin_addr.s_addr = inet_addr( "192.168.1.2" );

	fp = fopen( "in.txt" , "r" );
	while(1){
		char buf[3000];
		int r;
		
		bzero( buf , sizeof( buf) );
		
		if( fgets( buf , sizeof( buf) , fp ) == 0 )break;


		i++;
		
		r = sendto( sockfd , buf , strlen(buf) , 0,
				   (struct sockaddr*)&sin , sizeof( struct sockaddr_in ));
		

		fprintf( stderr, "line %d sent %d bytes\n" , i ,r );
	}
	close(sockfd );
	fclose( fp );

	fprintf( stderr , "TIME: %f msec\n" ,
			( getUTimeDouble() - starttime ) / 1000.0 );
	
}



static double getUTimeDouble(void)
{
	struct timeval tm;

	gettimeofday( &tm , NULL );

	return (double)( tm.tv_usec ) + (double)( tm.tv_sec) *
	1000000.0 ;
}
