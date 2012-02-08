#include <stdio.h>

main()
{
	FILE *fp = fopen("aho.file", "a" );

	stderr = fp;

	fprintf(stderr," ahoahoaoaoahaoh" );
	
}
