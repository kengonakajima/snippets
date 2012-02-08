

void cutLineFromCharArray( char *src , char *dest )
{

	int i;
	int l;

	for(i  = 0 ; ;i++){

		dest[i] = src[i];
		if( src[i] == '\n' ) break;
		if( src[i] == '\0' )break;
	}
	dest[i+1] = '\0';

	l = strlen( dest );
	for(i = 0 ;  ;i++){
		src[i] = src[i+l];
		if( src[i+l] == '\0')break;

	}
}


int pch( char *lin)
{
	int i;
	char tmpbuf[1000];
	char blankbuf[1000];
	
	if( strlen(lin) >=1000)return 0;
	
	cutLineFromCharArray( lin , tmpbuf );
	cutLineFromCharArray( lin , blankbuf );
	cutLineFromCharArray( lin , tmpbuf );
	printf( tmpbuf );

	if( strlen( tmpbuf ) > 4 && strlen(blankbuf) <=2) return 1;

	return 0;
	
}

int main(int argc , char **argv )
{

	char aho[1000]="POST / HTTP/1.0\n\nasdfasdf=asdfas\n";

	char tmp[100];

	printf( "%d\n" , pch( aho ));
}
