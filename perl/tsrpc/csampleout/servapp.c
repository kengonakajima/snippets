
#include "tsrpc_server.h"

int main( int argc, char **argv )
{
	SERVER *s = tsrpcCreateServer( "lsdev" , 10010 , NULL );

	tsrpcServerMainLoop( s );

	/* not reached */
}
