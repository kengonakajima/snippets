#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

extern int errno;
main()
{
	int fd;
	caddr_t ca;
	caddr_t base;
	off_t off;
	fd = open("mem" , O_RDWR );

	ca = mmap( base , 20 , PROT_READ | PROT_WRITE , MAP_SHARED ,  fd , off );

	printf("%x %x %x E%d\n" , ca , base , off ,errno);
	perror("");

	printf("%x\n" , ca[0] );
}
