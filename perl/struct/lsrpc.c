#define _LSRPC_C_
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <strings.h>
#include "lsrpc.h"
/*
  lsrpc routines
*/
int lsrpcAllocateCommonWork(int bufsiz)
{
	lsrpc.workbufsize = bufsiz;
	lsrpc.work = (char*)malloc( lsrpc.workbufsize );
	if( lsrpc.work == NULL ) return -1;
	bzero( lsrpc.work , lsrpc.workbufsize );
	lsrpc.arraywork = (char*)malloc( lsrpc.workbufsize );
	if( lsrpc.arraywork ==NULL){
		free( lsrpc.work );
		return -3;
	}
	bzero( lsrpc.arraywork , lsrpc.workbufsize );
	lsrpc.escapework = (char*)malloc( lsrpc.workbufsize );
	if( lsrpc.escapework == NULL ){
		free(lsrpc.work );free(lsrpc.arraywork );
		return -4;
	}
	bzero( lsrpc.escapework , lsrpc.workbufsize );
	lsrpc.val_str = (char*)malloc( lsrpc.workbufsize );
	if( lsrpc.val_str == NULL ){
		free(lsrpc.work );free(lsrpc.escapework );free( lsrpc.arraywork );
		return -5;
	}
	bzero( lsrpc.val_str , lsrpc.workbufsize );
	lsrpc.token_list = (char**)malloc( lsrpc.workbufsize *sizeof( char** ) );
	if( lsrpc.token_list == NULL){
		free(lsrpc.work);free(lsrpc.val_str);free(lsrpc.escapework );free(lsrpc.arraywork );
		return -6;
	}
	bzero( lsrpc.token_list , lsrpc.workbufsize );
	return 0;
}
/**********
  Get message information from a network input
**********/
void lsrpcGetMessageInfo( int *id , char *funcname , char **tk )
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
/**********
char *msg : a message which has complete style
**********/
int lsrpcAppendCliSendQueue( char *mesg )
{
	int l = strlen( lsrpc.cli_send_q );
	int ll = strlen( mesg );
	if( ( l + ll ) >= lsrpc.cli_send_q_size ) return -1;
	strcat( lsrpc.cli_send_q , mesg );
	return 0;
}
int lsrpcAppendCliReadBuffer( char *mesg )
{
	int l = strlen( lsrpc.cli_read_buffer );
	int ll = strlen( mesg );
	if( ( l + ll ) >= lsrpc.cli_read_buffer_size ) return -1;
	strcat( lsrpc.cli_read_buffer , mesg );
	return 0;
}

