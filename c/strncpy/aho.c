#include <string.h>

int main()
{
	char b[10]={'k','k','k','k', 'k','k','k','k', 'k','k' };
	char s[] = "asd";
	int i;
	
	strncpy( b , s ,strlen( s ) );
	for(i=0;i<10;i++)
	printf( "%d " , b[i] );
	
	
}
