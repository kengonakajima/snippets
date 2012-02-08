
#include <sys/types.h>
#include <stdio.h>


int
main()
{

#if (defined linux) && (FD_SETSIZE < 1024 )
#error "fuck?"
#endif
    
	fd_set fds;


	printf( "%d  %d\n", sizeof(fds), FD_SETSIZE );

	
}
