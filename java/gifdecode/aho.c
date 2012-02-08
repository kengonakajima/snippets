#include <stdio.h>
main()
{
	FILE *fp;
	fp = fopen("nazobutton.gif" , "r");
	while( 1)
	{
		int f;
		f = fgetc (fp );
		printf( "%d," , f );
	}
	fclose(fp);
	
}
