#include <stdio.h>
main()
{
	char *aho;
	int i,k;
	
	aho = (char *)malloc( 1024 * 1024 * 20 );

/*	for(k = 0 ; ; k++) {
		
		for(i = 0 ; i < 1024 * 1024 * 20 ;i++ ){
			aho[i] = k;
		}

	}
	*/
	k = 0;
	while(1)
	{
		k++;
		sleep(1 );
		aho[400] =k; 
	}
	
}
