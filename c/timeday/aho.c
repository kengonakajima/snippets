#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

main()
{
	struct timeval tv;

	int i;

	for(i = 0 ; i < 10000 ; i++){
		usleep(0);
		gettimeofday( & tv , NULL );
		fprintf(stderr,"%d\n" , tv.tv_usec );
	}
	
}
