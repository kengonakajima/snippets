#include <signal.h>
#include <sys/types.h>
#include <stdio.h>


void aho(int i)
{
	fprintf( stderr , "hoge: %d\n" ,i);


}

void hogehoge(int i)
{
	fprintf( stderr , "alarm\n" );

	alarm(1);

	signal( SIGALRM , hogehoge );

	
}

main()
{
	
	signal( SIGALRM , hogehoge );

	alarm(1);
	while(1){
		sleep(1);
		fprintf( stderr, ".");
		;
	}
}
