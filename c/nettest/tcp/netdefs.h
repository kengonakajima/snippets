/*
   	$B%M%C%H%o!<%/%f!<%F%#%j%F%#6&DL%X%C%@(B

*/

#if 0
#include "systype.h"
#endif

#include <stdio.h>

#define MAXHOSTNAMELEN 	64
#define MAXLINE 	255
#define MAXBUFF		2048


/*
   traceflg $B$r8+$F!"=PNO$9$k%G%P%C%0MQ$N%^%/%m(B


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
	int h_addrtype ;	/* $B8=:_$OI,$:(BAF_INET */
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
