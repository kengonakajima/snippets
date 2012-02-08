#include <stdio.h>
main()
{
	FILE *fp;
	fp = fopen("aho/" , "r" );
	printf("%d\n" , fp );
	fclose(fp );


	fp = fopen("bb" , "r" );
	printf("%d\n" , fp );
	fclose(fp );

}
