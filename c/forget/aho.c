#include <malloc.h>
#include <stdio.h>


main()
{
	char *mem;
	int i;
#define BUFS 80*1024*1024
	mem = (char*) malloc( BUFS );

	fprintf( stderr,"start");
	for(i=0;i<BUFS;i++){
		if( (i%(1024*1024))==0 )fprintf(stderr,"#");
		mem[i]=i;
	}
	fprintf( stderr,"end");
	
	sleep(20);
	fprintf( stderr,"free..");
	free( mem );
	fprintf( stderr,"freed");
	sleep(2000);
}
