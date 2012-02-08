#include <malloc.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>




double getUTimeDouble(void)
{

	struct timeval tm;

	gettimeofday( &tm , NULL );


    return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}



int main()
{
	char *mem;
	int i;
	
	mem = malloc( 400 * 1024 * 1024 );


	for(i=0;;i++ ){
		int j;
		double t , tt;

		t = getUTimeDouble();
		for(j=0;j<100000;j++){
			int p;
			mem[300*1000*1000] = 234;
			p = mem[200*1000*1000];
		}
		tt = getUTimeDouble();

		fprintf( stderr  ," %f\n" , tt -t );
	}
		
}
