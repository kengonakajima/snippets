#include <stdio.h>
#include "udpc.h"



int main()
{
	int vfd;
	UDPCInit(9001);

	while(1){

		
		UDPC();
		vfd = UDPCAccept();

		if( vfd >= 0 ){
			fprintf( stderr, "Accepted %d\n" , vfd );
			break;
		}
	}


	while(1){
		int n;
		static int c=0;
		char buf[100];
		bzero( buf,sizeof(buf));
		n = UDPCRead( vfd , buf,sizeof(buf));
		if( n > 0 ){
			fprintf( stderr ,"%d bytes read from vfd:[%s]\n",n,buf );
			n = UDPCWrite( vfd, buf , n );
			fprintf( stderr ,"%d bytes wrote\n",n );
		} else {
			if( ( ++c % 1000)==0)fprintf( stderr,"%d\n",c);
		}

		UDPC();
		
	}
}
