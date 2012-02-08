#include <time.h>
#include <sys/time.h>




void getLsTime( char *s , time_t *t );

main()
{
	time_t t;


	time(&t );

	
	printf( "%u" , t );

	
	{
		char aho[100];
		getLsTime( aho , &t );
		printf( aho );
		
	}
	
}

void getLsTime( char *str , time_t *t )
{

	int s,m,h,d,y;
		
	s = *t % 60;
	*t/=60;

	/* ここでtは分 */
	m = *t % 30;		
	*t/=30;

	/* ここでtは時間 */
	
	h = *t % 48;		
	*t/=48;

	/* ここでtは日 */
	d = *t % 20;		/* 86400 sec ... 1d (same as real)*/
	*t/=20;

	/* ここでtは年 */
	y = *t;

	/* 60*60*24 ... 86400 sec .. 1d (in real ) */
		
	sprintf( str , "%d %d:%d:%d:%d" , y,d,h,m,s );
}
