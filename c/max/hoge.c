#include <stdio.h>


int main()
{
	FILE *fp;
	char nm[2345];

	int i;


	for(i=0;i<23453;i++){
		sprintf( nm , "aho%d" , i );
		fp = fopen( nm , "w" );
		if( fp == NULL ){
			extern int errno;
			fprintf( stderr , "FGUCK: %d\n" , errno );
			exit(0);
		}
	}
	
}
