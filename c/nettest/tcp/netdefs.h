/*
   	ネットワークユーティリティ共通ヘッダ

*/

#if 0
#include "systype.h"
#endif

#include <stdio.h>

#define MAXHOSTNAMELEN 	64
#define MAXLINE 	255
#define MAXBUFF		2048


/*
   traceflg を見て、出力するデバッグ用のマクロ


*/

#define DEBUG( fmt )		if ( traceflg ){ \
					fprintf(stderr , fmt );	\
					fputc('\n' , stderr );	\
					fflush( stderr );	\
				} else ;

#define DEBUG1( fmt , arg1 )	if( traceflg ) { \
					fprintf(stderr , fmt , arg1 );	\
					fputc( '\n' , stderr );		\
					fflush( stderr );		\
				}  else ;

#define DEBUG2( fmt , arg1 , arg2 )	if( traceflg ) { \
					fprintf(stderr , fmt , arg1 ,arg2 );	\
					fputc( '\n' , stderr );		\
					fflush( stderr );		\
				}  else ;

#define DEBUG3( fmt , arg1 , arg2 , arg3 )	if( traceflg ) { \
					fprintf(stderr , fmt , arg1 ,arg2 , arg3);	\
					fputc( '\n' , stderr );		\
					fflush( stderr );		\
				}  else ;


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


#ifdef XENIX

#include <sys/types.h>
#include <sys/socket.h>

struct hostent {
	char *h_name;
	int h_addrtype ;	/* 現在は必ずAF_INET */
	int h_length;
	char *h_addr;
};

struct servent {
	char *s_name ;
	short s_port;
	char *s_proto;
};

char *inet_ntoa();
struct hostent *gethostbyname();
struct servent *getservbyname();

#endif
