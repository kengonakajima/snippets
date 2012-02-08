#include <stdio.h>

#include "udptcp.h"

#define MSG "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"


int main()
{

	int fd;
	int r;

	uinit( inet_addr("192.168.1.61") );
	
	fd  = usocket( );
	fprintf( stderr , "start con fd:%d\n" ,fd);
	
	r = uconnect( fd, inet_addr( "192.168.1.62") , htons(50) );
	fprintf( stderr, "Fd: %d connected! r:%d\n", fd , r );

	r = uwrite( fd , MSG , strlen(MSG) );
	fprintf( stderr , "wrote %d bytes\n" , r );

	while(1){
		uproc();
		if( ureadable( fd ) ){
			char buffer[100+10];
			int r;
			bzero(buffer,sizeof(buffer));
			r = uread( fd, buffer , sizeof( buffer) );
			fprintf( stderr , "read %d byte: [%s]\n",r , buffer );
			
		}
	}
	

	
}
