

#include "tsrpc.h"





int main(int argc , char **argv )
{
	int i;
	CLIENT *c = tsrpcCreateClient( "localhost" , 10010 );
	if( c == NULL ){
		printf( "fuck! "  );
		return;
	}

	for( i=0;;i++){
		int a[10]  = { 1,2,3,4,5,6,7,8,9,10};
		int ret = remotesum2( c , "aho" , a );

		printf( "remotesum2 returns %d\n" , ret );


		printf( "FUCK\n" );
/*		memorymap(0);   HP-UXでしかないのだ */
	}

	tsrpcDestroyClient( c );
}
