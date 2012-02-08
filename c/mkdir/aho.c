#include <stdio.h>
#include <unistd.h>


main()
{
	fprintf( stderr , "%d\n" , mkdir( "moomoo/mo" ,0 ) );
	fprintf( stderr , "%d\n" , mkdir( "hoge" ,0755 ) );

	fprintf( stderr , "%d\n" , mkdir( "/home/ringo/hoge2" ,0 ) );
}
