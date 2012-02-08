#include <time.h>

main()
{
	time_t t;
	char *aho;

	
	time( &t );

	aho = ctime( &t);
	puts(aho);
}
