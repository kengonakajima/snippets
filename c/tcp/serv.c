#include <stdio.h>
#include "rp.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <strings.h>


/*
   読みこんでファイルに保存
*/

int main(int argc, char **argv )
{
	int fd;
	int a;
	FILE *fp;
	double starttime = getUTimeDouble();
	char buf[10000];
		
	if( (a=rp_init( 9003,100))<0){
		fprintf( stderr,"rp_init fucks. %d\n",a);
		return -1;
	}

	unlink( "out.txt");

	
	while(1){
		unsigned long ip_net;
		unsigned short port_net;

		rp_proc();
		if( rp_acceptable() ){
			fd = rp_accept(&ip_net , &port_net);
			if( fd >= 0 ) break;
		}
	}

	fprintf( stderr,"accepted fd:%d\n", fd );

	
	while(1){
		
		int r;
		extern int current_seq;
		
		rp_proc();
		bzero(buf,sizeof(buf));
		   	if( (r=rp_recv( fd , buf , sizeof(buf))) > 0 ){
			fp = fopen( "out.txt" , "a+");			
			fwrite( buf , r,1,fp );
			fclose(fp);
			if( buf == (char*)0 ){
				fprintf( stderr , "FUCK!\n");
			}
				
			fprintf( stderr ,"received a message len:%d seq:%d\n",r ,
					current_seq );
		} else {
			
			if( r < 0 ){
				break;
			} else {
/*				fprintf( stderr , "R is ZERO!!!!!!!!!!!!!!!!!!\n");*/
			}
		}
	}


	fprintf( stderr , "TIME: %f msec\n" ,
			( getUTimeDouble() - starttime ) / 1000.0);

	return 0;
	
}
