#include <stdio.h>

main()
{

	int a = 10;
		
	int aho( int i)
	{
	
		return i*2 + a;
	}
	int boke( int j )
	{
		
		return a;
	}


	int j;

	j = aho ( 10 );
	printf("%d , a %d " , j ,a );
}
