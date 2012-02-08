#ifndef _LSSPROTOUTIL_H_
#define _LSSPROTOUTIL_H_
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <strings.h>
#include <unistd.h>
#endif

struct lssproto_ {
	int (*write_func)(int,char*,int) ;    /* write function */
	size_t workbufsize;                   /* size of work area */
    char  *work,*arraywork,*escapework,*val_str,*ret_work;      /* work areas which have same size  */
    char** token_list;                    /* token list */
    unsigned long message_id; /*= 1,  */       /* for generating message IDs */
};
#ifdef _LSSPROTOUTIL_C_
struct lssproto_ lssproto = {
	NULL,
	0,
	NULL,NULL,NULL,NULL,NULL,
	NULL,
	1,
};
char **lssproto_stringwrapper;
#else
extern char **lssproto_stringwrapper;
extern struct lssproto_ lssproto;
#endif
char* lssproto_escapeString( char*a );
char* lssproto_descapeString( char*a );
void lssproto_splitString( char *src  );
char*lssproto_mkstr_int( int i );
char*lssproto_mkstr_u_int( unsigned int i );
char*lssproto_mkstr_long( long l );
char*lssproto_mkstr_u_long( unsigned long l );
char*lssproto_mkstr_short( short s );
char*lssproto_mkstr_u_short( short s );
char*lssproto_mkstr_char( char c );
char*lssproto_mkstr_u_char( char c);
char*lssproto_mkstr_string( char*a );
char*lssproto_mkstr_float( float f );
char*lssproto_mkstr_double( double d );
char*lssproto_mkstr_int_array( int size , int *array );
char*lssproto_mkstr_u_int_array( int size , int *array );
char*lssproto_mkstr_short_array( int size , short *array );
char*lssproto_mkstr_u_short_array(int size , short *array );
char *lssproto_mkstr_char_array( int size , char *array );
char *lssproto_mkstr_u_char_array( int size , unsigned char *array );
char *lssproto_mkstr_float_array( int size , float *array );
char *lssproto_mkstr_double_array( int size , double *array );
int lssproto_demkstr_int( char*a );
unsigned int lssproto_demkstr_u_int( char*a );
long lssproto_demkstr_long( char*a );
unsigned long lssproto_demkstr_u_long(char*a );
short lssproto_demkstr_short( char*a );
unsigned short lssproto_demkstr_u_short( char*a );
char lssproto_demkstr_char( char*a );
unsigned char lssproto_demkstr_u_char( char*a );
float lssproto_demkstr_float( char*a );
double lssproto_demkstr_double(char*a );
char* lssproto_demkstr_string( char*a);
int *lssproto_demkstr_int_array( char**tk ,int *buf  ,int start , int size );
int *lssproto_demkstr_u_int_array( char **tk , int *buf , int start , int size );
unsigned int *lssproto_demkstr_long_array(
	char **tk , unsigned int *buf , int start , int size );
unsigned long *lssproto_demkstr_u_long_array(
	char **tk , unsigned long *buf , int start , int size );
short *lssproto_demkstr_short_array( char **tk , short *buf , int start , int size );
unsigned short*lssproto_demkstr_u_short_array( 
	char **tk , unsigned short *buf , int start , int size );
char *lssproto_demkstr_char_array( char **tk , char *buf , int start , int size );
unsigned char *lssproto_demkstr_u_char_array( 
	char **tk , unsigned char*buf , int start , int size );
float *lssproto_demkstr_float_array( char **tk , float *buf , int start , int size );
double *lssproto_demkstr_u_double_array( char **tk , double *buf , int start , int size );
char *lssproto_wrapStringAddr( char *copy , char*src );
void lssproto_ModifyStyle( char *a );
void lssproto_GetMessageInfo( int *id , char *funcname , char **tk );
int lssproto_ClientRead(void);
void lssproto_consumeLine(char *buf , int ofs );
void lssproto_copyLine( char*src , char *out );
void lssproto_Send( int fd , char *msg );
int lssproto_AllocateCommonWork(int bufsiz);
unsigned int lssproto_GetNewMessageID(void);
char *lssproto_ClientGetVirtualMessage(void);
void lssproto_CreateHeader(char*out, char *fname );
void lssproto_CreateHeaderID( char *out,unsigned long msgid , char *fname );
int lssproto_default_write_wrap( int fd , char *buf , int size );
void lssproto_bzero( char *b , int siz );
char *lssproto_Ltoa( long v );
char *lssproto_Ultoa( unsigned long v );
#endif

