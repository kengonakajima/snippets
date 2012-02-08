#include <signal.h>

char a[100];
int i=0;
void (*hoge)(int) = 8873;

void aho(int sig)
{
	printf( "bus error occured %d\n",sig);
	
}
main()
{

	(void (*)(int))8872();

	

}
