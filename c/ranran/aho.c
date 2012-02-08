#include <stdio.h>



double rangeRand( double mid , double range )
{
	double sign=1.0;
	long l = random();
	double r1 , r2,r3;
	
	if( (l & 01 )==0 ) sign*= -1.0;	/* まず最下位ビットを符号として使う。 */
	
	r1 = (double)((l & 0xff000000L)>>24 )/(double)( 256 );
	r2 = (double)((l & 0x00ff0000L)>>16 )/(double)( 256 );
	r3 = (double)((l & 0x0000ff00L)>>8 )/(double)( 256 );
	if( range < 0 ) range = -range;	

	range = range * ( 1-r1 ) * (1-r2) *(1-r3);
	
	return mid  + range*sign ;
	
}

int count[100];

main()
{

	int tb[100000];

	int i;
	for(i=0;i<100000;i++){
		tb[i] = (int)rangeRand( 50,50);
		count[tb[i]]++;
	}
	for(i=0;i<100;i++){
		int j;
		for(j=0;j<count[i]/100;j++)	printf("*");
		printf("\n");
	}
}
