#include <sys/time.h>
#include <stdio.h>





#include <signal.h>

int i;

void hook(void )
{
	fprintf(stderr,"i=%d\n" , i );
	exit(0);
}
int k;

void ahook( void )
{
	k++;
	fprintf(stderr,".");
	
}

void main()
{

	signal( SIGINT , hook );
	signal( SIGALRM , ahook );
	
	ualarm( 1000 * 100 );
	for( ;;  ){
		sleep( 1 );
	}
	
	
}

