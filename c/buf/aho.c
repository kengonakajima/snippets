#include <stdio.h>
main()
{

	FILE *fp;

	fp = fopen( "aho" , "a");
	for(;;){
		int k;
		sleep(1);
		for(k = 0 ; k < 200 ; k++ ){
			fprintf(fp , "aaaaabbbbbcccccdddd\n" );
		}
	}
	
}


