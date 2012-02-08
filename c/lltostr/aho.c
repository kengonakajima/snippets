#include <stdio.h>
#include <stdlib.h>

int main()
{
	char buf[1000];
	char*h ;

	h = lltostr( 1000 , buf );


	printf( "%s\n" , h);
}
