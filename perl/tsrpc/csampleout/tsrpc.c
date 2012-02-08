/*
  TSRPC $B$,=PNO$9$k%3!<%I$G$"$k!#(B

  $B$3$N%U%!%$%k$O!"(BTSRPC$B$N4pK\%k!<%A%s$,F~$C$F$$$k!#(B










*/

#define _TSRPC_C_


#include "tsrpc.h"





/*********************
   $B4pK\7?$+$iJ8;zNs$X(B
  
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
  $BJ8;zNs$+$i4pK\7?$X(B
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
/* int $BCM$N%H!<%/%s%j%9%H$+$i(Bint$B$NG[Ns$r:n$k!#(B
 */
int *demkstr_int32_array( char**tk ,int *buf  ,int start , int size )
{
#define DEMKSTR_ARRAYMACRO( func, defaultvalue )          \
    {\
        int i;\
        for(i=start;i<(start+size);i++){\
            if( tk[i] == NULL ){   /* $B%5%$%:$KB-$j$F$J$$$>(B */\
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
  $BJ8;zNs$r%(%9%1!<%W$9$k(B

  $BJ8;zNs$O!"(B ' ' , '\\' '\n' $B$r%(%9%1!<%W$9$k$@$1$@$+$i!":GBg(B,$B8=:_$N%P%C%U%!(B
$B$N(B2$BG\$K$J$k$@$1$G$"$k!#<+A0$G$=$&$$$&%P%C%U%!$r0l;~E*$KMQ0U$9$k!#(B

*********************/
char*  escapeString( char*a )
{
	int i,c=0;


/* tmp$B$r3NJ]$9$k(B */
	char *tmp = malloc( strlen( a ) * 2 +1);
	if( tmp == NULL ) return a;

/* tmp$B$KFbMF$r:n$C$F$$$C$F(B, */
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
/* $BD9$5$NJ,$@$1(Brealloc$B$7$F$+$i%3%T!<(B */
	a = realloc( a , strlen( tmp ) +1);
	strcpy( a , tmp );
	free( tmp );
	return a;
}
/********
  $B$b$H$K$b$I$9>l9g$O!"D9$/$J$k$3$H$O$J$$!#(B

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
   a$B$r$N$P$7$D$D$D$1$/$o$($F$$$/(B
   $B%5%$%:JQ99$K$H$b$J$C$F(Bfree/malloc$B$9$k$N$G%]%$%s%?$NCM$rJV$9I,MW$,$"$k!#(B
   $B$3$l$r;H$&%k!<%A%s$G$O$=$NCM$r85$NCM$KLa$5$M$P$J$i$J$$(B

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
    solaris$B$K$O(Bultoa$B$,$J$$$N$GDj5A$9$k(B. HP_UX$B$K$O$"$C$?(B
	
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
   escape$B$5$l$F$$$k>uBV$D$^$j%9%Z!<%95-9f$,$J$$>l9g$N$_F/$/(B
   perl$B$N(B split(' ' , $hoge)$B$@$,0z?t$N<BBN$K1F6A$r$*$h$\$9(B


  char **out $B$O%L%k=*C<$G$"$k!#(B

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
				


				/* $B:GBg?t$KC#$C$7$F$7$^$C$?(B */
				if( c >= _modsoutcount ){

					char ** modsoutcopy;
					int oldmodsoutcount;

					printf( "Exceeded. c: %d\n" , c );

					/* $B:n6HMQ%P%C%U%!$r3NJ]$7$F$=$3$KFbMF$rB`Hr(B */
					modsoutcopy = (char**)malloc( sizeof( char**) * _modsoutcount );
					if( modsoutcopy == NULL ) return NULL;
					bcopy( _modsout , modsoutcopy , sizeof( char**) * _modsoutcount );

					/* $B%a%b%j$r2rJ|$7$F(B */
					free( (void*)_modsout );

					/* _modsout$B$NBg$-$5$r(B2$BG\$K$7$F(B */
					oldmodsoutcount = _modsoutcount;
					_modsoutcount *= 2; 
					
					/* $B?7$?$K%a%b%j$r3NJ]$9$k(B */
					_modsout = (char**)malloc( sizeof(char**) * _modsoutcount  );
					if( _modsout == NULL ) return NULL;

					/* $B$=$l$+$iFbMF$rLa$9(B */
					bcopy( modsoutcopy , _modsout , sizeof( char**) * oldmodsoutcount );
					
					/* $B:G8e$K:n6HMQ%P%C%U%!$rJ|4~$9$k(B */
					free( (void*) modsoutcopy );
				}
					
			}
		}
	}
	

	/* $B:G8e$K$9$Y$F$N%9%Z!<%95-9f$r%L%kJ8;z$K$9$k(B */

	while( *src ){
		if( ( *src  ) == ' ' || (*src ) == '\n' ) *src = '\0';
		*src++;
	}

	printf( "C:%d\n" , c );
	_modsout[c] = (char*)NULL;

	return _modsout;
}
