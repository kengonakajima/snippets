


#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>


#define PORT 5666
static double getUTimeDouble(void);

int main()
{
	double starttime = getUTimeDouble();
	
	struct sockaddr_in sin;
	int ret;
	int sockfd;
	FILE *fp;
	int i=0;
	
	extern int errno;
	
	sockfd = socket( AF_INET , SOCK_STREAM ,0 );
	

	sin.sin_family = AF_INET;
	sin.sin_port = htons( PORT );
	sin.sin_addr.s_addr = inet_addr( "192.168.1.61" );

	ret = connect( sockfd,(struct sockaddr*)&sin,sizeof(struct sockaddr_in));

	if( ret < 0 ){
		fprintf( stderr,"connect fuck! %d\n" , errno);
	} else {
		fprintf( stderr,"connect ok!\n");
	}
	
	fp = fopen( "in.txt" , "r" );
	while(1){
		char buf[100];
		int r;
		
		bzero( buf , sizeof( buf) );
		
		if( fgets( buf , sizeof( buf) , fp ) == 0 )break;

		i++;
		
		r = write( sockfd , buf , strlen(buf ));

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
