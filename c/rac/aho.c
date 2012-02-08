#include "stdio.h"
extern int errno;

main()
{
	FILE *fp;
	int i;

	
	fp = fopen("hoge" , "r+" );
	if( fp == NULL )exit(-1);

	for(i = 0 ; i < 10000 ; i++){
		int r = rand()&0xffff;
		fseek( fp , i*4 , 0 );
		if(fwrite( &r , 4 , 1, fp )==0){
			fprintf( stderr,"%d\n" ,errno);
		}
	}

	fclose(fp);
	
}
