#include <stdio.h>

main(int argc , char **argv)
{
	int i;

	for(i=0;i<atoi(argv[1] ) ; i++ ){
		printf("%99d\n" , i  );
	}
}
