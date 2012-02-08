#include <stdio.h>
int main()
{

	int i;
	for(i = 0 ;;i++ ){
		if( (i % 10000000 ) == 0 ){
			fprintf( stderr , "." );
		}
	}
}
