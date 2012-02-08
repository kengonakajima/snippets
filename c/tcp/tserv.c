

#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>


#define PORT 5666


static double getUTimeDouble(void);

struct sockaddr_in peer;

int main()
{
	double starttime= getUTimeDouble();
	int sockfd;
	int ret;
	struct sockaddr_in sin;
	int newsockfd;

	int plen = sizeof( struct sockaddr_in );
	
	FILE *fp;
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = INADDR_ANY;
	


	sockfd = socket( AF_INET , SOCK_STREAM , 0 );

	ret = bind( sockfd , (struct sockaddr*)&sin ,
			   sizeof( struct sockaddr_in));

	if( ret < 0 ){
		fprintf( stderr , "bind fuck!\n" );
	} else {
		fprintf( stderr , "bind ok!\n" );
	}
	ret = listen( sockfd, 5 );

	newsockfd= accept( sockfd , (struct sockaddr*)&peer, &plen );

	
	if( newsockfd < 0 ){
		fprintf( stderr , "Fuck!!!!\n");
	} else {
		fprintf( stderr , "accepted! fd:%d\n" , newsockfd );
	}
	

	fp = fopen( "tout.txt" ,"w");
	
	while(1){
		char buf[1000];
		int r;
		bzero( buf , sizeof( buf ));
		
		r = read( newsockfd , buf , sizeof( buf ));
		if( r == 0 )break;
		fwrite( buf, r , 1, fp );
		fprintf( stderr ," received a message len:%d\n",r);
	}

	close( newsockfd );
	close( sockfd );
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

