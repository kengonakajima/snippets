#include <stdio.h>


main()
{
	FILE *fp ;
	char u = 10;
	
	fp = fopen( "boke" , "w");

	fwrite( &u , 1 , 1, fp );
	fprintf( fp , "ahoahoa");
	fwrite( &u , 1,1,fp );
	fprintf( fp , "a");
	fclose( fp );
	
}
