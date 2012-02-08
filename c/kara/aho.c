#include <stdio.h>
int main()
{
	int i=0;


	while( 1 ){
		i++;
		if( ( i % 4000000 ) == 0 ){
			fprintf( stderr , "." );
		}
	}
}
