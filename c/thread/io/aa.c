

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

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

	int i=100000;
	char buf[2343];

	fprintf( stderr ,"child started.\n" );
	
	while (1){
		int fd;
		char s[134];
		
		counter++;
		i++;

		fd = open( "/lifestorm/hoge.txt" , O_RDWR );
		if(  fd < 0 ){
			fprintf( stderr , "fuck " );
			continue;
		}

		sprintf( s ,"%d\n" , i );
		write( fd, s ,strlen( s ) );
		fprintf( stderr , "C%dFD%d " , i ,fd);
		close( fd );
	}

	fprintf( stderr , "child end\n" );
	

}

int main()
{


	pthread_t newthread_id;
	pthread_attr_t newthread_attr;
	int i=0;
	char ppp[]="This is an arguement." ;

//	signal( SIGINT , hook );


	fprintf( stderr , "parent start" );
	
	pthread_attr_init( &newthread_attr );
	pthread_create( &newthread_id , &newthread_attr , start_funk ,(void *)ppp);




	while(1){

		int fd;
		char s[194];
		fd = open( "/lifestorm/baka.txt" , O_RDWR );
		if( fd < 0 ){
			fprintf( stderr, "shit " );
			continue;
		}
		sprintf( s , "%d\n" ,i );
		write( fd , s , strlen( s ) );
		fprintf( stderr, "P%dFD%d " , i ,fd);
		close( fd );
		counter++;
		i++;
		
	}
	
}


