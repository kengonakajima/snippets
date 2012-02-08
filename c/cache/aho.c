#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int a[512][512];

double getUTimeDouble(void)
{

	struct timeval tm;

	gettimeofday( &tm , NULL );


    return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}




main()
{
	register int i;
	register int j;
	register int k;
	
	double t ,tt;

	t = getUTimeDouble();
	for(k=0;k<100;k++){
		for(i=0;i<512;i++){
			for(j=0;j<512;j++){
				a[i][j]=j;
			}
		}
	}
	fprintf( stderr , "%fms\n" , (getUTimeDouble() - t)/1000 );

	t = getUTimeDouble();
	for(k=0;k<100;k++){
		for(i=0;i<512;i++){
			for(j=0;j<512;j++){
				a[j][i]=j;
			}
		}
	}
	fprintf( stderr , "%fms\n" , (getUTimeDouble() - t)/1000 );

	
}