/*******
  set default fd for write function
*******/
int lsrpcSetDefaultFd( int fd )
{
	lsrpc.default_fd = fd;
	return 0;
}
/********************************************
  string utilities
*********************************************/
char*mkstr_int( int i )
{
#define MKSTR_INT(v) lsrpcLtoa( (long)(v)) 
	strcpy( lsrpc.val_str ,  (char*)MKSTR_INT(i) );
	strcat( lsrpc.val_str ," ");
	return lsrpc.val_str;
}
char*mkstr_u_int( unsigned int i )
{
#define MKSTR_U_INT(v) lsrpcUltoa( (unsigned long ) (v) )
	strcpy( lsrpc.val_str , MKSTR_U_INT(i));
	strcat( lsrpc.val_str ," ");
	return lsrpc.val_str;
}
char*mkstr_long( long l )
{
#define MKSTR_LONG(v) lsrpcLtoa( v ) 
	strcpy( lsrpc.val_str , MKSTR_LONG(l) );
	strcat( lsrpc.val_str ," ");
	return lsrpc.val_str;
}
char*mkstr_u_long( unsigned long l )
{
#define MKSTR_U_LONG(v) lsrpcUltoa(v ) 
	strcpy( lsrpc.val_str , MKSTR_U_LONG(l) );
	strcat( lsrpc.val_str ," ");
	return lsrpc.val_str;	
}
char*mkstr_short( short s )
{
#define MKSTR_SHORT16(v)    lsrpcLtoa( (long)((int)(v) ))
	strcpy( lsrpc.val_str , MKSTR_SHORT16(s) );
	strcat( lsrpc.val_str ," ");
	return lsrpc.val_str;	
}
char*mkstr_u_short( short s )
{
#define MKSTR_U_SHORT16(v)   lsrpcLtoa( (long)(  ((long)(v)) & 0xffff ))
	strcpy( lsrpc.val_str , MKSTR_U_SHORT16(s) );
	strcat( lsrpc.val_str ," ");
	return lsrpc.val_str;	
}
char*mkstr_char( char c )
{
#define MKSTR_CHAR8(v)   lsrpcLtoa( ( long)((int)(v)))
	strcpy( lsrpc.val_str , MKSTR_CHAR8(c));
	strcat( lsrpc.val_str ," ");
	return lsrpc.val_str;	
}
char*mkstr_u_char( char c)
{
#define MKSTR_U_CHAR8(v)   lsrpcLtoa( (long)( ((long)(v)) & 0xff ))
	strcpy( lsrpc.val_str , MKSTR_U_CHAR8(c));
	strcat( lsrpc.val_str ," " );
	return lsrpc.val_str;
}
char*mkstr_string( char*a )
{
	char *ret = escapeString( a );
	strcat( ret , " ");
	return ret;
}
char*mkstr_float(float f )
{
	sprintf( lsrpc.val_str , "%f " , f );
	return lsrpc.val_str;
}
char*mkstr_double( double d )
{
	sprintf( lsrpc.val_str , "%f " , d );
	return lsrpc.val_str;
}
char*mkstr_int_array( int size , int *array )
{
#define MKSTR_ARRAYMACRO( func) \
	{\
		 int i;\
         lsrpc.arraywork[0] = ' ';\
		 for(i=0;i<size;i++){   \
             strcat( lsrpc.arraywork , func(array[i]));\
		 }\
         return lsrpc.arraywork;\
    }
	MKSTR_ARRAYMACRO( mkstr_int );
}
char*mkstr_u_int_array( int size , int *array )
{
	MKSTR_ARRAYMACRO( mkstr_u_int );
}
char*mkstr_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( mkstr_short );
}
char*mkstr_u_short_array( int size , short *array )
{
	MKSTR_ARRAYMACRO( mkstr_u_short );
}
char *mkstr_char_array( int size , char *array )
{
	MKSTR_ARRAYMACRO( mkstr_char );
}
char *mkstr_u_char_array( int size , unsigned char *array )
{
	MKSTR_ARRAYMACRO( mkstr_u_char );
}
char *mkstr_float_array( int size , float *array )
{
	MKSTR_ARRAYMACRO( mkstr_float );
}
char *mkstr_double_array( int size , double *array )
{
	MKSTR_ARRAYMACRO( mkstr_double );
}
/*********************
   translate string into base types
*********************/
int demkstr_int( char*a )
{
	if( a == (char*)NULL)return 0;
	return (int)strtol( a ,NULL , 10);
}
unsigned int demkstr_u_int( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned int ) strtoul( a ,NULL,10);
}
long demkstr_long( char*a )
{
	if( a == (char*)NULL)return 0;
	return (long)strtol( a , NULL , 10 );
}
unsigned long demkstr_u_long(char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned long ) strtoul( a , NULL , 10 ) ;
}
short demkstr_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (short) strtol( a , NULL , 10 );
}
unsigned short demkstr_u_short( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned short) strtoul( a , NULL , 10 );
}
char demkstr_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (char) strtol(  a  , NULL , 10 );
}
unsigned char demkstr_u_char( char*a )
{
	if( a == (char*)NULL)return 0;
	return (unsigned char ) strtoul( a,NULL , 10 );
}
float demkstr_float( char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (float) atof( a);
}
double demkstr_double(char*a )
{
	if( a == (char*)NULL)return 0.0F;
	return (double) strtod( a , NULL );
}
char* demkstr_string( char*a)
{
	if( a == (char*)NULL){
		strcpy( lsrpc.escapework , "" );
		return lsrpc.escapework;
	}
	return descapeString( a );
}
int *demkstr_int_array( char**tk ,int *buf  ,int start , int size )
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
    DEMKSTR_ARRAYMACRO( demkstr_int , 0 );
}
int *demkstr_u_int_array( char **tk , int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_int , 0 );
}
unsigned int *demkstr_long_array(
 char **tk , unsigned int *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_long  , 0);
}
unsigned long *demkstr_u_long_array(
 char **tk , unsigned long *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_long , 0);
}
short *demkstr_short_array( char **tk , short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_short , 0);
}
unsigned short*demkstr_u_short_array( 
 char **tk , unsigned short *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_short , 0);
}
char *demkstr_char_array( char **tk , char *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_char , 0);
}
unsigned char *demkstr_u_char_array( 
char **tk , unsigned char*buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_u_char , 0);
}
float *demkstr_float_array( char **tk , float *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_float , 0.0);
}
double *demkstr_u_double_array( char **tk , double *buf , int start , int size )
{
	DEMKSTR_ARRAYMACRO( demkstr_double , 0.0);
}
/*********************
	Escape a string
	Escape only  ' ' , '\', '
'
*********************/
char*  escapeString( char*a )
{
	int i,c=0;
	lsrpc.escapework[0] = '\0';
	for(i=0;;i++){
		if( a[i] == '\0' ){
			lsrpc.escapework[c++] = '\0';
			break;
		} else if( a[i] == '\\' ){
			lsrpc.escapework[c++] = '\\';
			lsrpc.escapework[c++] = '\\';
		} else if( a[i] == ' ' ){
			lsrpc.escapework[c++] = '\\';
			lsrpc.escapework[c++] = 'S';
		} else if( a[i] == '\n' ){
			lsrpc.escapework[c++] = '\\';
			lsrpc.escapework[c++] = 'n';
		} else {
			lsrpc.escapework[c++] = a[i];
		}
	}
	return lsrpc.escapework;
}
char* descapeString( char*a )
{
	int i ,c =0;
	lsrpc.escapework[0] = '\0';
	for(i=0;;i++){
		if( a[i] == '\0' ){
			lsrpc.escapework[c++]='\0';
			break;
		} else if( a[i] == '\\' ){
			if( a[i+1] == 'S' ){     /* space */
				lsrpc.escapework[c++] = ' ';
			} else if( a[i+1] == 'n' ){
				lsrpc.escapework[c++] = '\n';
			} else if( a[i+1] == '\\' ){
				lsrpc.escapework[c++] = '\\';
			} else {
				lsrpc.escapework[c++] = a[i];
			}
			i++;           
		} else {
			lsrpc.escapework[c++] = a[i];
		}
	}
	return lsrpc.escapework;
}
/*
   This function works only when char*src is escaped
   NOTICE: Effects and Modifies the contents of char*src!
   NOTICE : Ends the output token list with NULL pointer
Ex:
        v out[0]       v out[1]
  "     asdjfhasfdasdf asdf asf asdf "
 */
