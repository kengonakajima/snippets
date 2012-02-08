#include "rp.h"


#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>

#include <malloc.h>


/*

   このクライアントはテキストファイルを1行づつ送信する。
   
*/

int main(int argc, char **argv )
{
	char buf[100];
	int fd;
	FILE *fp;
	int line = 0;
	double starttime = getUTimeDouble();
	int loopc=0;
	
	rp_init(9002 , 100 );

	fp = fopen( "in.txt" , "r");
	if( fp == NULL )return 0;

	

	fd = rp_open( inet_addr("192.168.1.61") , htons( 9003));
	fprintf( stderr , "Open! fd = %d\n" , fd );

	
	while(1){
		if( fgets( buf,sizeof(buf),fp) == 0 )break;
		line++;
		loopc++;


		
		while(1){
			extern int current_send_i;
			if( rp_send( fd , buf ,strlen(buf))== strlen( buf)){
				
				fprintf( stderr , "line %d sent %dbytes seq:%d\n",
						line,strlen(buf) , current_send_i );
				rp_proc();
				break;
			} 
			rp_proc();
		}
	}

	rp_close(fd);

	while(1){
		rp_proc();
		if( rp_isclosed( fd))break;
	}
	
	fprintf( stderr , "TIME: %f msec\n" ,
			( getUTimeDouble() - starttime ) / 1000.0 );


	return 0;
}


