#include <signal.h>
#include <sys/signal.h>
#include <stdio.h>

void aho(int hoge)
{
	
	signal( SIGALRM , aho );
	
	//alarm(1);
	
	usleep(1200*1000 );

	printf("hoge\n");
	kill( getpid() , SIGALRM );
	
}


main()
{
	char buf[100];
	
	signal( SIGALRM , aho );
	kill( getpid() , SIGALRM );
	


	while( 1){
		if(fgets( buf , sizeof( buf ),stdin)==0)continue;
		printf( buf );
	}
	
	
	
}
