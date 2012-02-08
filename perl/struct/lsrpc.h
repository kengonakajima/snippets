#ifndef _LSRPC_H_
#define _LSRPC_H_
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define SENDRECV
#define SENDONLY
#define NONE
#define SERVACTIVE

struct lsrpc {
	int (*write_func)(int,char*,int) ;    /* write function */
	size_t workbufsize;                   /* size of work area */
    char  *work,*arraywork,*escapework,*val_str;       /* workk areas which have same size  */
    int default_fd; /*= -1;*/                  /* default fd (client)*/
    char** token_list;                    /* token list */
    unsigned long message_id; /*= 1,  */       /* for generating message IDs */
    char *cli_read_buffer;                /* read buffer (client)*/
    size_t cli_read_buffer_size;          /* read buffer size */
    char *cli_send_q;                     /* memory everything sent(client) */
    size_t cli_send_q_size;               /* size of it */
};
#ifdef _LSRPC_C_
struct lsrpc lsrpc = {
	NULL,
	0,
	NULL,NULL,NULL,NULL,
	-1,
	NULL,
	1,
	NULL,
	0,
	NULL,
	0
};
char **stringwrapper;
#else
extern char **stringwrapper;
extern struct lsrpc lsrpc;
#endif
char* escapeString( char*a );
char* descapeString( char*a );
void splitString( char *src  );
char*mkstr_int( int i );
char*mkstr_u_int( unsigned int i );
char*mkstr_long( long l );
char*mkstr_u_long( unsigned long l );
char*mkstr_short( short s );
char*mkstr_u_short( short s );
char*mkstr_char( char c );
char*mkstr_u_char( char c);
char*mkstr_string( char*a );
char*mkstr_float( float f );
char*mkstr_double( double d );
char*mkstr_int_array( int size , int *array );
char*mkstr_u_int_array( int size , int *array );
char*mkstr_short_array( int size , short *array );
char*mkstr_u_short_array(int size , short *array );
char *mkstr_char_array( int size , char *array );
char *mkstr_u_char_array( int size , unsigned char *array );
char *mkstr_float_array( int size , float *array );
char *mkstr_double_array( int size , double *array );
int demkstr_int( char*a );
unsigned int demkstr_u_int( char*a );
long demkstr_long( char*a );
unsigned long demkstr_u_long(char*a );
short demkstr_short( char*a );
unsigned short demkstr_u_short( char*a );
char demkstr_char( char*a );
unsigned char demkstr_u_char( char*a );
float demkstr_float( char*a );
double demkstr_double(char*a );
char* demkstr_string( char*a);
int *demkstr_int_array( char**tk ,int *buf  ,int start , int size );
int *demkstr_u_int_array( char **tk , int *buf , int start , int size );
unsigned int *demkstr_long_array(
	char **tk , unsigned int *buf , int start , int size );
unsigned long *demkstr_u_long_array(
	char **tk , unsigned long *buf , int start , int size );
short *demkstr_short_array( char **tk , short *buf , int start , int size );
unsigned short*demkstr_u_short_array( 
	char **tk , unsigned short *buf , int start , int size );
char *demkstr_char_array( char **tk , char *buf , int start , int size );
unsigned char *demkstr_u_char_array( 
	char **tk , unsigned char*buf , int start , int size );
float *demkstr_float_array( char **tk , float *buf , int start , int size );
double *demkstr_u_double_array( char **tk , double *buf , int start , int size );
char *wrapStringAddr( char *copy , char*src );
int lsrpcSetDefaultFd( int fd );
void lsrpcModifyStyle( char *a );
void lsrpcGetMessageInfo( int *id , char *funcname , char **tk );
int lsrpcClientRead(void);
int lsrpcCheckLineComplete(char*out);
void consumeLine(char *buf , int ofs );
void copyLine( char*src , char *out );
void lsrpcGetSendQueueLine( int msgid , char *out );
void lsrpcClientSend( char *msg , int flg);
void lsrpcServerSend( int fd , char *msg );
int lsrpcAllocateCommonWork(int bufsiz);
unsigned int lsrpcGetNewMessageID(void);
char *lsrpcClientGetVirtualMessage(void);
void lsrpcCreateHeader(char*out, char *fname );
void lsrpcCreateHeaderID( char *out,unsigned long msgid , char *fname );
int lsrpcAppendCliSendQueue( char *mesg );
int lsrpcAppendCliReadBuffer( char *mesg );
int default_write_wrap( int fd , char *buf , int size );
int default_read_wrap( int fd , char *buf , int size );
void lsrpcClientDispatchMessage(char*line);
void lsrpcServerDispatchMessage( int fd, char *line );
char *lsrpcLtoa( long v );
char *lsrpcUltoa( unsigned long v );
#endif

