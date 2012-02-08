#include <stdio.h>

/*
	コマンドのバイナリを決めるために、文字の
	表を出力する。
	

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
