#include <stdio.h>
#include <time.h>
#include <sys/time.h>

main()
{
	double t;

	struct timeval tm;

	gettimeofday( &tm , NULL );

	t = (double)( tm.tv_usec ) + (double) tm.tv_sec*1000000;

	printf( "%f\n" , (float)t );
}
