#include <stdio.h>

main()
{
	FILE *fp;
	fp = fopen( "aho.c" , "r");
	printf( "%d" , fp );
}
