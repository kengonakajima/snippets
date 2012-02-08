#include "rc.h"


#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>

#include <malloc.h>




int main()
{
	int fd;

	if(rc_init( 9008 , 100 )<0){
		return -1;
	}

	while(1){
		
		rc_proc();
		fd = rc_accept();
		if( fd >= 0 ){
			break;
		}
	}

	unlink( "out" );
	
	printf( "Accepted fd:%d\n" , fd );

	while(1){
		int n;
		char dummy[10];
		char buf[10000];
		char dummy2[100];

		
		rc_proc();

		n = rc_read( fd , buf , sizeof(buf));

		if( n != 0 ){
			FILE *fp;
			fp = fopen( "out" , "a+");
			fprintf( fp , "%c%c%c%c\n" , buf[0] , buf[1] , buf[2] , buf[3]);
			fclose(fp);
			fprintf( stderr , "rc_read returns  %d msg:%c%c%c%c\n" , n ,
							buf[0] , buf[1] , buf[2] , buf[3]);
			
		}
		

		
	}

}

