#include <signal.h>
#include <stdio.h>
#include <sys/types.h>


int sig(int i)
{
	int k;
	fprintf(stderr,"signal val=%d\n" , i );
	if( (k =  fork() ) == 0 ){
		fprintf( stderr,"child fork\n");
		execl("a.out" , "a.out" , NULL );
		
	} else {
		if( k == -1 ){
			fprintf(stderr,"fork error.\n");
		}
		while(1){
			sleep(1);
			fprintf(stderr,"$");
		}
		
		
	}
	
	
	
}

main()
{
	
	char *p = 0;
	fprintf(stderr,"PID:%d\n" , getpid());
	
	signal( SIGSEGV , sig );
	fprintf(stderr,"hooked.\n");
	*p = 4;

	while(1){
		sleep(1);    
		fprintf(stderr,"*");
	}
	
}

