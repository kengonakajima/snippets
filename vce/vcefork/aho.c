#include "vce.h"

#include <unistd.h>


tcpcontext_t tcp;

int callback_a( conn_t c )
{
    vce_errout( "aho\n" );
    
    return 0;
}


int callback_p( conn_t c, char *d, int len )
{
    vce_errout( "hoge: %d pid=%d\n",len, getpid() );

    if( d[0] == 'f' ){
        if( fork() == 0 ){
            vce_errout( "child\n" );
            execl( "/bin/ls", "/",0 );
        } else {
            vce_errout( "parent\n" );
        }
    } 
    
    return 0;
}

                
int main( int argc, char **argv )
{

    vce_initialize();

    tcp = vce_tcpcontext_create(
        1, NULL, 11113, 10,
        1024,1024, 99999,
        VCE_BLOCK_CIPHER_NONE, 0,
        0,0,0 );
    if(!tcp)return 1;

    vce_tcpcontext_set_conn_acceptwatcher( tcp, callback_a );
    vce_tcpcontext_set_conn_parser( tcp,
                                    vce_protocol_parser_through,
                                    callback_p );
    while(1){
        vce_heartbeat();
    }
    return 0;
}

    
