#include <stdio.h>


#include <pthread.h>		/* needed for MT */
#include <sched.h>			/* needed for MT */

int counter = 0;

void *start_func( void *a)
{

	while(1){
		sleep(1);
		counter++;
		sched_yield();
	}
}
	
int main(int argc, char **argv )
{
	int tno = atoi( argv[1]);
	pthread_t pths[1000];
	pthread_attr_t pthas[1000];
	int i;
	void *arg;

	for(i=0;i<tno;i++){
		pthread_attr_init( &pthas[i] );
		pthread_attr_setscope( &pthas[i] , PTHREAD_SCOPE_SYSTEM );
		
		fprintf( stderr , "thcr returns: %d\n" ,
			pthread_create( &pths[i] , &pthas[i] , start_func ,arg ) );
				
	}
	while(1){
		sleep(1);
		fprintf( stderr , "counter: %d\n" , counter );
		sched_yield();
	}
}



/*

   PTHREAD_SCOPE_SYSTEM often causes too-high load for system if
   too many threads are used.

   To achieve better performance , be sure to use PTHREAD_SCORE_PROCESS
   and sched_yield() function. this allows system to schedule execution
   context within a process, so less context-switching gets CPU power.

   for example, program 'top' showed as follows:
   
   PTHREAD_SCOPE_SYSTEM:  system:1000% a.out: 4%
   PTHREAD_SCOPE_PROCESS: system:10% a.out:60%

   according to this table , we'll know the CPU cost of context-switching.
   
*/



