#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void hook(void )
{
	int aho;
	wait( &aho );
	printf("%dn" , aho );
}
main()
{
	int i;

	i = 200;

	signal( SIGCHLD , hook);
	if( fork() == 0 ){
		printf("%d\n" , i );
		sleep(10);
		
	} else {
		i = 30000;
		printf("%d\n" , i );
		sleep(20);
	}
		
	if( i == 200 )printf("ahoahoa\n"); else printf("hogehoge");

}
