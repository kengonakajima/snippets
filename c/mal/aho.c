#include <malloc.h>
#include <stdio.h>

main()
{

	int i;
	char *p;
	
	for(i=0;;i++ ){
		fprintf( stderr,"%d\n" , i  );
		p = malloc( 10240 );
		if( p == NULL ) break;
	}
	
}
