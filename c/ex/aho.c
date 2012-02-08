#include <stdio.h>
#include <signal.h>


int aho(int i)
{
	fprintf( stderr,"ahoahhoahoaoa\n");
}

main()
{
	char line[1000];

	signal(SIGINT , aho);

	while( fgets(line,1000,stdin)){

		fprintf(stderr,"%s" , line );

	}
	
}
