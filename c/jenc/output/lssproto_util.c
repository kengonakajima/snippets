#define _LSSPROTOUTIL_C_

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#include <strings.h>
#endif
#include "lssproto_util.h"
/*
  lsrpc routines
*/
int lssproto_AllocateCommonWork(int bufsiz)
{
	lssproto.workbufsize = bufsiz;
	lssproto.work = (char*)malloc( lssproto.workbufsize );
	if( lssproto.work == NULL ) return -1;
	lssproto_bzero( lssproto.work , lssproto.workbufsize );
	lssproto.arraywork = (char*)malloc( lssproto.workbufsize );
	if( lssproto.arraywork ==NULL){
		free( lssproto.work );
		return -3;
	}
	lssproto_bzero( lssproto.arraywork , lssproto.workbufsize );
	lssproto.escapework = (char*)malloc( lssproto.workbufsize );
	if( lssproto.escapework == NULL ){
		free(lssproto.work );free(lssproto.arraywork );
		return -4;
	}
	lssproto_bzero( lssproto.escapework , lssproto.workbufsize );
	lssproto.val_str = (char*)malloc( lssproto.workbufsize );
	if( lssproto.val_str == NULL ){
		free(lssproto.work );free(lssproto.escapework );free( lssproto.arraywork );
		return -5;
	}
	lssproto_bzero( lssproto.val_str , lssproto.workbufsize );
	lssproto.token_list = (char**)malloc( lssproto.workbufsize *sizeof( char** ) );
	if( lssproto.token_list == NULL){
		free(lssproto.work);free(lssproto.val_str);
		free(lssproto.escapework );free(lssproto.arraywork );
		return -6;
	}
	lssproto_bzero( (char*)lssproto.token_list , lssproto.workbufsize );
	return 0;
}
/**********
  Get message information from a network input
**********/
void lssproto_GetMessageInfo( int *id , char *funcname , char **tk )
{
	if( tk[0] == NULL || tk[1] == NULL ){
		*id = 0;
		strcpy( funcname , "" );
		return;
	}
	*id =  strtoul( tk[0] ,NULL,10);
	strcpy( funcname , tk[1] );
	return;
}
/********************************************
  string utilities
*********************************************/
char*lssproto_mkstr_int( int i )
{
#define MKSTR_INT(v) lssproto_Ltoa( (long)(v)) 
	strcpy( lssproto.val_str ,  (char*)MKSTR_INT(i) );
	strncat( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_u_int( unsigned int i )
{
#define MKSTR_U_INT(v) lssproto_Ultoa( (unsigned long ) (v) )
	strcpy( lssproto.val_str , MKSTR_U_INT(i));
	strncat( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_long( long l )
{
#define MKSTR_LONG(v) lssproto_Ltoa( v ) 
	strcpy( lssproto.val_str , MKSTR_LONG(l) );
	strncat( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_u_long( unsigned long l )
{
#define MKSTR_U_LONG(v) lssproto_Ultoa(v ) 
	strcpy( lssproto.val_str , MKSTR_U_LONG(l) );
	strncat( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_short( short s )
{
#define MKSTR_SHORT16(v)    lssproto_Ltoa( (long)((int)(v) ))
	strcpy( lssproto.val_str , MKSTR_SHORT16(s) );
	strncat( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_u_short( short s )
{
#define MKSTR_U_SHORT16(v)   lssproto_Ltoa( (long)(  ((long)(v)) & 0xffff ))
	strcpy( lssproto.val_str , MKSTR_U_SHORT16(s) );
	strncat( lssproto.val_str ," " , lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_char( char c )
{
#define MKSTR_CHAR8(v)   lssproto_Ltoa( ( long)((int)(v)))
	strcpy( lssproto.val_str , MKSTR_CHAR8(c));
	strncat( lssproto.val_str ," ", lssproto.workbufsize );
	return lssproto.val_str;	
}
char*lssproto_mkstr_u_char( char c)
{
#define MKSTR_U_CHAR8(v)   lssproto_Ltoa( (long)( ((long)(v)) & 0xff ))
	strcpy( lssproto.val_str , MKSTR_U_CHAR8(c));
	strncat( lssproto.val_str ," "  , lssproto.workbufsize );
	return lssproto.val_str;
}
char*lssproto_mkstr_string( char*a )
{
	char *ret = lssproto_escapeString( a );
	strcat( ret , " ");
	return ret;
}
char*lssproto_mkstr_float(float f )
{
	sprintf( lssproto.val_str , "%f " , f );
	return lssproto.val_str;
}
char*lssproto_mkstr_double( double d )
{
	sprintf( lssproto.val_str , "%f " , d );
	return lssproto.val_str;
}
char*lssproto_mkstr_int_array( int size , int *array )
{
#define MKSTR_ARRAYMACRO( func) \
	{\
		 int i;\
         lssproto.arraywork[0] = '\0';\
		 for(i=0;i<size;i++){   \
             strncat( lssproto.arraywork , func(array[i]) , lssproto.workbufsize );\
		 }\
         return lssproto.arraywork;\
    }\

	MKSTR_ARRAYMACRO( lssproto_mkstr_int );
}
char*lssproto_mkstr_u_int_array( int size , int *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_u_int );
}
char*lssproto_mkstr_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_short );
}
char*lssproto_mkstr_u_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_u_short );
}
char *lssproto_mkstr_char_array( int size , char *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_char );
}
char *lssproto_mkstr_u_char_array( int size , unsigned char *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_u_char );
}
char *lssproto_mkstr_float_array( int size , float *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_float );
}
char *lssproto_mkstr_double_array( int size , double *array )
{
	MKSTR_ARRAYMACRO( lssproto_mkstr_double );
}
/*********************
   translate string into base types
*********************/
int lssproto_demkstr_int( char*a )
{
	if( a == (char*)NULL)return 0;
	return (int)strtol( a ,NULL , 10);
}
unsigned int lssproto_demkstr_u_int( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned int ) strtoul( a ,NULL,10);
}
long lssproto_demkstr_long( char*a )
{
	if( a == (char*)NULL)return 0;
	return (long)strtol( a , NULL , 10 );
}
unsigned long lssproto_demkstr_u_long(char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned long ) strtoul( a , NULL , 10 ) ;
}
short lssproto_demkstr_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (short) strtol( a , NULL , 10 );
}
unsigned short lssproto_demkstr_u_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned short) strtoul( a , NULL , 10 );
}
char lssproto_demkstr_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (char) strtol(  a  , NULL , 10 );
}
unsigned char lssproto_demkstr_u_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned char ) strtoul( a,NULL , 10 );
}
float lssproto_demkstr_float( char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (float) atof( a);
}
double lssproto_demkstr_double(char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (double) strtod( a , NULL );
}
char* lssproto_demkstr_string( char*a)
{
	if( a == (char*)NULL){
		strcpy( lssproto.escapework , "" );
		return lssproto.escapework;
	}
	return lssproto_descapeString( a );
}
int *lssproto_demkstr_int_array( char**tk ,int *buf  ,int start , int size )
{
#define DEMKSTR_ARRAYMACRO( func, defaultvalue )          \
    {\
        int i;\
        for(i=start;i<(start+size);i++){\
            if( tk[i] == NULL ){   \
		        buf[i-start]=defaultvalue ;\
            } else {\
     	 	    buf[i-start] = func( tk[i] );\
            }\
	    }\
        return buf;\
    }
    DEMKSTR_ARRAYMACRO( lssproto_demkstr_int , 0 );
}
int *lssproto_demkstr_u_int_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_int , 0 );
}
unsigned int *lssproto_demkstr_long_array(
 char **tk , unsigned int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_long  , 0);
}
unsigned long *lssproto_demkstr_u_long_array(
 char **tk , unsigned long *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_long , 0);
}
short *lssproto_demkstr_short_array( char **tk , short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_short , 0);
}
unsigned short* lssproto_demkstr_u_short_array( 
 char **tk , unsigned short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_short , 0);
}
char *lssproto_demkstr_char_array( char **tk , char *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_char , 0);
}
unsigned char *lssproto_demkstr_u_char_array( 
char **tk , unsigned char*buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_u_char , 0);
}
float *lssproto_demkstr_float_array( char **tk , float *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_float , (float)0.0);
}
double *lssproto_demkstr_u_double_array( char **tk , double *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( lssproto_demkstr_double , (double)0.0);
}
/*********************
	Escape a string
	Escape only  ' ' , '\', '
'
*********************/
char*  lssproto_escapeString( char*a )
{
	int i,c=0;
	lssproto.escapework[0] = '\0';
	for(i=0;;i++){
		if( a[i] == '\0' ){
			lssproto.escapework[c++] = '\0';
			break;
		} else if( a[i] == '\\' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = '\\';
		} else if( a[i] == ' ' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = 'S';
		} else if( a[i] == '\n' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = 'n';
		} else if( a[i] == '\r' ){
			lssproto.escapework[c++] = '\\';
			lssproto.escapework[c++] = 'r';
		} else {
			lssproto.escapework[c++] = a[i];
		}
	}
	return lssproto.escapework;
}
char* lssproto_descapeString( char*a )
{
	int i ,c =0;
	lssproto.escapework[0] = '\0';
	for(i=0;;i++){
		if( a[i] == '\0' ){
			lssproto.escapework[c++]='\0';
			break;
		} else if( a[i] == '\\' ){
			if( a[i+1] == 'S' ){     /* space */
				lssproto.escapework[c++] = ' ';
			} else if( a[i+1] == 'n' ){
				lssproto.escapework[c++] = '\n';
			} else if( a[i+1] == 'r' ){
				lssproto.escapework[c++] = '\r';
			} else if( a[i+1] == '\\' ){
				lssproto.escapework[c++] = '\\';
			} else {
				lssproto.escapework[c++] = a[i];
			}
			i++;           
		} else {
			lssproto.escapework[c++] = a[i];
		}
	}
	return lssproto.escapework;
}
/*
   This function works only when char*src is escaped
   NOTICE: Effects and Modifies the contents of char*src!
   NOTICE : Ends the output token list with NULL pointer
Ex:
        v out[0]       v out[1]
  "     asdjfhasfdasdf asdf asf asdf "
 */
