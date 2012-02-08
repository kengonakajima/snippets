#include <stdio.h>

int main()
{
	int i;

	FILE *fp;

	
	for(i = 0 ; i < 20000 ; i++ ){
		fp = fopen( "log" , "a");
		fprintf( fp , "ahoboke%d\n%d\n" ,i,i);
		fclose( fp );

	}

	
	
}
