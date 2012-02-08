#include <stdio.h>

void main()

{
	
	int *tmp;
	int i;
	
	tmp = aho( 100);
	for(i = 0 ;i < 10 ; i++ ){
		printf("%d " , tmp[i]);
	}
}
int *aho( int i)
{
	int ret[10];
	for(i = 0 ; i < 10 ; i++ ){
		ret[i] = i;
	}
	return ret;



}








