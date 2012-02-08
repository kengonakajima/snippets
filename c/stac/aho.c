#include <stdio.h>

int a[1000];

int main()
{
	int b;
	char *m = (char*)malloc(1000);
	
	fprintf( stderr, "BSS: %x STACK:%x MEM:%x MAIN:%x\n" , a , &b , m ,main);
	sleep(100);
}