void splitString( char *src  )
{
	int i,c=0;
	for(i = 0 ; ; i++ ){
		if( src[i] == '\0' ) break;
		if( src[i] != ' ' ){
			if( i > 0 && src[i] == ' ' && src[i-1] == ' ' ){
				lsrpc.token_list[c++] = &( src[i] );
			}
			if( i == 0 || src[i-1] == ' ' ){
				lsrpc.token_list[c++] = & ( src[i] );
			}
		}
	}
	while( *src ){
		if( ( *src  ) == ' ' || (*src ) == '\n' ) *src = '\0';
		src++;
 	}
	lsrpc.token_list[c] = (char*)NULL;
}
void lsrpcModifyStyle( char *a )
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
int default_write_wrap( int fd , char *buf , int size )
{
	return write( fd , buf , size );
}
int default_read_wrap( int fd , char *buf , int size )
{
	return read( fd , buf , size );
}
int lsrpcCheckLineComplete(char *line )
{
	int i;
	for(i=0;;i++){
		if( lsrpc.cli_read_buffer[i] == '\0' )break;
		line[i] = lsrpc.cli_read_buffer[i];
		if( lsrpc.cli_read_buffer[i] == '\n'){
			line[i+1] = '\0';
			return 1;
		}
	}
	return 0;
}
void consumeLine(char *buf , int offset )
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
void copyLine( char*src , char *out )
{
	int i;
	for(i=0;;i++){
		out[i] = src[i];
		if( src[i] == '\n' ){
			out[i+1] = '\0' ;
			return;
		}
	}
	strcpy( out , "" );
}
/******************
	Get a command line from message ID and sendQ
int msgid : message ID
char *out : output (one line)
******************/
void lsrpcGetSendQueueLine( int msgid , char *out )
{
	int i;
	int first_msgid = atoi( lsrpc.cli_send_q );

	if( first_msgid == msgid ){
		copyLine( lsrpc.cli_send_q , out );
		consumeLine( lsrpc.cli_send_q , 0 );
		return;
	}
	for(i=0;;i++){
		if( lsrpc.cli_send_q[i] == '\0' )break;
		if( lsrpc.cli_send_q[i] == '\n' ){
			int next_msgid = atoi( lsrpc.cli_send_q + i + 1 );
			if( next_msgid == msgid ){
				copyLine( lsrpc.cli_send_q + i +1 , out );
				return;
			}
		}
	}
	strcpy( out , "" );
}
unsigned int lsrpcGetNewMessageID(void)
{
	return lsrpc.message_id++;
}
/****************
  Get last message which is made by this RPC routines
****************/
char *lsrpcClientGetVirtualMessage(void)
{
	return lsrpc.work;
}
/*****************
  WRITE
   int flg : if 1, actually write. Otherwise no Network access
*****************/
void lsrpcClientSend( char *msg ,int flg)
{
    lsrpcModifyStyle( msg );
	if( flg ){
		lsrpcAppendCliSendQueue( msg );
		lsrpc.write_func( lsrpc.default_fd , msg , strlen( msg ));
	}
}
void lsrpcServerSend( int fd , char *msg )
{
	lsrpcModifyStyle( msg );
	lsrpc.write_func( fd , msg , strlen(msg ));
}
/****************
  create a header which has function name and new Message ID
****************/
void lsrpcCreateHeader( char *out ,char *fname )
{
	sprintf( out ,"%u %s " , lsrpcGetNewMessageID() , fname );
}
void lsrpcCreateHeaderID( char *out,unsigned long msgid , char *fname )
{
	sprintf( out ,"%u %s " , (unsigned int)msgid , fname );
}
char *lsrpcLtoa( long v )
{
	static char _ltoa_out[64];
	sprintf( _ltoa_out , "%d" , (int)v );
	return _ltoa_out;
}
char *lsrpcUltoa( unsigned long v )
{
	static char _ultoa_out[64];
	sprintf( _ultoa_out , "%u" , (unsigned int)v );
	return _ultoa_out;
}
/****************
   string address wrapper
 RINGO  
****************/
char *wrapStringAddr( char *copy , char*src )
{
	strcpy( copy , src );
	return copy;
}

