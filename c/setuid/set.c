#include <stdio.h>


#include <sys/types.h>


int main( int argc , char **argv )
{
	FILE *fp;
	
	/* setuid( 502 );  */

	fp = fopen ( "/home/ringo/test/setuid/aho" , "w" );
	fprintf( fp , "asdfasdfaskkdj" );
	fclose( fp );
}
