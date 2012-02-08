#include <stdio.h>


/***************
  エスケープシー  ケンスに対応した文字カウント。

  char * buf ... 文字列
  int escc ..... エスケープ記号
  int c ........ さがしたい文字
  int n ........ カウント数
  
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
  エスケープ記号に対応したトークナイザ

  エスケープされるべき記号は、

  0x20 スペース
  0x22 "
  0x3d =
  0x5c \

  エスケープ記号は、 \で、それ自体をあらわすには、\\とする。

  文字cで区切られたcount個目のトークンをdestにコピーする。0個目からはじまる。
  destはすでに確保されている領域を指すポインタ。
  return value .... 0  無事にトークンがみつかった。
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
  指定された先頭の文字列を持つトークンを返す。トークンはエスケープされている
  空白でくぎられたものとする。
  トークンがなかった場合は、空の文字列を返す。
  512文字のトークンが限界。
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
