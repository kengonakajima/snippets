#include <stdio.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include <dirent.h>


int main(int argc , char **argv )
{

	DIR *d;
	char dirname[1000];
	
	if( argc == 1 ) strcpy( dirname , "." );
	else strcpy ( dirname , argv[1] );
	
	d = opendir( dirname );

	for( ; ;  ){
		struct dirent *de;
		de = readdir( d );
		if( de == NULL ) break;
		printf( "<li><a href=\"%s\">%s</a>\n",de->d_name, de->d_name);
		
	}
	
	
	closedir( d );
}
