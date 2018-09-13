#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main()
{

	struct stat sbuf;
    int r;
	r = stat( "aho.c" ,  &sbuf );

    printf("R:%d\n" , r);
	printf( " size: %lld\n" , sbuf.st_size );

	printf(" %d\n" , S_ISDIR( sbuf.st_mode));
    printf(" mtime: %lld\n", sbuf.st_mtime );    
    
}
