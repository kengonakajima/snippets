/*
  TSRPC が出力するコードである。

  このファイルは、TSRPCの基本ルーチンが入っている。










*/

#define _TSRPC_C_


#include "tsrpc.h"





/*********************
   基本型から文字列へ
  
*********************/
char*mkstr_int32( char *buf ,int i )
{
#define MKSTR_INT32(v) ltoa( (long)(v)) 
 	strcpy( buf ,  (char*)MKSTR_INT32(i) );
	return buf;
}
char*mkstr_u_int32( char *buf , unsigned int i )
{
#define MKSTR_U_INT32(v) ultoa( (unsigned long ) (v) )
	strcpy( buf , MKSTR_U_INT32(i));
	return buf;
}
char*mkstr_long32( char *buf ,long l )
{
#define MKSTR_LONG32(v) ltoa( v ) 
	strcpy( buf , (char*)MKSTR_LONG32(l) );
	return buf;
}
char*mkstr_u_long32( char *buf ,unsigned long l )
{
#define MKSTR_U_LONG32(v) ultoa(v ) 
	strcpy( buf , MKSTR_U_LONG32(l) );
	return buf;	
}
char*mkstr_short16( char *buf , short s )
{
#define MKSTR_SHORT16(v)    ltoa( (long)((int)(v) ))
	strcpy( buf , (char*)MKSTR_SHORT16(s) );
	return buf;	
}
char*mkstr_u_short16( char *buf ,short s )
{
#define MKSTR_U_SHORT16(v)   ltoa( (long)(  ((long)(v)) & 0xffff ))
	strcpy( buf , MKSTR_U_SHORT16(s) );
	return buf;	
}
char*mkstr_char8( char *buf , char c )
{
#define MKSTR_CHAR8(v)   ltoa( ( long)((int)(v)))
	strcpy( buf , MKSTR_CHAR8(c));
	return buf;	
}
char*mkstr_u_char8( char *buf , char c)
{
#define MKSTR_U_CHAR8(v)   ltoa( (long)( ((long)(v)) & 0xff ))
	strcpy( buf , MKSTR_U_CHAR8(c));
	return buf;
}
char*mkstr_string( char*a )
{
	return escapeString( a );
}
char*mkstr_float32( char *buf ,float f )
{
	sprintf( buf , "%f" , f );
	return buf;
}
char*mkstr_double64( char *buf , double d )
{
	sprintf( buf , "%f" , d );
	return buf;
}
char*mkstr_int32_array( char *buf , int size , int *array )
{
#define MKSTR_ARRAYMACRO( func) \
	{\
		 char tmp[MKSTR_MINSIZE];\
		 int i;\
		 for(i=0;i<size;i++){   \
             buf = joinString(buf ,func(tmp,array[i]));\
		 }\
         return buf;\
    }

	MKSTR_ARRAYMACRO( mkstr_int32 );
}
char*mkstr_u_int32_array( char *buf , int size , int *array )
{
	MKSTR_ARRAYMACRO( mkstr_u_int32 );
}
char*mkstr_short16_array( char*buf , int size , short *array )
{
	MKSTR_ARRAYMACRO( mkstr_short16 );
}
char*mkstr_u_short16_array( char*buf , int size , short *array )
{
	MKSTR_ARRAYMACRO( mkstr_u_short16 );
}
char *mkstr_char8_array( char*buf , int size , char *array )
{
	MKSTR_ARRAYMACRO( mkstr_char8 );
}
char *mkstr_u_char8_array( char*buf , int size , char *array )
{
	MKSTR_ARRAYMACRO( mkstr_u_char8 );
}
char *mkstr_float32_array( char*buf , int size , char *array )
{
	MKSTR_ARRAYMACRO( mkstr_float32 );
}
char *mkstr_double64_array( char*buf , int size , char *array )
{
	MKSTR_ARRAYMACRO( mkstr_double64 );
}




/*********************
  文字列から基本型へ
*********************/
int demkstr_int32( char*a )
{
	return (int)strtol( a ,NULL , 10);
}
unsigned int demkstr_u_int32( char*a )
{
	return (unsigned int ) strtoul( a ,NULL,10);
}
long demkstr_long32( char*a )
{
	return (long)strtol( a , NULL , 10 );
}
unsigned long demkstr_u_long32(char*a )
{
	return (unsigned long ) strtoul( a , NULL , 10 ) ;
}
short demkstr_short16( char*a )
{
	return (short) strtol( a , NULL , 10 );
}
unsigned short demkstr_u_short16( char*a )
{
	return (unsigned short) strtoul( a , NULL , 10 );
}
char demkstr_char8( char*a )
{
	return (char) strtol(  a  , NULL , 10 );
}
unsigned char demkstr_u_char8( char*a )
{
	return (unsigned char ) strtoul( a,NULL , 10 );
}
float demkstr_float32( char*a )
{
	return (float) atof( a);
}
double demkstr_double64(char*a )
{
	return (double) strtod( a , NULL );
}
char* demkstr_string( char*a)
{
	return descapeString( a );
}
/* int 値のトークンリストからintの配列を作る。
 */
int *demkstr_int32_array( char**tk ,int *buf  ,int start , int size )
{
#define DEMKSTR_ARRAYMACRO( func, defaultvalue )          \
    {\
        int i;\
        for(i=start;i<(start+size);i++){\
            if( tk[i] == NULL ){   /* サイズに足りてないぞ */\
		        buf[i-start]=defaultvalue ;\
            } else {\
     	 	    buf[i-start] = func( tk[i] );\
            }\
	    }\
        return buf;\
    }

    DEMKSTR_ARRAYMACRO( demkstr_int32 , 0 );
}
int *demkstr_u_int32_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_int32 , 0 );
}
int *demkstr_long32_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_long32  , 0);
}
int *demkstr_u_long32_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_long32 , 0);
}
int *demkstr_short16_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_short16 , 0);
}
int *demkstr_u_short16_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_short16 , 0);
}
int *demkstr_char8_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_char8 , 0);
}
int *demkstr_u_char8_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_char8 , 0);
}
int *demkstr_float_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_float32 , 0.0);
}
int *demkstr_u_double_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_double64 , 0.0);
}


