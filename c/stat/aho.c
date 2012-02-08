#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

main()
{

	struct stat sbuf;
    int r;
	r = stat( "fud" ,  &sbuf );

    printf("R:%d\n" , r);
	printf( " size: %d\n" , sbuf.st_size );

	printf(" %d\n" , S_ISDIR( sbuf.st_mode));
    
}
