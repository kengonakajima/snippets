/*
  最小のクライアント。暗号を使用
  
 */

#include "vce.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int counter = 0;

int callback( conn_t ct, char *data, int len )
{
    vce_errout( "callback: %d counter:%d\n",len,counter);
    sleep(1);
    counter++;
	return 0;
}

int main( int argc,char **argv )
{
	tcpcontext_t t;
	conn_t c;

	vce_initialize();
    vce_set_verbose_mode(1);
	t = vce_tcpcontext_create( 1, NULL, 0, 1,
                               10000,10000,100,0,0,0,0,0 );
	if( !t ){
		vce_errout( "cannot create tcpcontext\n" );
		return 1;		
	}
	vce_tcpcontext_set_conn_parser( t, vce_protocol_parser_bin16, callback );
	c = vce_tcpcontext_connect( t, "localhost", 8001 );
	if( !vce_conn_is_valid( c )){
		vce_errout( "cannot connect to server\n" );
		return 1;
	}
	while(1){
		vce_heartbeat();
        if( vce_conn_writable( c ) <= 0 ){
            vce_errout( "connection is fucked\n");
            break;
        }
	}
	vce_finalize();
	return 0;
}
