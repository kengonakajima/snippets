#include "vce.h"
#include <string.h>
#include <unistd.h>

#define HOGE_LEN 1234

#define PORT 9001
#define MAXCLI 100
circ_t clients[MAXCLI];
tcpcontext_t t;

static int callb( circ_t c, char *d, int l )
{
    return 0;
}
static int acceptw( circ_t c )
{
    int i;
    vce_errout( "ACCEPTED\n");
    for(i=0;i<MAXCLI;i++){
        if( !vce_circ_is_valid( clients[i] )){
            clients[i] = c;
            return 0;
        }
    }
    return -1;
}
static int closew( circ_t c )
{
    int i;
    vce_errout( "CLOSED\n");
    for(i=0;i<MAXCLI;i++){
        if( vce_circ_is_equal( clients[i], c ) ){
            vce_circ_clear( & clients[i] );
            return 0;
        }
    }
    return 0;
}



void try_to_send( void )
{
    int i;
    for(i=0;i<MAXCLI;i++){
        int wa = vce_circ_writable( clients[i] );
        if( wa > 0 ){
            char hoge[HOGE_LEN];
            vce_errout("writable: %d\n",wa);
            memset( hoge, 'a',sizeof(hoge));
            hoge[sizeof(hoge)-1]=0;
            if( vce_vecprotocol_unparser_bin16(
                clients[i], hoge,strlen(hoge))  < 0 ){
                vce_errout( "cannot send : %s\n",STRERR );
            } else {
                vce_errout(".");
            }
            usleep( 1000 * 50 );
        }
    }
}

int main( int argc, char **argv )
{
    vce_initialize();
	vce_set_verbose_mode(1);

    t = vce_tcpcontext_create( 1, "0.0.0.0",  PORT,
							   1,
                               65536,65536,
							   10000,
                               VCE_BLOCK_CIPHER_BLOWFISH,8,
                               0,0, 1000);
    
	if( !t ){
		vce_errout( "nogen_vecsv: cannot create tcpcontext: %s\n",STRERR);
		return 1;
	}

	vce_tcpcontext_set_conn_parser( t,
									vce_protocol_parser_vecmain_server,NULL);
	vce_tcpcontext_set_circ_parser( t,
									vce_vecprotocol_parser_bin16,
									callb );

	if( vce_vec_init_mainserv( t, MAXCLI, 8192 ) < 0 ){
		vce_errout( "nogen_vecsv: cannot init vecarray\n" );
		return 1;
	}
    vce_tcpcontext_set_circ_closewatcher( t, closew );
    vce_tcpcontext_set_circ_acceptwatcher( t, acceptw );
	vce_set_heartbeat_wait_flag(1);

    while(1){

        vce_heartbeat();

        try_to_send();

    }
    return 0;
}
