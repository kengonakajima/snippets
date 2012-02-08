#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double
vce_utime_diff( struct timeval *before, struct timeval *after )
{
    return 1000000.0*(after->tv_sec - before->tv_sec) +
        (after->tv_usec - before->tv_usec);
    
}


typedef struct _rect
{
	int x1,y1,x2,y2;
} rect;

int num = 10000;

double total_loop_find_counter = 0;

rect *r;

/* 1個みつけるとやめてしまう。 */
void
simple_rect_search( int x1, int y1, int x2, int y2 )
{
	int i;
	for(i=0;i<num;i++){
		if( r[i].x1 <= x2 && r[i].x2 >= x1 &&
			r[i].y1 <= y2 && r[i].y2 ){
			total_loop_find_counter += i;
			break;
		}
	}
}

int
main()
{
	struct timeval start_time , end_time;
	
	int i;

	r = malloc( sizeof(rect) * num );
	if( !r ) return 1;


	///////////	///////////	///////////	///////////	///////////	
	gettimeofday( &start_time, NULL );
	for(i=0;i<num;i++){
	}
	gettimeofday( &end_time, NULL );
	printf( "pure %d * 2 random : %f us\n", num,
			vce_utime_diff( &start_time, &end_time ) );


	///////////	///////////	///////////	///////////	///////////
	gettimeofday( &start_time, NULL );
	for(i=0;i<num;i++){
		r[i].x1 = random() % 100000;
		r[i].y1 = random() % 100000;
		/* この30をかえるとサイズもかわるしヒット率もかわる */
		r[i].x2 = r[i].x1 + ( random() % 100 ) + 1; 
		r[i].y2 = r[i].y1 + ( random() % 100 ) + 1;
	}
	gettimeofday( &end_time, NULL );
	printf( "make %d rect : %f us\n", num,
			vce_utime_diff( &start_time, &end_time ) );

	///////////	///////////	///////////	///////////	///////////
	gettimeofday( &start_time, NULL );	
	for(i=0;i<num;i++){
		int x1 = random() % 100000;
		int y1 = random() % 100000;
		int x2 = x1 + random() % 30;
		int y2 = y1 + random() % 30;
		
		simple_rect_search( x1,y1,x2,y2 );
	}
	gettimeofday( &end_time, NULL );
	printf( "simple search %d rect : %f us (%f loop avg)\n", num,
			vce_utime_diff( &start_time, &end_time ),
			(double)total_loop_find_counter / num
			);	
	return 0;
}
