#include <stdio.h>

#include "udptcp.h"


/*

   $BMQ0U$9$k%$%s%?!<%U%'%$%9(B


   ufd fd = usocket( );  $B%=%1%C%HI=$r$R$H$DKd$a$k(B

   ulisten( ufd fd);
   uaccept( ufd fd , unsigned long *cliaddr  );
   uwrite( ufd fd, char *buf , int siz );
   uread( ufd fd , char *buf , int siz );
   uclose( ufd fd );
   uconnect( ufd fd , addr)

   uinit(ul) UDP$B$N%=%1%C%H$r=i4|2=$9$k!#(B
   uproc();     $B%M%C%H%o!<%/(BIO$B$r;H$&$H$3$m(B($B$3$l$rIQHK$K8F$S$@$9I,MW$,$"$k!#(B)

   $B%/%i%$%"%s%H$,@\B3$r$7$h$&$H$7$F!"%5!<%P!<$,$=$l0J>eB?$/$N@\B3$,$G$-$J$$(B
   $B$H$$$&>uBV$K$J$C$?$i!"(BUDp$B$N%Q%1%C%H$r$+$($7$F!"(BREF$B%S%C%H$r$?$F$k!#(B
  REF$B%S%C%H$,$?$C$F$?$i!"%/%i%$%"%s%H$N(Bconnect$B$O<:GT$9$k!#(B
   connect$B$O(BSYN$B%S%C%H$,$?$C$F$$$k%G!<%?$N$J$$%Q%1%C%H$rAw?.$9$k!#(B


   $B%/%i%$%"%s%H$,(Bconnect$B$9$k$H(BSYN$B$r$*$/$k!#$=$l$r$&$1$H$k$H(BSYN$B$rJVEz$9$k$H$I$&$8$K(Bread_syn$B$r$?$F$k!#(B
   $B%/%i%$%"%s%H$O$=$NJVEz$r$&$1$H$k$H!"(Bread_syn$B$rN)$F$k!#$=$l$,(Bconnect$B$G$"$k!#(B
   connect$B$5$l$F$$$k$"$$$@$O!"%5!<%P!<$b%/%i%$%"%s%H$b(Bread_syn$B$,$?$C$F$$$k$o$1$@!#(B
   
   

   1$B%^%7%s$K(B1$B%3%M%/%7%g%s$H$$$&$h$&$K8GDj$9$k$H!"%"%I%l%9$N$_$r<1JL$K(B
   $B;H$($k$N$G!"$R$8$g$&$K%7%s%W%k$K$J$k!#(B
   
   
*/


/*   UDPTCP$B$N%X%C%@!<MQDj?t(B */

