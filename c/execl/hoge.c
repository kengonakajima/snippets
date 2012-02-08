#include <stdio.h>
char buf[1000];
main(){
	while( fgets( buf,4,stdin ) ){
		printf( buf );
	}
}
