#include <stdio.h>


/***************
  $B%(%9%1!<%W%7!<(B  $B%1%s%9$KBP1~$7$?J8;z%+%&%s%H!#(B

  char * buf ... $BJ8;zNs(B
  int escc ..... $B%(%9%1!<%W5-9f(B
  int c ........ $B$5$,$7$?$$J8;z(B
  int n ........ $B%+%&%s%H?t(B
  
***************/
int countCharEscaped( char *buf , int escc , int c ,int n)
{
    int i;
    int count = 0;
    for(i = 0 ; ;i++){
		if( buf[i] == '\0' ) break;
		if( buf[i] == c && i > 0 && buf[i-1] != escc ){
			count++;
			if( count == n ) return i;
		}
    }

    return -1;
}


/******************
  $B%(%9%1!<%W5-9f$KBP1~$7$?%H!<%/%J%$%6(B

  $B%(%9%1!<%W$5$l$k$Y$-5-9f$O!"(B

  0x20 $B%9%Z!<%9(B
  0x22 "
  0x3d =
  0x5c \

  $B%(%9%1!<%W5-9f$O!"(B \$B$G!"$=$l<+BN$r$"$i$o$9$K$O!"(B\\$B$H$9$k!#(B

  $BJ8;z(Bc$B$G6h@Z$i$l$?(Bcount$B8DL\$N%H!<%/%s$r(Bdest$B$K%3%T!<$9$k!#(B0$B8DL\$+$i$O$8$^$k!#(B
  dest$B$O$9$G$K3NJ]$5$l$F$$$kNN0h$r;X$9%]%$%s%?!#(B
  return value .... 0  $BL5;v$K%H!<%/%s$,$_$D$+$C$?!#(B
                    <0 Error
******************/
int getEscapedToken( char *src , int c , int count , char *dest )
{
	int en=0,sn=0;
	int i=0,ii=0;
	
	if( count == 0 ){
		sn = 0;
		en = countCharEscaped( src , '\\' , c , 1 );
	} else if( count > 0 ){

		sn = countCharEscaped( src , '\\' , c , count );
		en = countCharEscaped( src , '\\' , c , count+1 );

		if( en < 0 ) en = strlen( src );
		if( sn < 0 ) return -1;
		sn++;

	} else {
		return -2;
	}

	ii=0;
	for(i=sn;i<en ; i++ ){
		
		if( src[i] == '\\' && ( src[i+1] == 0x20 || src[i+1] == 0x22 ||
			src[i+1]== 0x3d || src[i+1] == 0x5c ) ){
			
			/* Escaped character */
			
			dest[ii++] = src[i+1];
			i++;
			
		} else {
			
			/* NonEscaped character */
			
			dest[ii++] = src[i];
		}

	}
	dest[ii] = '\0';

	return 0;
	
}

/***********************
  $B;XDj$5$l$?@hF,$NJ8;zNs$r;}$D%H!<%/%s$rJV$9!#%H!<%/%s$O%(%9%1!<%W$5$l$F$$$k(B
  $B6uGr$G$/$.$i$l$?$b$N$H$9$k!#(B
  $B%H!<%/%s$,$J$+$C$?>l9g$O!"6u$NJ8;zNs$rJV$9!#(B
  512$BJ8;z$N%H!<%/%s$,8B3&!#(B
***********************/
void getTokenByTop( char *src , char *type , char *output )
{
	int i;
	char dest[512];
	int l;
	output[0] = '\0';
	
	l = strlen( type );
	if( strlen( src ) > 511 ) return;
	
	for(i=0;;i++ ){
		if( getEscapedToken( src , ' ', i ,dest ) < 0 )break;
		if( strncmp( dest , type , l ) == 0 ){
			strcpy( output , dest + l );
		}
	}
}


main()
{
	char t[]="TALK mode=normal text=asdfas\\ asfd\\\"asf\\\"asd\\\\asfd fuck=asdfasdfasdf\n";
	char t2[] = "ATTACK mode=normal\n";
	char new[100];
	int i;

	getTokenByTop( t ,"mode=" , new);
	printf( new );
	printf( "\n");
	getTokenByTop( t ,"yee=" , new);
	printf( new );
	printf( "\n");
	getTokenByTop( t ,"text=" , new);
	printf( new );
	printf( "\n");
	getTokenByTop( t ,"fuck=" , new);
	printf( new );
	printf( "\n");
	

	
	
	
}
