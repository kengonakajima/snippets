#include <stdio.h>

main()
{
	while(1)
	{
		sleep(1);
		fprintf(stderr,"hoge");
		fprintf(stdout,"kasu");
	}
	
}
