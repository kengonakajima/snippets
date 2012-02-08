#include <stdio.h>


#include <pthread.h>		/* needed for MT */
#include <sched.h>			/* needed for MT */


/*

   To compile this program , you need to add

   -lpthread -lposix4

   compiling options.
   
   These man commands will help you:
   
   man threads
   man pthread_create
   
*/

int counter = 0;


void * start_funk( void * a)
{

	pthread_t me;
	int i = 0;
	me = pthread_self();
	
	while(1){
		i++;
		
		if( i % 10000000 == 0 ){
			counter++;
			fprintf( stderr ,"child: yield: counter:%d pthread_t:%x\n" ,counter, me);
			sched_yield();
		}
	}
	
}



int main(int argc , char **argv )
{
	pthread_t newthread_id;
	pthread_attr_t newthread_attr;
	void *arg;
	int ret;
	
#if 0
	
	/* we can also use code below */
	pthread_attr_init( &newthread_attr );
	pthread_create( &newthread_id , newthread_attr , start_funk , arg );
	
#endif

	pthread_attr_init( &newthread_attr );
#if 1
	/*
		When PTHREAD_SCOPE_PROCESS is used, scheduling is up to this
		program itself.
	*/
	pthread_attr_setscope( &newthread_attr , PTHREAD_SCOPE_PROCESS );
#else
	/*
	    When PTHREAD_SCOPE_SYSTEM is used , self scheduling is not needed.
	*/		
	pthread_attr_setscope( &newthread_attr , PTHREAD_SCOPE_SYSTEM );
	
#endif
						  
	ret = pthread_create( &newthread_id , &newthread_attr , start_funk ,arg);
	
	fprintf( stderr , "main thread: %d\n" ,ret );

	
	while(1){
		pthread_t me;
		me = pthread_self();
		
		fprintf( stderr , "main: counter:%d pthread_t:%x\n" ,counter,me);
		sleep(1);
		counter++;
		sched_yield();
	}
	 
}

