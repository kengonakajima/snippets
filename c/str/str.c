#define _STR_C_

#include <string.h>
#include <ctype.h>


#define ON 	1
#define OFF 0



main()
{
	char line[]="IMAGE fuck type=fucker mode=hoge baka=aho\n" ;

	char token[1000];

	getEscapedTokenByTop( line , "type=" , token, sizeof(token ));
						 
	printf( "%s\n"  ,  token );

}


/***************************
  大文字、小文字を区別しない文字列比較
  同じかどうかしか調べないぞ
  
***************************/
int strcmpIcase(char *s1, char *s2 )
{

	while( *s1 != '\0' && *s2 != '\0' ){
		if( tolower(*s1++) != tolower( *s2++ )){
			return ON;
		}
	}

	return 0;
}

/***************************
  文字列をエスケープする。エスケープされるべき記号は、

  0x20 スペース
  0x3d =
  0x5c \
  0x22 "
  0x28 (
  0x29 )
  0x5b [
  0x5d ]
  
***************************/
void escapeString( char *src , char *dest )
{
	int sc=0,dc=0;

	for(sc=0,dc=0; ;sc++ ){
		if( src[sc]== ' ' || src[sc]== '=' ||
		   src[sc]=='\\' || src[sc]=='\"' || src[sc]=='(' || src[sc]==')' ||
		   src[sc]=='[' || src[sc]==']' || src[sc] == ',' ){
			dest[dc++] = '\\';
			dest[dc++] = src[sc];
		} else {
			dest[dc++] =src[sc];
		}
		if( src[sc] == '\0' )break;
	}
}


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



  エスケープ記号は、 \で、それ自体をあらわすには、\\とする。

  文字cで区切られたcount個目のトークンをdestにコピーする。0個目からはじまる。
  destはすでに確保されている領域を指すポインタ。
  return value .... 0  無事にトークンがみつかった。
                    <0 Error


  maxlenまでの長さがdestに出力される。
  
******************/
int getEscapedToken( char *src , int c , int count , char *dest , int maxlen)
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
		if( sn < 0 ){
			return -1;
		}
		sn++;
	} else {
		return -2;
	}
	ii=0;
	for(i=sn;i<en ; i++ ){
		
		if( src[i] == '\\' ){
			/* Escaped character */
			dest[ii++] = src[i+1];
			i++;
		} else {
			/* NonEscaped character */
			dest[ii++] = src[i];
		}
		if( ii >= (maxlen -1))break;
	}
	dest[ii] = '\0';
	return 0;
}

/***********************
  指定された先頭の文字列を持つトークンを返す。トークンはエスケープされている
  空白でくぎられたものとする。
  トークンがなかった場合は、空の文字列を返す。

***********************/
void getEscapedTokenByTop( char *src , char *type , char *output ,int outputlen)
{
	int i,l;

	output[0] = '\0';
	l = strlen( type );

	
	for(i=0;;i++ ){
		output[0] = '\0';
		if( getEscapedToken( src , ' ', i , output , outputlen) < 0 ){
			break;
		}
		if( strncmp( output , type , l ) == 0 ){
			for(i=0;i<outputlen;i++){
				output[i] = output[i+l];
				if( output[i] == '\0'){
					break;
				}
			}
			output[outputlen-1]='\0';		/* 強引に終わらせる */
			return;
		}
	}
}

/*************************
  文字列中の特定の文字を置きかえる。
*************************/
void replaceChar( char *buf , int from , int  to)
{
	
	while( *buf ){
		if( *buf == from ) *buf = to;
		buf++;
	}
}

/*************************
  ファイル名の末尾の拡張子を指定のものに変える  
  *************************/
void changeFileNameTail( char *name ,char *tail)
{
    int len;
    int i;
    int pos =0;
	
    len = strlen( name );

    /* 場所を求める。 */
	
    for(i = 0 ; i < len ; i++){
		if( name[len-i] == '.' ){
			pos = len-i;
			name[len-i+1] = '\0';
			break;
		}
    }
    if( pos==0)return;

    strcat( name , tail );
}

/*************************
  英文字列を小文字だけにする
  *************************/
void lowerCase( char *src )
{
    int c;
	
    while( (c = *src) != '\0' )
    {
		if( c >= 'A' && c <= 'Z' ) *src = c - ('a'-'A');
		src++;
    }
}

/*************************
  改行記号類を文字列から削除して詰める。
  
  *************************/
void removeCRandLF( char *src )
{
    int i;
    for(i = 0 ; ; i++){
		if( src[i] == '\0')break;
		if( src[i] == 10 || src[i] == 13 ){
			shiftCharArray( src , i );
			i--;
		}
    }
}

/*************************
  文字列のn文字目を取りだして詰める。
  元も文字列はエディットされる。
  値の範囲などは、上位ルーチンにお任せ。
  *************************/
void shiftCharArray( char *src , int n )
{
    int i;
    for(i = n ; ;i++){
		if((src[i] = src[i+1])==0)return;
    }
}

/*************************
  byteArrayLineInputStreamのようなことをする。文字列があったら、その文字列から
  一行取りだして、詰める。元の文字列も変化するので注意
  *************************/
void cutLineFromCharArray( char *src , char *dest )
{

    int i;
    int l;
	
    for(i  = 0 ; ;i++){
		
		dest[i] = src[i];
		if( src[i] == '\n' ) break;
		if( src[i] == '\0' ) break;
    }
    dest[i+1] = '\0';

    l = strlen( dest );
    for(i = 0 ;  ;i++){
		src[i] = src[i+l];
		if( src[i+l] == '\0')break;
    }	
}
/*************************
  文字列が数字列かどうか調べる。
  *************************/
int isnumstr(char *s )
{
    int c;

    if( s[0] == '-' ) s++;
	
    while( ( c = *s++) != '\0' ) if(  ! isdigit( c )) return 0;

    /* ぜんぶ数字だった */
    return 1;
	
}

/****************************************************
  汎用のコード
  ****************************************************/

/*****
  指定された文字が指定された回数出てきたら、その位置を返す。
  位置だから、文字列の0番目に出てきたら0が返るぞ。
  指定された個数がなかったら、-1を返す。
  *****/   
int countChar( char *buf , int c ,int n)
{
    int i;
    int count = 0;
    for(i = 0 ; ;i++){
		if( buf[i] == '\0' ) break;
		if( buf[i] == c ){
			count++;
			if( count == n ) return i;
		}
    }

    return -1;
}




/**************************
  文字列の改行記号を殺す。文字列は1行のデータとする。
  **************************/
char * chop( char *buf )
{
    char *backup = buf;
    while( *buf++){
		if( *buf == 10 || *buf == 13 ){
			*buf='\0';
			break;
		}
			
    }
    return backup;
}

