

#include <stdio.h>


#include <pthread.h>		/* needed for MT */
#include <sched.h>			/* needed for MT */
#include <signal.h>

int counter =0;


void hook( int i )
{
	fprintf( stderr , "fuck signal" );
}

void * start_funk( void * a)
{

	int i=0;
	char buf[2343];

///	signal( SIGINT , hook );
	fprintf( stderr ,"child started.\n" );



	
	
	while (1){
		
		counter++;
		i++;

//		fprintf( stderr , "child: %d %d [%s]\n" ,  counter ,i , a);
//		usleep(1);

	}

	fprintf( stderr , "child end\n" );
	

}

int main()
{


	pthread_t newthread_id;
	pthread_attr_t newthread_attr;
	int i=0;
	char ppp[]="This is an arguement." ;

	signal( SIGINT , hook );


	fprintf( stderr , "parent start" );
	
	pthread_attr_init( &newthread_attr );
	pthread_create( &newthread_id , &newthread_attr , start_funk ,(void *)ppp);




	while(1){

		usleep(1);
		counter++;
		i++;
		fprintf( stderr , "main: %d %d \n" , counter ,i);
	}
	
}


