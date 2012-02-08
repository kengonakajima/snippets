#include <stdio.h>
main()
{
	FILE *fp;
	int i;
	


	while (1)
	{
		int k;

		
		i ++;

		for(k=0;k<100;k++){
			int kk;
			fp = fopen( "fuck" , "a+" );
			fprintf( fp , "%d\n" , k+i );
			
			for(kk=0;kk<100000;kk++);
			fclose(fp);
		}
			
		
	}


}
