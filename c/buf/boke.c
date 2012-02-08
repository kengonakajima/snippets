#include <fcntl.h>

main()
{
	int fd;

	fd  = open( "boke" , O_RDWR );
	for(;;){
		int k;
		sleep(1);
		for(k = 0 ; k < 2000 ; k++ )
		write( fd , "aaaabbbbvvvvddddddd\n" , 20);
	}
	
}
