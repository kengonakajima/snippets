#include <stdio.h>
#include <sys/param.h>


int main(int argc , char **argv)
{
	int i;

	if( fork() != 0 ) exit(0);
	
	/* for(i = 0 ;i < NOFILE ; i++ ) close(i); */

	chdir( "/" );
	setpgrp( 0 , getpid() );
	
	while(1)
	{
		fprintf(stderr,"aho" );
		sleep(1);
	}
	

}
