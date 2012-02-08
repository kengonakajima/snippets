#include <stdio.h>

void modifyFileNameSlash( char *fname );

main(int a , char **v)
{
	FILE *fp;
	char fuck[1000];
	strcpy( fuck , v[1] );
	modifyFileNameSlash( fuck );
	printf( "name: %s\n" , fuck );
	
	fp = fopen( fuck ,"w" );


	fprintf( fp , "fuck\n" );

	fclose(fp);
}

