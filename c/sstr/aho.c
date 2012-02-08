#include <stdio.h>

struct big
{
	int a;
	char fuck[2000];
};

struct big b[50000];

main()
{
	int i;

	fprintf( stderr, "s\n" );
	for(i=0;i<50000;i++){
		b[i].a=i;
	}

	fprintf( stderr, "e\n" );
}
