#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>


#define MEMSIZ (400*1024*1024)


double getUTimeDouble(void)
{

	struct timeval tm;

	gettimeofday( &tm , NULL );


    return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}


double total_t=0.0;
int total_count=0;

int main()
{
	char *mem;
	int i;
	double t,tt;
	int p;
	int usl = 1;
	
	if( (	mem = malloc( MEMSIZ ) ) == 0 ){
		fprintf( stderr,"fuck!" );
		return;
	}

	
	for(i=0;i<10000000;i++){
		int pp;
		int j;
		
		t = getUTimeDouble();	
		mem[ pp = (random() % MEMSIZ) ] = i;
		p = mem[ pp ];
		
		tt = getUTimeDouble();

		total_t += ( tt  - t );
		total_count ++;

		if( ( i % 1000 ) == 0 ){
			fprintf( stderr,"usec/access = %f\n" , total_t/total_count );
			total_t = 0;
			total_count=0;
		}
	}
	

	
}
