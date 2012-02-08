#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>


main( int argc , char **argv)
{
	long atol();
	static struct timeval timeout;

	if( argc != 3 ){
	    	fprintf(stderr,"usage: timer <#seconds> <#microseconds>\n");
		exit(1);
	}

	timeout.tv_sec = atol( argv[1] );
	timeout.tv_usec = atol( argv[2] );

	if( select( 0 , ( fd_set *) 0 , ( fd_set * ) 0 , (fd_set * ) 0 , &timeout ) < 0 ){
	    fprintf(stderr,"select error.\n");
	    exit(1);
	}
       	exit(0);
}
