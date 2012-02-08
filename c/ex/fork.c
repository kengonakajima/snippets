#include <stdio.h>
main()
{
	int i;
	int d=808;
	
	i = fork();

	fprintf(stderr,"%d %d\n" , i ,d);
	if( i == 0 ) d = 909;
	fprintf(stderr,"%d %d\n" , i ,d);
}
