
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
	int l=0;
	int n;
	
	if(rc_init( 9005 , 100 )<0) return -1;
	

	fd = rc_open( inet_addr( "192.168.1.61") , htons( 9008 ) );
	printf( "FD:%d\n" , fd );


	
	for(l=0;l<30;l++){
		char msg[100];
		sprintf(msg , "%d" , l );
		n = rc_write( fd , msg , strlen(msg) );
		fprintf( stderr,"rc_write returns %d\n" , n );
		rc_proc();
		usleep(40*1000);
	}

	while(1){
		rc_proc();
	}
	
	
}
