#include <signal.h>


void b(int i)
{
	signal( SIGSEGV , b );
}

main()
{

	int *i;

	signal( SIGSEGV , b );
	for(;;){
		*i++ = 20;
	}
	
}