void lssproto_splitString( char *src  )
{
	int i,c=0;
	for(i = 0 ; ; i++ ){
		if( src[i] == '\0' ) break;
		if( i==0){
		    lssproto.token_list[c++]=&(src[i]);
		}
		if( src[i]== ' '){
		    lssproto.token_list[c++]=&(src[i+1]);
		}
	}
	while( *src ){
		if( ( *src  ) == ' ' || (*src ) == '\n' || (*src ) == '\r' ) *src = '\0';
		src++;
 	}
	lssproto.token_list[c] = (char*)NULL;
}
void lssproto_ModifyStyle( char *a )
{
	int l = strlen( a );
	if( l <= 0 )  return;
	if( a[l-1] != '\n' ){
		if(a[l-1] == ' '){
			a[l-1] = '\n'; 
		} else { 
			a[l] = '\n'; a[l+1] = '\0';
		}
	}
}
int lssproto_default_write_wrap( int fd , char *buf , int size )
{
#ifndef WIN32
	return write( fd , buf , size );
#else
	return 0;
#endif
}

void lssproto_consumeLine(char *buf , int offset )
{
	int i;
	int shift=0;
	buf+= offset;
	for(i=0;;i++){
		if( buf[i] == '\n' ){
			shift = i + 1;
			break;
		}
	}
    if( shift == 0 )return;
	for(i=shift;;i++){
		buf[i - shift] = buf[i];
		if( buf[i] == '\0')break;
	}
}
void lssproto_copyLine( char*src , char *out )
{
	int i;
	for(i=0;;i++){
		out[i] = src[i];
		if( src[i] == '\n' ){
			out[i+1] = '\0' ;
			return;
		}
		if( src[i] == '\0' )return;
	}
	strcpy( out , "" );
}
unsigned int lssproto_GetNewMessageID(void)
{
	return lssproto.message_id++;
}
/****************
  Get last message which is made by this RPC routines
****************/
char *lssproto_ClientGetVirtualMessage(void)
{
	return lssproto.work;
}
/*****************
  WRITE
   int flg : if 1, actually write. Otherwise no Network access
*****************/
void lssproto_Send( int fd , char *msg )
{
    lssproto_ModifyStyle( msg );
	lssproto.write_func( fd , msg , strlen( msg ));
}
/****************
  create a header which has function name and new Message ID
****************/
void lssproto_CreateHeader( char *out ,char *fname )
{
	sprintf( out ,"%u %s " , lssproto_GetNewMessageID() , fname );
}
void lssproto_CreateHeaderID( char *out,unsigned long msgid , char *fname )
{
	sprintf( out ,"%u %s " , (unsigned int)msgid , fname );
}
char *lssproto_Ltoa( long v )
{
	static char _ltoa_out[64];
	sprintf( _ltoa_out , "%d" , (int)v );
	return _ltoa_out;
}
char *lssproto_Ultoa( unsigned long v )
{
	static char _ultoa_out[64];
	sprintf( _ultoa_out , "%u" , (unsigned int)v );
	return _ultoa_out;
}
/****************
   string address wrapper
 RINGO  
****************/
char *lssproto_wrapStringAddr( char *copy , char*src )
{
	strcpy( copy , src );
	return copy;
}
/***************
 bzero buffer ( some OSs like win32 don't have bzero )
***************/
void lssproto_bzero( char *b , int siz )
{
	int i;
	for(i=0;i<siz;i++) b[i]=0;
}
/***************
    Encoder routine made by jun 1998.11.15
***************/    
int jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)
{
	char sum;
	int i;
	if(srclen+1 > maxencodedlen){
		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++) encoded[i] = src[i];
	}
	if(srclen+1 <= maxencodedlen){
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
			if(((key%7) == (i%5))||((key%2) == (i%2)))src[i] = ~src[i];
		}
		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i) encoded[i] = src[i] + sum*((i*i)%3);
			if(abs((key%srclen)) == i) encoded[i] = sum;
			if(abs((key%srclen)) < i) encoded[i] = src[i-1] + sum*((i*i)%7);
		}
        return 0;
	} else {
        return -1;
    }
}
/**************
    Decoder routine made by jun 1998.11.15
**************/
void jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)
{
	char sum;
	int i;
	*decodedlen=srclen-1;
	sum = src[abs(key%(*decodedlen))];
	for(i=0;i<srclen;i++){
		if(abs((key%(*decodedlen))) > i) decoded[i] = src[i] - sum*((i*i)%3);
		if(abs((key%(*decodedlen))) < i) decoded[i-1] = src[i] - sum*((i*i)%7);
	}
	for(i=0;i<(*decodedlen);i++){
		if(((key%7) == (i%5))||((key%2) == (i%2))) decoded[i] = ~decoded[i];
	}
}

/* end of generated code */
