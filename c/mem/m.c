#include <stdio.h>
#include <malloc.h>


int main(int argc , char **argv )
{
	int i;
	char *p;
	
	p = malloc( atoi( argv[1]) );
	if( p == NULL ) printf("fuck");

	sleep(1000);
}
