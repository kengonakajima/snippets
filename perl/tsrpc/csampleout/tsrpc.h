/*










*/


#ifndef _TSRPC_H_
#define _TSRPC_H_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>







#undef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64


struct _CLIENT
{
    char hostname[MAXHOSTNAMELEN];
    int port;
    int tcp_sockfd;
    int udp_sockfd;
	
	char errmsg[1024];

	/* 読みこみバッファ。1行単位の読みこみを可能にするためにバッファを使わんとあかん */
#define READBUFSIZE 1024
#define WRITEBUFSIZE 4096
	char *readbuf;
	int readbufsize;       /* バッファのサイズ */
	int readbuf_use;       /* 何バイトつかってるのか、いま。 */

	int timeout;
};

typedef struct _CLIENT CLIENT ;

/* サーバー用管理構造体。サーバーもいくつでも保持できる */
struct _SERVER
{
	char hostname[MAXHOSTNAMELEN];
	int port;
	int tcp_sockfd;         /* これはメインソケットだ。これでacceptするのだ */
	int udp_sockfd;         /* これもメインだがacceptなし */
	
	char errmsg[1024];


	/* ソケットごとに記憶する情報が多い */
#define  MAXSOCKFD 1024
	int sockuse[MAXSOCKFD];
	char *readbuf[MAXSOCKFD] , *writebuf[MAXSOCKFD];
	int readbufsize[MAXSOCKFD] , writebufsize[MAXSOCKFD];
	int readbuf_use[MAXSOCKFD] , writebuf_use[MAXSOCKFD];
	struct sockaddr_in cli[MAXSOCKFD];
	struct timeval lastwhen[MAXSOCKFD];

	int timeout;

	/* ループ関数 */
	void (*loopfunc) (void );
	
};
typedef struct _SERVER SERVER;


/*


  関数のプロトタイプがここにある

これらの関数はtsrpc.cで定義される
*/

/* 文字列を作るための適当なバッファ */
#define MKSTR_MINSIZE 128





/* 可変長文字列のライブラリ */
char*escapeString( char*a );
char*descapeString( char*a );
char* joinString( char*a, char *join );
char** modifySplitString( char *src  );


/* 文字列と基本型の変換 */
char*mkstr_int32( char *buf ,int i );
char*mkstr_u_int32( char *buf , unsigned int i );
char*mkstr_long32( char *buf ,long l );
char*mkstr_u_long32( char *buf ,unsigned long l );
char*mkstr_short16( char *buf ,short s );
char*mkstr_u_short16( char *buf ,short s );
char*mkstr_char8( char *buf ,char c );
char*mkstr_u_char8( char *buf ,char c);
char*mkstr_string( char*a );
char*mkstr_float32( char *buf ,float f );
char*mkstr_double64( char *buf ,double d );
char*mkstr_int32_array( char *buf ,int size , int *array );
char*mkstr_u_int32_array( char *buf ,int size , int *array );
char*mkstr_short16_array( char*buf , int size , short *array );
char*mkstr_u_short16_array( char*buf , int size , short *array );
char *mkstr_char8_array( char*buf , int size , char *array );
char *mkstr_u_char8_array( char*buf , int size , char *array );
char *mkstr_float32_array( char*buf , int size , char *array );
char *mkstr_double64_array( char*buf , int size , char *array );
int demkstr_int32( char*a );
unsigned int demkstr_u_int32( char*a );
long demkstr_long32( char*a );
unsigned long demkstr_u_long32(char*a );
short demkstr_short16( char*a );
unsigned short demkstr_u_short16( char*a );
char demkstr_char8( char*a );
unsigned char demkstr_u_char8( char*a );
char* demkstr_string( char*a);
float demkstr_float32( char*a );
double demkstr_double64(char*a );
int *demkstr_int32_array( char**tk ,int *buf  ,int start , int size );
int *demkstr_u_int32_array( char **tk , int *buf , int start , int size );
int *demkstr_long32_array( char **tk , int *buf , int start , int size );
int *demkstr_u_long32_array( char **tk , int *buf , int start , int size );
int *demkstr_short16_array( char **tk , int *buf , int start , int size );
int *demkstr_u_short16_array( char **tk , int *buf , int start , int size );
int *demkstr_char8_array( char **tk , int *buf , int start , int size );
int *demkstr_u_char8_array( char **tk , int *buf , int start , int size );
int *demkstr_float_array( char **tk , int *buf , int start , int size );
int *demkstr_u_double_array( char **tk , int *buf , int start , int size );


/* その他のライブラリ使う用 */
extern int errno;

static char *ultoa( unsigned long fuck );
static char *ltoa( long fuck );



#endif     /* ifndef _TSRPC_H_ */





















