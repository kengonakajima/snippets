
#include <stdio.h>

#include "udptcp.h"




int main( int argc , char **argv )
{

	unsigned long cliaddr;
	unsigned short cliport;
	
	int ufd , nfd;

	fprintf( stderr, "start\n");

	uinit(INADDR_ANY);
	fprintf( stderr, "inited\n");	
	ufd = usocket_serv( 8 );
	fprintf( stderr, "socketd %d\n", ufd );
	ulisten( ufd );
	fprintf( stderr, "listened\n");
	while(1){
		int r =uproc();
/*		fprintf( stderr, "uproced %d\n" , r);*/
		nfd = uaccept( ufd , &cliaddr );
		if( nfd >= 0 ){
			fprintf( stderr, "accepted! nfd:%d %x %d\n" , nfd ,cliaddr , ntohs(cliport) );
			break;
		}
	}
	while(1){
		int r = uproc();
		
		fprintf( stderr,"." );

		if( ureadable( nfd ) ){
			char buffer[800];
			int r;
			fprintf( stderr, "REadable!!\n");
			bzero( buffer ,sizeof(buffer));
			r = uread( nfd, buffer , sizeof( buffer ));
			fprintf( stderr, "UREAD: ret:%d\n" , r );
			if( r > 0 ){
				int wr;
				fprintf( stderr , "ReadData %d: [%s]\n" ,r, buffer );
				wr = uwrite( nfd , buffer , r );
				fprintf( stderr ,"Wrote %dbytes\n",wr );
				
			}
		}
		
		usleep(100*1000);

	}
	

	
}
