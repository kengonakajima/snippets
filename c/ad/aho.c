#include <stdio.h>

struct aho
{
	int aho;
	int hoge;
};

struct aho a[10];
	
main()
{
	
	int i;
	for(i=0;i<10   ;i++){
		a[i].aho = i;
	}
	
	++a[4].aho;

	for(i = 0 ;i<10;i++){
		fprintf( stderr,"%d\n" , a[i].aho );
	}

	
}
