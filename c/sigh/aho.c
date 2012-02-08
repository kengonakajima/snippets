#include <signal.h>


main()
{
	int i=0;
	
	while(1)
	{
		i++;
		
		sleep(1);

		if( i == 3 ) kill( getpid() , SIGBUS );
		
	}
	
}
