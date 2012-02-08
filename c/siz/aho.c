#include <fcntl.h>
#include <stdio.h>

main()
{
	char buf[100000];
	int fd;

	fd = open( "/home/ringo/test/siz/a.out" , O_RDONLY );
	fprintf(stderr, "%d\n" , read( fd , buf , 100000 ) );

	

	
	
	
}
