/***************************
  $BJ8;zNs$r%(%9%1!<%W$9$k!#%(%9%1!<%W$5$l$k$Y$-5-9f$O!"(B

  0x20 $B%9%Z!<%9(B
  0x22 "
  0x3d =
  0x5c \
***************************/
void escapeString( char *src , char *dest )
{
	int sc=0,dc=0;

	for(sc=0,dc=0; ;sc++ ){
		if( src[sc]== 0x20 || src[sc]== 0x22 || src[sc]== 0x3d ||
		   src[sc]==0x5c ){
			dest[dc++] = '\\';
			dest[dc++] = src[sc];
		} else {
			dest[dc++] =src[sc];
		}
		if( src[sc] == '\0' )break;
	}
}



main()
{

	char t[] = "I am a boy. I said \"fuck you\"";
	char new[1111];

	escapeString( t , new );
	printf( new );
	
}
