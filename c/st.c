#include <stdio.h>

typedef struct aho {
	int i;
	int j;
} a ;
main()
{
	a boke;
	a kasu;


	boke.i = 10;
	boke.j = 20;

	kasu = boke;

	printf("%d %d " , kasu.i , kasu.j );
}
