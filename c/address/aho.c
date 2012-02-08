#include <stdio.h>

int main()
{
	int *mem;

	mem = malloc( 512*512 );

	printf("%d\n" , (unsigned long)mem % 4096 );
}