#define SEQ_OFS			0		/* ulong	htonl() $BAw?.MQ%7!<%1%s%9HV9f(B */
#define ACK_OFS			4		/* ulong 	htonl() $B1~Ez3NG'HV9f(B */
#define RESET_OFS		8		/* char $B:FAw$7$F$bDL?.$r2sI|$G$-$J$+$C$?!#$b$&$d$a$8$c!*(B $B%3%^%s%I(B*/
#define SYNCRONIZE_OFS	9		/* char 1$B$,;XDj$5$l$?$i!"%7!<%1%s%9HV9f$r(B0$B$K=i4|2=(B */
#define FINISH_OFS		10		/* char 1$B$@$C$?$i=*N;=hM}$7$F!"%G!<%?$rAw?.$7$-$C$F$7$^$&!#(B */
#define REFUSE_OFS		11		/* char 1$B$G@\B3$r5qH]$9$k$N$@!#(B(backlog$B$,$$$C$Q$$(B)
								   $B$N$3$j$rA4ItAw?.$G$-$?$i(BFIN$B$r;XDj$7$?%Q%1%C%H$rAw?.$9$k!#(B */
#define LENGTH_OFS		12		/* htonl() $B%G!<%?$ND9$5(B */
#define DATA_OFS		16		/* $B%G!<%?$N$O$8$^$k0LCV(B */
#define HEADERSIZE		DATA_OFS/* $B%X%C%@!<$N%5%$%:(B */



struct usock
{
	int use;             
	int listen;       /* listen$B$9$k$+$I$&$+(B */
	int acceptwait;		/* accept$B$5$l$k$N$rBT$C$F$$$k$J$i(B1$B!#(B */
	int read_syn;		/* SYN$B%3%^%s%I$r<u$1$H$C$?$+(B */
	
	unsigned long addr_dest;	/* $BAj<j$N(BIP htonl$B$5$l$?$"$H$N%G!<%?(B */	

	unsigned long ws_top;		/* $B2?%P%$%H=q$-$3$s$G$-$?$+(B */
	unsigned long rs_top;		/* $B2?%P%$%HFI$_$3$s$G$-$?$+(B */

#define SEGMENTSIZ 512
#define BUFFERSIZ (	SEGMENTSIZ * 8)	

	unsigned long rb_use , wb_use;
	char rb[BUFFERSIZ];		/* $BFI$_$3$_%P%C%U%!!<!#(B*/
	char wb[BUFFERSIZ];		/* $B=q$-$3$_%P%C%U%!!<!#(B */
	char rlog[BUFFERSIZ];	/* $BFI$_$3$_>u67$r5-O?!#%P%$%H$N0LCV$,40A4$KBP1~$7$F$$$k$N$@!#(B */
	char wlog[BUFFERSIZ];	/* $B=q$-$3$_>u67$r5-O?(B */

#define WLOG_ACKOK		3	/* ACK$B$5$l$?$H$$$&>u67$N%P%$%H(B */
#define WLOG_ACKWAIT	2	/* $B$"$H$O(BACK$B$r$^$D$N$_$H$$$&>u67$N%P%$%H!#(B */
#define WLOG_SENDWAIT	1	/* $BAw?.$5$l$k$N$rBT$D$N$_$H$$$&>u67$N%P%$(B */
							/* $B%H!#(Bwb$B$K$D$$$F$O!"%P%$%H$NL?$O!"(B0->1->2->3 */
							/* $B$H0\F0$9$k!#(B */
#define WLOG_NOUSE		0	/* $BL$;HMQ%P%C%U%!!<(B */

#define RLOG_ACKOK		1	/* $B%M%C%H%o!<%/$+$iFI$_$3$s$@!#(BACK$B$rAw?.$7$?$H$$$&$3$H(B */
#define RLOG_NOUSE		0	/* $BL$;HMQ!#$D$^$j(BRLOG$B$G$O(B 0->1$B$H0\9T$9$k(B */
	
	
};

#define MAXCON 16
struct usock stb[ MAXCON ];  /* $B:GBg$N@\B3(B */

#define MAXACCEPTWAIT	5		/* $B%"%/%;%9$,$"$C$?$,(Baccept$B$5$l$F$$$J$$@\B3$N$+$:(B  */

#define UDPPORT 9000

#define ACK_NOUSE	0xffffffff	/* ACK$B%U%#!<%k%I$,L58z$G$"$k$H$$$&0UL#!#(B */

struct udptcp
{
	int maxbacklog;      /* $B:GBg$K$?$^$k%P%C%/%m%0(B */
	int errno;              /* $B%(%i!<HV9f(B */
	int mainsockfd;				/* UDP$B$N%=%1%C%H(B */
	unsigned long ipaddr;		/* $B<+J,$N(BIP$B%"%I%l%9(B */
	
} udptcp;


static void shift( char *dat , int len ,int max);
static void memset( char *dat , int data , int len );
static int setack( int fd , unsigned long ack_start , unsigned long ack_length );
static int getindexbyaddr( unsigned long addr  );
static int newsock( int fd );
static unsigned long getrsend( int fd );
static int setsend( int fd , unsigned long start , unsigned long end );
static int getwfdpart(int fd , unsigned long *start , unsigned long *end ,unsigned long siz);
static issockinited( int fd );
static double getUTimeDouble(void);
static int sender( char *data , int len , unsigned long ipaddr );
static  void getheader( char *data ,
					   unsigned long *seq ,
					   unsigned long *ack ,
					   unsigned char *rst ,
					   unsigned char *syn ,
					   unsigned char *fin ,
					   unsigned char *ref ,
					   unsigned long *length);
static void makeheader( char* data,
					   unsigned long seq ,
					   unsigned long ack ,
					   unsigned char rst ,
					   unsigned char syn ,
					   unsigned char fin ,
					   unsigned char ref ,
					   unsigned long length );

#define CHECKFD(fd) \
	if( (fd) < 0 || (fd) >= MAXCON ||stb[fd].use ==0){\
		udptcp.errno = EBADFDNUM;\
		return -1;\
	}\

#define CHECKFDNUM(fd) \
	if( (fd) < 0 || (fd) >= MAXCON ){\
		udptcp.errno = EBADFDNUM;\
		return -1;\
	}\



/*
   UDP$B$N%=%1%C%H$r=i4|2=$9$k!#(B

   ipaddr$B$O(Bhtonl$B8e$N$b$N(B
   
   return values:
   0 OK
   <0 Error
   
*/
int uinit(unsigned long ipaddr )
{
	int i;
	struct sockaddr_in sin;
	
	udptcp.mainsockfd = socket( AF_INET , SOCK_DGRAM , 0 );
	if( udptcp.mainsockfd < 0 ){
		udptcp.errno = ESOCKETERR;
		fprintf( stderr, "#socket fuck\n");
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons( UDPPORT );
	sin.sin_addr.s_addr = ipaddr;
	if( (i=bind( udptcp.mainsockfd , (struct sockaddr*)& sin , sizeof( sin )))<0){
		udptcp.errno = ESOCKETERR;
		return i;
	}

	bzero( &stb , sizeof( stb) );
	
	udptcp.ipaddr = ipaddr;
	
}


/*
   socket$BI=$X$N%$%s%G%/%9$r$+$($9(B

   return values:
   >= 0 OK
   < 0 Error
   
   
*/
int usocket( void )
{
	int i;
	for(i=0;i<MAXCON;i++){
		if( ! stb[i].use  ){
			bzero( &stb[i] , sizeof( stb[0]  ) );
			newsock( i );
			return  i;
		}
	}
	
	udptcp.errno = ENOMORESOCKET;  /* too many open file */
	return -1;
}
/*
   $B2?HV$N%P!<%A%c%k%]!<%H$GBT$D$N$+$r;XDj$9$k%5!<%P!<MQ!#(B
   
 */
int usocket_serv(int index )
{
	CHECKFDNUM(index);
	
	if( ! stb[index].use  ){
		bzero( &stb[index] , sizeof( stb[0]  ) );
		newsock( index );
		return  index;
	} else {
		udptcp.errno = EALREADYUSE;
		return -1;
	}
}


/*
	listen$B$9$k(B

   int fd : socket$BI=$X$N(Bindex

   return values:
   0 OK
   <0 Error
   
*/
int ulisten( int fd )
{
	CHECKFD(fd);

	stb[fd].listen = 1;

	return 0;

}

/*
   usock$B$G%"%/%;%W%H(B

   int fd : socket$BI=$X$N(Bindex
   unsigned long cliaddr : $B%/%i%$%"%s%H$N(BIP$B%"%I%l%9(B

   fd$B$O(Blisten$B$7$F$$$J$1$l$P$J$i$J$$!#(B
   

   return value:
   >= 0 OK
   <0 Error
   

   $B%=%1%C%HI=$+$i8!:w$7$F!"(Bacceptwait$B$N>uBV$K$J$C$F$$$k$N$r$5$,$9(B

   cliaddr $B$O(B htonl$B8e!"(B 
   
   
 */
int uaccept( int fd , unsigned long *cliaddr )
{
	int i;

	CHECKFD(fd);

	if( ! stb[fd].listen ){
		udptcp.errno = ENOLISTEN;
		return -1;
	}
	
	for(i=0;i<MAXCON;i++){
		if( stb[i].use && stb[i].acceptwait ){
			stb[i].acceptwait = 0;
			*cliaddr = stb[i].addr_dest;
			return i;
		}
	}

	udptcp.errno = ENOACCEPTWAIT;
	return -1;
}


/*
   UDPTCP$B%=%1%C%H$r$H$8$k(B(FIN$B$rAw?.(B)
   
*/
int uclose( int fd )
{
	
}

/*



   addr$B$NCM$O!"(B htonl$B8e$NCM$G$"$k!#(B


   return values:
   0 OK
   <0 Error

   uproc$B$O!"2?$+%Q%1%C%H$,$-$?$iJV$jCM$G$7$i$;$k$N$G!"(B
   uproc$B$,2?$+$&$1$H$k$?$S$K%F%9%H$7$F!"(Back$B$,$-$?$i(Bconnect$B@.8y!"(B
   $B$H$$$&$h$&$K$7$J$1$l$P$J$i$J$$!#%V%m%C%/$9$k!#(B
   
*/
int uconnect( int fd , unsigned long addr )
{

	int sr;
	char tmpdata[HEADERSIZE];

	CHECKFD(fd);

	/* $B<+J,$N@_Dj$r$9$k(B */
	stb[fd].addr_dest = addr;
	
	/* connect$BMQ$N%Q%1%C%H$rAw?.!#%X%C%@$N$_(B */
	makeheader( tmpdata, 
			   0 ,	/* seq */
			   ACK_NOUSE ,	/* ack */
			   0 ,	/* rst */
			   1 ,	/* syn */
			   0 ,	/* fin */
			   0 ,	/* ref */
			   0 	/* len */
	);
	sender( tmpdata, HEADERSIZE , addr );


	if( sr < 0 ){
		/* errno$B$O%;%C%H$5$l$F$k(B */
		return -1;
	}

	/* $B%V%m%C%/$7$F!"JV;v$,$/$k$^$GBT$D(B */
	while(1){
		usleep(100*1000);
		fprintf( stderr, "c ");
		if( uproc() > 0 ){
			if( issockinited( fd ) ){
				break;
			}
		}
	}
	
	udptcp.errno = 0;
	return 0;
}


/*

   write$B$9$k!#(Bfd$B$N%P%C%U%!$K$?$^$k$@$1$J$N$G!"$3$N$"$H(Buproc$B$7$J$1$l$P$J$i$J$$!#(B

   return values:
   0 OK
   <0 Error
   */
int uwrite( int fd , char *data , int len )
{

	char tmpdata[SEGMENTSIZ];
	int i;
	int write_total = 0;
	int cpsz;
	
	CHECKFD(fd);

	/* $B%5%$%:$,JQ(B */
	if(  len < 0 ){
		udptcp.errno = EBADDATALEN;
		return -1;
	}


	/* $B%=%1%C%H$N%P%C%U%!$K$?$9!#C1$K%"%Z%s%I$9$k$N$_(B */
	if( (stb[fd].wb_use + len) >= BUFFERSIZ){
		/* BUFFERSIZ$B$r$3$($F$k$>(B */
		cpsz = BUFFERSIZ - stb[fd].wb_use;
	} else {
		cpsz = len;
	}
	
	bcopy( data , stb[fd].wb + stb[fd].wb_use , cpsz );
	memset( stb[fd].wlog + stb[fd].wb_use , WLOG_SENDWAIT , cpsz );
	stb[fd].wb_use += cpsz;
	udptcp.errno = 0;
	fprintf( stderr, "copied %d byte to rb\n" , cpsz );
	return cpsz;
}

/*
   read$B$9$k!#(Bfd$B$N%P%C%U%!$K$?$^$C$F$$$kJ,$rFI$_$3$`$@$1$J$N$G!"(Buproc$B$7$?$"$H$G$J$$$H2?$b(B
   $B5/$3$i$J$$!#(B
 */
int uread( int fd, char *data , int len )
{

	int i ,k;
	int read_total = 0;
	int rs_end;
	int completesiz;

	CHECKFD(fd);

	/* $B%9%H%j!<%`%P%C%U%!$N$I$3$^$G$,40@.$7$?$N$+$r7W;;$9$k(B */
	rs_end = getrsend(fd);
	completesiz = rs_end - stb[fd].rs_top;

	udptcp.errno =0;
	if( completesiz > len ){
		;
	} else {
		len = completesiz;
	}
	
	bcopy( stb[fd].rb , data , len );
	shift( stb[fd].rb , len , BUFFERSIZ);
	shift( stb[fd].rlog , len ,BUFFERSIZ);
	memset( stb[fd].rlog+len,RLOG_NOUSE,BUFFERSIZ-len );
	stb[fd].rb_use -= len;
	fprintf( stderr, "RBUSE:%d\n",	stb[fd].rb_use );
	return len;
}


/*
 	$B%=%1%C%H$,FI$_$3$_2DG=$+$I$&$+$7$i$Y$k(B

   return values:
   0 Not readable
   1 OK
   -1 $B%(%i!<(B
   
   
 */
int ureadable( int fd )
{
	int i;

	CHECKFD(fd);

	if( issockinited(fd ) != 1 ){
		udptcp.errno = ENOTINIT;
		return 0;
	}


	/*	fprintf( stderr, "UREADABLE:%d END:%d TOP:%d\n" ,fd,getrsend(fd) , stb[fd].rs_top );*/

	/* $B%H%C%W$N0LCV$H=*$o$j$N0LCV$,F1$8$G$J$$$H$-!"FI$_$3$_2DG=(B */
	return !( ( getrsend(fd) - stb[fd].rs_top ) ==0 );

}


/*
   $BIQHK$K8F$S$@$5$l$k!"(BI/O$B$9$k4X?t!#%Q%1%C%H$NFbMF$r8+$F!"(B

   return values:

   $B$$$/$D$N%Q%1%C%H$r<u?.$7$?$+!#(B
   
 */
int uproc(void)
{
	fd_set rfd , wfd;
	int a;
	int rcount =0;
	int i;
	
	int read_ok ;
	int write_more;	
	while(1){
		struct timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = 0;
		
		FD_ZERO( &rfd );
		FD_ZERO( &wfd );

		FD_SET(  udptcp.mainsockfd , &rfd);
		FD_SET(  udptcp.mainsockfd , &wfd );

		a = select( 100 , &rfd , &wfd , (fd_set*)0 , &tm );

		read_ok = 0;
		write_more = 0;

		/* $B=q$-$3$_2DG=(B */
		if( FD_ISSET( udptcp.mainsockfd , &wfd )){
			int i,sr;
			unsigned long start,end,datalen;
			char buf[SEGMENTSIZ+HEADERSIZE];

			for(i=0;i<MAXCON;i++){
				/* start $B$+$i(Bend$B$^$G=q$-$3$_$?$$!#D9$5$OE,@Z$K$J$C$F$$(B */
				/* $B$k!#(B */

				if( stb[i].use == 0 ) continue;

				getwfdpart( i , &start, &end ,SEGMENTSIZ );
				datalen = end-start;
				if( datalen >0 ){
					fprintf( stderr, "send_start:%d len:%d\n" , start,datalen );
					makeheader( buf ,
					   start ,
					   ACK_NOUSE ,
					   0 , 0 , 0 , 0 ,
						datalen );
					bcopy( stb[i].wb+start ,buf + HEADERSIZE ,
						  datalen);

					/* $BAw?.(B */
					sr = sender( buf , datalen +HEADERSIZE , stb[i].addr_dest );
					if( sr != ( datalen+HEADERSIZE) ){
						break;
					} else {
						/* $B$A$c$s$HAw$l$?$+$i%P%C%U%!$r99?7(B */
						setsend( i, start , end );
					}
				}
			}

			write_more = 0;		/* $B:#$O$$$D$G$b(Bwrite_more = 0 PENDING */
		}

		/* $BFI$_$3$_2DG=(B */
		if( FD_ISSET( udptcp.mainsockfd  , &rfd )){
			int n;
			char buf[SEGMENTSIZ+HEADERSIZE];
			struct sockaddr_in sin;
			int clilen;
			read_ok = 1;

			n = recvfrom( udptcp.mainsockfd ,
						 buf , sizeof( buf ) , 0 , (struct sockaddr*)&sin , &clilen);
			if( n< 0 ) continue;
			fprintf( stderr , "recvfrom addr:%x\n", sin.sin_addr.s_addr );
			if( n > 0 ){
				
				/* $B%Q%1%C%H$rFI$_$3$`$3$H$,$G$-$?!#(B ack$B%U%#!<%k%I$K%G!<%?$,$"$k>l9g$O(Back$B$N=hM}$r$9$k!#(B
				 $B%G!<%?$,(B0xffffffff$B$N>l9g$O%G!<%?$G$"$k!#(B*/
				int index;
			   	unsigned long seq , ack;
			   	unsigned char rst,syn,fin,ref;
			   	unsigned long length;
				getheader( buf , &seq , &ack , &rst , &syn , &fin , &ref ,&length );

				
				/* $BBP1~$9$k(Bindex$B$r%]!<%H$N>pJs$+$iF@$k!#$J$$$H$-$O(B */
				/* accept$BBT$A9TNs$KDI2C$9$k!#(B */
				index = getindexbyaddr( sin.sin_addr.s_addr );
			
				if( syn ){
					/* SYN$B$G$"$k!#(B */
					if( index == -1 ){
						fprintf( stderr, "SYN!\n");
						/* $B:#$^$G$K$J$$%"%I%l%9$@(B($B%5!<%P!<$K$H$C$F$OEvA3(B)$B!#(B
						   accept$BBT$A%=%1%C%H$r$D$$$+(B */
						for(i=3;i<MAXCON;i++){
							if( stb[i].use == 0 ){
								bzero( &stb[i] , sizeof( stb[0] ) );
								newsock(i);
								stb[i].acceptwait = 1;
								stb[i].read_syn = 1;	/* syn$B$,$-$?$+$i(B */
								stb[i].addr_dest = sin.sin_addr.s_addr;
								break;
							}
						}
						if( i == MAXCON ){
							/* $B$3$l0J>e2?$b$G$-$J$$$+$i%Q%1%C%H$r<N$F$k!#(B
							   $B%/%i%$%"%s%H$+$i$O%5!<%P!<$,1~Ez$7$J$$$h$&$K8+$($k(B
							   $B$+$i!"%?%$%`%"%&%H$9$k$3$H$@$m$&!#(B*/
						} else {
							/* SYN$B$r$+$($9(B */
							makeheader( buf , 0 , 0, 0 , 1 , 0, 0 ,0 );/*ACK->0*/
							sender( buf , HEADERSIZE , sin.sin_addr.s_addr );		   
						}
						
						
					} else {
						/* $B:#$^$G$KB8:_$7$F$$$?%=%1%C%H$K(BSYN$B$,$-$?$i!"(B */
						/* $B$=$l$O%/%i%$%"%s%H$G$OEvA3$G$"$k(B */
						if( ack == ACK_NOUSE ){
							/* ACK_NOUSE$B$O%G!<%?Aw?.$N$H$-$K$D$+$&$N$G(BSYN$B$O(B2$B2s$3$J$$(B.
							 $B$=$s$J8m$C$?%G!<%?$O$9$F$k(B*/
							fprintf( stderr , "Bad SYN code\n");
						} else {
							/* SYN$B$N(BACK$B$G$"$k$+$i(Bsender$B$7$J$$(B */
							stb[index].read_syn = 1;
							fprintf( stderr , "fd%d read_syn on!\n",index );
						}
					}
				} else if( ack == ACK_NOUSE ){
					/* $B%G!<%?$G$"$k!#(B*/
					int i;
					if( index == -1 ){
						/* $BB8:_$7$J$$%=%1%C%H$K$?$$$7$F%G!<%?$,$/$k$3$H$O$J$$!#(B */
						fprintf( stderr, "Bad Data\n" );
					} else {
						/* $B$9$G$K3NN)$7$F$$$k%3%M%/%7%g%s$N$?$a$N%G!<%?$@$+$i!"(Breadbuf$B$K$/$o$($k!#(B */
						int ret = addrb( index , buf + HEADERSIZE ,seq , length );
						if( ret == 0 ){
							/* $BFI$_$3$_@.8y$7$?$+$i(BACK$B$+$($9!#(B
							 $BD9$5$b$=$N$^$^JV$9!#(B*/
							makeheader( buf , 0xffffffff , seq , 0 , 0, 0 ,0 ,length );
							sender( buf , HEADERSIZE , sin.sin_addr.s_addr );
						} else {
							/* $B<:GT$7$?$+$i(BACK$B$+$($5$J$$(B($B:FAw$5$;$s$H(B) */
						}
					}
				} else {
					/* ACK$B$G$"$k(B. */
					/* ACK$B$r$&$1$H$C$?$i!"(B ACK$B$NCM$r$b$D(Bwritebuf$B$,$"$k$+8!:w(B */
					if( index == -1 ){
						/* $BB8:_$7$J$$%=%1%C%H$K(BACK$B$-$F$b0UL#$J$7!#L5;k$9$k(B */
						fprintf( stderr , "Bad ack code %\n" );
					} else{
						/* $B$=$N$^$^JV$C$F$-$?D9$5$,$3$3$K@8$+$5$l$k!#(B
						   ACK$B%Q%1%C%H$N>l9g$@$1!"(Blength$BJ,$N%G!<%?$r$b$?$J$$(B */
						setack( index , ack, length );
					}
				}
			}
			rcount ++;
			fprintf( stderr, "R ");
		}

		/* $B$b$&FI$_$3$`$b$N$,$J$/$J$C$F!"=q$-$3$`$b$N$b$J$/$J$C$?$iJV$9(B */
		if( read_ok == 0 && write_more == 0 ){
/*			fprintf( stderr , "%d %d\n" , read_ok , write_more );*/
			break;
		}
	}

	return rcount;
}



/*******************************************************************************/
/*
   read$BFI$_$3$s$@$H$-$N$7$g$j(B

   $BFI$_$3$_%m%0$r(BRLOG_ACKOK$B$K$9$k(B
   $B@.8y$7$?$i(B0$B$r$+$($9!#(B

   start$B$+$i(Blen$BJ,$@$1(B.
   
 */
int addrb( int fd, char *buf , unsigned long start ,unsigned long len )
{
	int i , ai;
	
	CHECKFD(fd);

	ai = start - stb[fd].rs_top;
	if( ai < 0 || ai >= BUFFERSIZ ){
		return -1;
	}
	if( ( ai + len )>= BUFFERSIZ ){
		return -1;
	}
	for(i=ai;i<ai+len;i++){
		stb[fd].rb[i]=buf[i-ai];
		stb[fd].rlog[i]=RLOG_ACKOK;
	}
	stb[fd].rb_use += len;
	return 0;
}

/*
   ack$B$,$-$?$H$-$N=hM}(B
*/
static int setack( int fd , unsigned long ack_start , unsigned long ack_length )
{
	int i;
	int ai;
	
	CHECKFD( fd );

	ai = ack_start - stb[fd].ws_top;
	if( ai < 0 || ai >= BUFFERSIZ ){
		/* $B$=$j$c$*$+$7$$$J$j!#(B*/
		return -1;
	}
	if( (ai+ack_length) >= BUFFERSIZ ){
		return -1;
	}
	
	for(i=ai;i<ai+ack_length;i++ ){
		stb[fd].wlog[i] = WLOG_ACKOK;
	}

	/* ai$B$,(B0$B$N$H$-$O!"%P%C%U%!!<$N:G=i$NItJ,$r(BACK$B$G$-$?$H$$$&$3$H$@$+$i!"(B
	   $B%7%U%H$7$F$h$$!#(B */
	if( ai == 0 ){
		stb[fd].wb_use -= ack_length;
		shift( stb[fd].wb , ack_length ,BUFFERSIZ );
		shift( stb[fd].wlog , ack_length ,BUFFERSIZ );
		memset( stb[fd].wlog + ack_length , WLOG_NOUSE , BUFFERSIZ - ack_length );
	}
	
	return 0;
}

/*
   index$B$r%"%I%l%9$+$iF@$k(B
 */
static int getindexbyaddr( unsigned long addr )
{
	int i;
	int index = -1;		
	for(i=0;i<MAXCON;i++){
		/* $BAw$j<j$N(BIP$B$,$*$J$8(B */
		if( stb[i].use &&
		   stb[i].addr_dest == addr ){
			index = i;
			break;
		}
					
	}
	fprintf( stderr , "INDEX:%d\n" , index );

	return index;
}
/*
   $B$"$?$i$7$$%=%1%C%H$rM=Ls(B
 */
static int newsock( int fd )
{
	CHECKFDNUM(fd);

	stb[fd].use = 1;
	stb[fd].listen = 0;
	stb[fd].acceptwait = 0;
	stb[fd].read_syn = 0;
	stb[fd].addr_dest = 0;
	stb[fd].wb_use = 0;
	stb[fd].rb_use = 0;
	stb[fd].ws_top = 0;
	stb[fd].rs_top = 0;
}

/*
   $B=q$-$3$s$@$H$-$N=hM}(B
 */
static int setsend( int fd , unsigned long start , unsigned long end )
{
	int i;

	CHECKFD(fd);


	for(i=start;i<end;i++){
		stb[fd].wlog[i] = WLOG_ACKWAIT;
	}
	return 0;
		
}

/*
   $B$"$k(Bfd$B$r;XDj$7$F!"%M%C%H%o!<%/$K=q$-$3$_$?$$%G!<%?$N<!$N$+$?$^$j$N(B
   $B0LCV$r65$($k!#%?%$%`%"%&%H$N=hM}$b$3$3$G$G$-$k!#(B
   $B=q$-$3$_$?$$<!$N$+$?$^$j$H$O!"(BWLOG_SENDWAIT$B$N>uBV$N>l=j$N$3$H$G$"$k!#(B

   $B$=$l$KD9$5$N@)8B$r(BSEGMENTSIZ$B$G$+$1$k$N$G!"(B
   

   
 */
static int getwfdpart(int fd , unsigned long *start , unsigned long *end ,unsigned long siz)
{
	int i;
	int find=0;

	CHECKFD(fd);

	for(i=0;i<BUFFERSIZ;i++){
		if( find == 0 && stb[fd].wlog[i] == WLOG_SENDWAIT ){
			*start = i;
			find = 1;
		}
		if( find == 1 && stb[fd].wlog[i] != WLOG_SENDWAIT ){	/* WLOG_NOUSE$B$@$m$&(B */
			*end = i;
			if( ( *end - *start ) > siz ){
				*end = *start + siz;
			}
			udptcp.errno = 0;
			return 0;
		}
	}

	if( find == 1 ){
		*end = i;
		udptcp.errno = 0;
		return 0;
	} else {
		*start = *end = BUFFERSIZ;
		udptcp.errno = 0;
		return -0;
	}
}

/*
   $B<u?.%m%0$r$_$F!"%9%H%j!<%`$,$I$3$^$G40@.$7$F$$$k$N$+$r$7$i$Y$k(B
   $B$+$J$i$:M-8z$JCM$r$+$($9!#(B
 */
static unsigned long getrsend( int fd )
{
	unsigned long end = 0;
	unsigned long now = stb[fd].rs_top;
	int i ,j ;

	CHECKFD(fd);
	
	for(i=0;i<BUFFERSIZ;i++){
		if( stb[fd].rlog[i] != RLOG_ACKOK ){
			break;
		}
	}
	return now + i;
}


/*
   $B%=%1%C%H$,=i4|2=$5$l$F$k$+$I$&$+$7$i$Y$k(B

   return values:
   0 Not yet
   1 OK
   -1 Error
   
 */
static issockinited( int fd )
{
	CHECKFD(fd);

/*	fprintf( stderr, "ISSINI: fd:%d syn:%d\n",
			fd,stb[fd].read_syn );*/
	
	if( stb[fd].read_syn ){
		return 1;
	} else {
		return 0;
	}

}





/*
   ipaddr: htonl$B8e$N%G!<%?(B


   $B$3$l$O$G$-$?!#(B
   
 */
static int sender( char *data , int len , unsigned long ipaddr )
{

	struct sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons( UDPPORT );
	to.sin_addr.s_addr = ipaddr;

	fprintf( stderr, "Sender: addr:%x len:%d\n" , ipaddr , len );
	return sendto( udptcp.mainsockfd , data, len , 0,(struct sockaddr*)&to, sizeof( struct sockaddr ) );


}


static  void getheader( char *data ,
					   unsigned long *seq ,
					   unsigned long *ack ,
					   unsigned char *rst ,
					   unsigned char *syn ,
					   unsigned char *fin ,
					   unsigned char *ref ,
					   unsigned long *length)
{


	*seq = ntohl( *(long*)(data + SEQ_OFS) );
	*ack = ntohl( *(long*)(data + ACK_OFS) );
	*rst = *( (char*)(data + RESET_OFS ) );
	*syn = *( (char*)(data + SYNCRONIZE_OFS ) );
	*fin = *( (char*)(data + FINISH_OFS ) );
	*ref = *( (char*)(data + REFUSE_OFS ) );
	*length = ntohl( *(long*)(data + LENGTH_OFS) );
	
	fprintf( stderr, "GETH (host)SEQ:%d ACK:%d RST:%d SYN:%d FIN:%d REF:%d LEN:%d\n",
				 *seq , *ack , *rst , *syn, *fin , *ref, *length );
}
static void makeheader( char* data,
					   unsigned long seq ,
					   unsigned long ack ,
					   unsigned char rst ,
					   unsigned char syn ,
					   unsigned char fin ,
					   unsigned char ref ,
					   unsigned long length )
{
	fprintf( stderr, "MAKEH (host) SEQ:%d ACK:%d RST:%d SYN:%d FIN:%d REF:%d LEN:%d\n",
				 seq , ack , rst , syn, fin , ref, length );


	seq = htonl( seq );
	ack = htonl( ack );
	length = htonl( length );
	
	bcopy((char*)&seq , (char*)(data + SEQ_OFS) , sizeof(unsigned long) );
	bcopy((char*)&ack , (char*)(data + ACK_OFS) , sizeof(unsigned long) );
	bcopy((char*)&rst , (char*)(data + RESET_OFS) , sizeof(unsigned char) );
	bcopy((char*)&syn , (char*)(data + SYNCRONIZE_OFS) , sizeof(unsigned char) );
	bcopy((char*)&fin , (char*)(data + FINISH_OFS) , sizeof(unsigned char) );
	bcopy((char*)&ref , (char*)(data + REFUSE_OFS) , sizeof(unsigned char) );
	bcopy((char*)&length ,(char*)(data + LENGTH_OFS) , sizeof(unsigned long) );	
}



/*
   double$B$N(Bgettimeofday
   
 */
static double getUTimeDouble(void)
{
	struct timeval tm;

	gettimeofday( &tm , NULL );

	return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}

/*
   shift buffer
*/
static void shift( char *dat , int len ,int max)
{
	int i;
	for(i=len;i<max;i++){
		dat[i-len]=dat[i];
	}
}
static void memset( char *dat , int data , int len )
{
	int i;
	for(i=0;i<len;i++){
		dat[i]=data;
	}
}
				   
void ustat(void)	
{
	int i;

	for(i=0;i<MAXCON;i++){
		if( stb[i].use ){
			int k;
			fprintf( stderr ,
					"[%d] USE:%d READSYN:%d LISTEN:%d ACPW:%d AD_DEST:%x "
					"R_ADDR:%d W_ADDR:%d\n",
					i , stb[i].use ,stb[i].read_syn ,stb[i].listen ,
					stb[i].acceptwait , 
					stb[i].addr_dest ,
					stb[i].rs_top,stb[i].ws_top  );
#if 0
			for(k=0;k<WINDOWCOUNT;k++){
				fprintf( stderr ,"%d:RLEN:%d WLEN:%d RADDR:%d WADDR:%d\n",
						k,
						stb[i].readbuflen[k] ,
						stb[i].writebuflen[k] ,
						stb[i].readbufaddr[k] ,
						stb[i].writebufaddr[k] );
			}
#endif
			
		}
	}
}
