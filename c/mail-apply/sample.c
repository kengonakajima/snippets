#include <stdio.h>
main()
{

	char buf[65536];
	FILE *fp;

	fp = fopen("/home/ringo/test/mail-apply/aho","w");
	while( fgets( buf , 65536 , stdin ) )
	{
		
		fprintf(fp , "%s" , buf );
	}
	fclose(fp);
	
}
	
