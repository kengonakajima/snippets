#include <stdio.h>

/*
	$B%3%^%s%I$N%P%$%J%j$r7h$a$k$?$a$K!"J8;z$N(B
	$BI=$r=PNO$9$k!#(B
	

*/
void main()
{
	int i;

	for(i = 33 ; i< 128; i++ ){
		printf("%d %c " ,i , i );
		if( (i & 7) == 0) printf("\n");
	} 
	    
	printf("\n");



}
