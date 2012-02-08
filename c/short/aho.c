#include <stdio.h>
#include <time.h>
#include <sys/time.h>


int a[1000*1000];
unsigned short b[1000*1000*2];
char c[1000*1000*4];

double getUTimeDouble(void)
{

	struct timeval tm;

	gettimeofday( &tm , NULL );


    return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}




main()
{
	double t;
	register int i;
	
	t = getUTimeDouble();
	for(i=0;i<1000*1000;i++){
		a[i]=i;
	}
	fprintf( stderr , "%f\n" , getUTimeDouble() - t );

	t = getUTimeDouble();
	for(i=0;i<1000*1000;i++){
		b[i*2]=i;
	}
	fprintf( stderr , "%f\n" , getUTimeDouble() - t );

	t = getUTimeDouble();
	for(i=0;i<1000*1000;i++){
		c[i*4]=i;
	}
	fprintf( stderr , "%f\n" , getUTimeDouble() - t );
	
}
