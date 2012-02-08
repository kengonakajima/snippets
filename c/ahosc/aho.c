#include <stdio.h>


main(int ar , char **g)
{
	int i;

	printf("int main(){ }\n" );

	for(i=0;i<atoi(g[1]);i++){
		printf("int aho%d(int i){ printf(\"ah%d\"); }\n" ,i,i);
	}
	

}
