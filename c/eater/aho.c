#include <malloc.h>
#include <stdio.h>


main()
{
	char *mem;
	int i;
	
	mem = (char *)malloc( 400*1024*1024 );
	for(;;){
		i++;
		mem[300*1024*1024]=i;
	}
	
}
