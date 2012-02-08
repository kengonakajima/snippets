#include <stdio.h>
#include <errno.h>

int main()
{
	char buf[10];
	
	int n = read( 88 , buf , 10);

	perror( "aho");
	
}
