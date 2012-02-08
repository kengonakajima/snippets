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

	/* $BFI$_$3$_%P%C%U%!!#(B1$B9TC10L$NFI$_$3$_$r2DG=$K$9$k$?$a$K%P%C%U%!$r;H$o$s$H$"$+$s(B */
#define READBUFSIZE 1024
#define WRITEBUFSIZE 4096
	char *readbuf;
	int readbufsize;       /* $B%P%C%U%!$N%5%$%:(B */
	int readbuf_use;       /* $B2?%P%$%H$D$+$C$F$k$N$+!"$$$^!#(B */

	int timeout;
};

typedef struct _CLIENT CLIENT ;

/* $B%5!<%P!<MQ4IM}9=B$BN!#%5!<%P!<$b$$$/$D$G$bJ];}$G$-$k(B */
struct _SERVER
{
	char hostname[MAXHOSTNAMELEN];
	int port;
	int tcp_sockfd;         /* $B$3$l$O%a%$%s%=%1%C%H$@!#$3$l$G(Baccept$B$9$k$N$@(B */
	int udp_sockfd;         /* $B$3$l$b%a%$%s$@$,(Baccept$B$J$7(B */
	
	char errmsg[1024];


	/* $B%=%1%C%H$4$H$K5-21$9$k>pJs$,B?$$(B */
#define  MAXSOCKFD 1024
	int sockuse[MAXSOCKFD];
	char *readbuf[MAXSOCKFD] , *writebuf[MAXSOCKFD];
	int readbufsize[MAXSOCKFD] , writebufsize[MAXSOCKFD];
	int readbuf_use[MAXSOCKFD] , writebuf_use[MAXSOCKFD];
	struct sockaddr_in cli[MAXSOCKFD];
	struct timeval lastwhen[MAXSOCKFD];

	int timeout;

	/* $B%k!<%W4X?t(B */
	void (*loopfunc) (void );
	
};
typedef struct _SERVER SERVER;


/*


  $B4X?t$N%W%m%H%?%$%W$,$3$3$K$"$k(B

$B$3$l$i$N4X?t$O(Btsrpc.c$B$GDj5A$5$l$k(B
*/

/* $BJ8;zNs$r:n$k$?$a$NE,Ev$J%P%C%U%!(B */
#define MKSTR_MINSIZE 128





/* $B2DJQD9J8;zNs$N%i%$%V%i%j(B */
char*escapeString( char*a );
char*descapeString( char*a );
char* joinString( char*a, char *join );
char** modifySplitString( char *src  );


/* $BJ8;zNs$H4pK\7?$NJQ49(B */
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


/* $B$=$NB>$N%i%$%V%i%j;H$&MQ(B */
extern int errno;

static char *ultoa( unsigned long fuck );
static char *ltoa( long fuck );



#endif     /* ifndef _TSRPC_H_ */





