/*********************
  文字列をエスケープする

  文字列は、 ' ' , '\\' '\n' をエスケープするだけだから、最大,現在のバッファ
の2倍になるだけである。自前でそういうバッファを一時的に用意する。

*********************/
char*  escapeString( char*a )
{
	int i,c=0;


/* tmpを確保する */
	char *tmp = malloc( strlen( a ) * 2 +1);
	if( tmp == NULL ) return a;

/* tmpに内容を作っていって, */
	for(i=0;;i++){
		if( a[i] == '\0' ){
			tmp[c++] = '\0';
			break;
		} else if( a[i] == '\\' ){
			tmp[c++] = '\\';
			tmp[c++] = '\\';
		} else if( a[i] == ' ' ){
			tmp[c++] = '\\';
			tmp[c++] = 'S';
		} else if( a[i] == '\n' ){
			tmp[c++] = '\\';
			tmp[c++] = 'n';
		} else {
			tmp[c++] = a[i];
		}
	}
/* 長さの分だけreallocしてからコピー */
	a = realloc( a , strlen( tmp ) +1);
	strcpy( a , tmp );
	free( tmp );
	return a;
}
/********
  もとにもどす場合は、長くなることはない。

*********/
char* descapeString( char*a )
{
	int i ,c =0;
	char *na = (char*)malloc( strlen( a ) +1);

	for(i=0;;i++){
		if( a[i] == '\0' ){
			na[c++]='\0';
			break;
		} else if( a[i] == '\\' ){
			if( a[i+1] == 'S' ){     /* space */
				na[c++] = ' ';
			} else if( a[i+1] == 'n' ){
				na[c++] = '\n';
			} else if( a[i+1] == '\\' ){
				na[c++] = '\\';
			} else {
				na[c++] = a[i];  /* shoudn't hit here */
			}
			i++;             /* NOTICE! */
		} else {
			na[c++] = a[i];
		}

	}
	strcpy( a , na );
	free( na );
	return a;
}
/* 
   aをのばしつつつけくわえていく
   サイズ変更にともなってfree/mallocするのでポインタの値を返す必要がある。
   これを使うルーチンではその値を元の値に戻さねばならない

 */
char* joinString( char*a, char* append )
{
	char *na;
	na = (char * )malloc( strlen(a ) + strlen(append) + strlen( " " ) + 1 );
	strcpy( na , a );
	strcat( na , " " );
	strcat( na , append );
	free(a);
	return na;
}

/*********
    solarisにはultoaがないので定義する. HP_UXにはあった
	
*********/
static char *ultoa( unsigned long fuck )
{
	static char buf[32];
	sprintf( buf , "%u" , fuck );
	return buf;
}
static char *ltoa( long fuck )
{
	static char buf[32];
	sprintf( buf , "%d" , fuck );
	return buf;
}

/****************************************************************/
/*
   escapeされている状態つまりスペース記号がない場合のみ働く
   perlの split(' ' , $hoge)だが引数の実体に影響をおよぼす


  char **out はヌル終端である。

        v out[0]       v out[1]
  "     asdjfhasfdasdf asdf asf asdf "

 */
char** modifySplitString( char *src  )
{
	int i,c=0;
	static int _modsoutcount  =   0;
	static char** _modsout = (char**)NULL;

#define BEGINWITH 4
	if( _modsout == (char**)NULL ){
		_modsoutcount = BEGINWITH;    /* start with 16-1 tokens */
		_modsout = (char**)malloc( sizeof( char**) * _modsoutcount );
		if( _modsout == (char**)NULL ) return NULL;
	}

	printf( "modsoutcount: %d\n" , _modsoutcount );

	for(i = 0 ; ; i++ ){
		if( src[i] == '\0' ) break;
		if( src[i] != ' ' ){
			if( i == 0 || src[i-1] == ' ' ){
				_modsout[c++] = & ( src[i] );
				


				/* 最大数に達っしてしまった */
				if( c >= _modsoutcount ){

					char ** modsoutcopy;
					int oldmodsoutcount;

					printf( "Exceeded. c: %d\n" , c );

					/* 作業用バッファを確保してそこに内容を退避 */
					modsoutcopy = (char**)malloc( sizeof( char**) * _modsoutcount );
					if( modsoutcopy == NULL ) return NULL;
					bcopy( _modsout , modsoutcopy , sizeof( char**) * _modsoutcount );

					/* メモリを解放して */
					free( (void*)_modsout );

					/* _modsoutの大きさを2倍にして */
					oldmodsoutcount = _modsoutcount;
					_modsoutcount *= 2; 
					
					/* 新たにメモリを確保する */
					_modsout = (char**)malloc( sizeof(char**) * _modsoutcount  );
					if( _modsout == NULL ) return NULL;

					/* それから内容を戻す */
					bcopy( modsoutcopy , _modsout , sizeof( char**) * oldmodsoutcount );
					
					/* 最後に作業用バッファを放棄する */
					free( (void*) modsoutcopy );
				}
					
			}
		}
	}
	

	/* 最後にすべてのスペース記号をヌル文字にする */

	while( *src ){
		if( ( *src  ) == ' ' || (*src ) == '\n' ) *src = '\0';
		*src++;
	}

	printf( "C:%d\n" , c );
	_modsout[c] = (char*)NULL;

	return _modsout;
}
