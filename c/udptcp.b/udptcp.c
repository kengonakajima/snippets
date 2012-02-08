#include <stdio.h>

#include "udptcp.h"


/*

   $BMQ0U$9$k%$%s%?!<%U%'%$%9(B


   ufd fd = usocket( );  $B%=%1%C%HI=$r$R$H$DKd$a$k(B

   ulisten( ufd fd);
   uaccept( ufd fd , unsigned long *cliaddr , unsigned short *port );
   uwrite( ufd fd, char *buf , int siz );
   uread( ufd fd , char *buf , int siz );
   uclose( ufd fd );
   uconnect( ufd fd , addr,port)

   uinit(ul) UDP$B$N%=%1%C%H$r=i4|2=$9$k!#(B
   uproc();     $B%M%C%H%o!<%/(BIO$B$r;H$&$H$3$m(B($B$3$l$rIQHK$K8F$S$@$9I,MW$,$"$k!#(B)

   $B%/%i%$%"%s%H$,@\B3$r$7$h$&$H$7$F!"%5!<%P!<$,$=$l0J>eB?$/$N@\B3$,$G$-$J$$(B
   $B$H$$$&>uBV$K$J$C$?$i!"(BUDp$B$N%Q%1%C%H$r$+$($7$F!"(BREF$B%S%C%H$r$?$F$k!#(B
  REF$B%S%C%H$,$?$C$F$?$i!"%/%i%$%"%s%H$N(Bconnect$B$O<:GT$9$k!#(B
   connect$B$O(BSYN$B%S%C%H$,$?$C$F$$$k%G!<%?$N$J$$%Q%1%C%H$rAw?.$9$k!#(B


   $B%/%i%$%"%s%H$,(Bconnect$B$9$k$H(BSYN$B$r$*$/$k!#$=$l$r$&$1$H$k$H(BSYN$B$rJVEz$9$k$H$I$&$8$K(Bread_syn$B$r$?$F$k!#(B
   $B%/%i%$%"%s%H$O$=$NJVEz$r$&$1$H$k$H!"(Bread_syn$B$rN)$F$k!#$=$l$,(Bconnect$B$G$"$k!#(B
   connect$B$5$l$F$$$k$"$$$@$O!"%5!<%P!<$b%/%i%$%"%s%H$b(Bread_syn$B$,$?$C$F$$$k$o$1$@!#(B
   
   


   
*/


/*   UDPTCP$B$N%X%C%@!<MQDj?t(B */
#define SRC_PORT_OFS 	0		/* unsigned short htons() $BAw?.85%]!<%H(B */
#define DEST_PORT_OFS 	2		/* unsigned short htons() $B08@h%]!<%H(B */
#define SEQ_OFS			4		/* ulong	htonl() $BAw?.MQ%7!<%1%s%9HV9f(B */
#define ACK_OFS			8		/* ulong 	htonl() $B1~Ez3NG'HV9f(B */
#define RESET_OFS		12		/* char $B:FAw$7$F$bDL?.$r2sI|$G$-$J$+$C$?!#$b$&$d$a$8$c!*(B $B%3%^%s%I(B*/
#define SYNCRONIZE_OFS	13		/* char 1$B$,;XDj$5$l$?$i!"%7!<%1%s%9HV9f$r(B0$B$K=i4|2=(B */
#define FINISH_OFS		14		/* char 1$B$@$C$?$i=*N;=hM}$7$F!"%G!<%?$rAw?.$7$-$C$F$7$^$&!#(B */
#define REFUSE_OFS		15		/* char 1$B$G@\B3$r5qH]$9$k$N$@!#(B(backlog$B$,$$$C$Q$$(B)
								   $B$N$3$j$rA4ItAw?.$G$-$?$i(BFIN$B$r;XDj$7$?%Q%1%C%H$rAw?.$9$k!#(B */
#define LENGTH_OFS		16		/* htonl() $B%G!<%?$ND9$5(B */
#define DATA_OFS		20		/* $B%G!<%?$N$O$8$^$k0LCV(B */
#define HEADERSIZE		DATA_OFS/* $B%X%C%@!<$N%5%$%:(B */



struct usock
{
	int use;             
	int listen;       /* listen$B$9$k$+$I$&$+(B */
	int acceptwait;		/* accept$B$5$l$k$N$rBT$C$F$$$k$J$i(B1$B!#(B */
	int read_syn;		/* SYN$B%3%^%s%I$r<u$1$H$C$?$+(B */
	
	unsigned long addr_dest;	/* $BAj<j$N(BIP htonl$B$5$l$?$"$H$N%G!<%?(B */	
	unsigned short port_dest;	/* $BAj<j$N%]!<%HHV9f(B(index) ,htons$B$5$l$?$"$H$N$G!<$?(B*/

	unsigned long read_addr;		/* $B2?%P%$%H=q$-$3$a$?$+(B */
	unsigned long write_addr;	

#define WINDOWCOUNT 8
#define ELEMENTSIZE 512			/* $B%P%C%U%!!<%(%l%a%s%H$N(Bsize*/
	
	int readbuflen[WINDOWCOUNT];		/* readbuf/writebuf$B$r;H$C$F$k$+$I$&$+(B */
	int writebuflen[WINDOWCOUNT];		
	unsigned long readbufaddr[WINDOWCOUNT];		/* read/write$B%P%C%U%!$,2?%P%$%HL\$+$iMQ$N%G!<%?$J$N$+$r5-21(B */
	unsigned long writebufaddr[WINDOWCOUNT];	
	char readbuf[WINDOWCOUNT][ELEMENTSIZE];		/* $BFI$_$3$_%P%C%U%!!<!#(B*/
	char writebuf[WINDOWCOUNT][ELEMENTSIZE];		/* $B=q$-$3$_%P%C%U%!!<!#(B */
	unsigned long writebuf_sent[WINDOWCOUNT];		/* $B=q$-$3$s$@$i$3$N%U%i%0$r$?$F!"(Back$B$^$A$r$9$k(B */
	struct timeval writebuf_sent_when[WINDOWCOUNT];	/* $B$=$l$>$l$N=q$-$3$_%P%C%U%!$G!"$$$DAw?.$7$?$+(B */
};

#define MAXCON 16
struct usock socktable[ MAXCON ];  /* $B:GBg$N@\B3(B */

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



static issockinited( int fd );
static double getUTimeDouble(void);
static int sender( char *data , int len , unsigned long ipaddr );
static  void getheader( char *data ,
					   unsigned short *sport ,
					   unsigned short *dport ,
					   unsigned long *seq ,
					   unsigned long *ack ,
					   unsigned char *rst ,
					   unsigned char *syn ,
					   unsigned char *fin ,
					   unsigned char *ref ,
					   unsigned long *length);
static void makeheader( char* data,
					   unsigned short sport ,
					   unsigned short dport ,
					   unsigned long seq ,
					   unsigned long ack ,
					   unsigned char rst ,
					   unsigned char syn ,
					   unsigned char fin ,
					   unsigned char ref ,
					   unsigned long length );
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
	i= bind( udptcp.mainsockfd , (struct sockaddr*)& sin , sizeof( sin ));

	fprintf( stderr, "bind: %d %x\n", i , ipaddr );
	for(i=0;i<MAXCON;i++){
		bzero( &socktable[i] , sizeof( socktable[0]) );
	}
	
	udptcp.ipaddr = ipaddr;
	
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
			int i;

			/* $B%=%1%C%H$K%G!<%?$,$?$^$C$F$?$i=q$-$3$`!#$=$7$F$^$@%G!<(B */
			/* $B%?$,$"$k$h$&$@$C$?$i(Bwrite_more$B$r(B1$B$K$9$k(B */

			for(i=0;i<MAXCON;i++){
				int j;
				for(j = 0;j<WINDOWCOUNT;j++){
					if( socktable[i].writebuflen[j] > 0 ){
						int sr;	
						char tmpbuf[ELEMENTSIZE+HEADERSIZE];
						fprintf( stderr, "W \n");
						makeheader( tmpbuf ,
								   i , 	/* index$B$,%]!<%H$K$J$k$N$@(B */
								   socktable[i].port_dest ,
								   socktable[i].writebufaddr[j] ,
								   ACK_NOUSE ,
								   0 , 0 , 0 , 0 ,
								   socktable[i].writebuflen[j] );
						bcopy( socktable[i].writebuf[j] ,tmpbuf + HEADERSIZE , socktable[i].writebuflen[j] );
						
						sr = sender( tmpbuf , socktable[i].writebuflen[j] +HEADERSIZE , socktable[i].addr_dest );
						
						if( sr != (socktable[i].writebuflen[j]+HEADERSIZE) ){
							break;
						} else {
							/* $B$?$@$7$/=q$-$3$a$?$,!":FAw$N$?$a$,$"$k$+$i%G!<%?$O>C$;$J$$!#(B */


							socktable[i].writebuf_sent[j] = 1;
							gettimeofday( & socktable[i].writebuf_sent_when[j] , NULL );
#if 1
							/* $B:FAw$7$J$$>l9g$O$3$3$G%G!<%?$r$1$7$F$h$$!#(B(length$B$r(B0$B$K$9$k$@$1(B) */
							socktable[i].writebuf_sent[j]= 0;
							socktable[i].writebuflen[j] = 0;
#endif
							
							break;
						}
					}
				}
			}
			write_more = 0;		/* $B:#$O$$$D$G$b(Bwrite_more = 0 PENDING */
		}

		/* $BFI$_$3$_2DG=(B */
		if( FD_ISSET( udptcp.mainsockfd  , &rfd )){
			int n;
			char buf[ELEMENTSIZE+HEADERSIZE];
			struct sockaddr_in sin;
			int clilen;
			read_ok = 1;

			n = recvfrom( udptcp.mainsockfd , buf , sizeof( buf ) , 0 , (struct sockaddr*)&sin , &clilen);
			if( n< 0 ) continue;
			fprintf( stderr , "recvfrom addr:%x\n", sin.sin_addr.s_addr );
			if( n > 0 ){
				
				/* $B%Q%1%C%H$rFI$_$3$`$3$H$,$G$-$?!#(B ack$B%U%#!<%k%I$K%G!<%?$,$"$k>l9g$O(Back$B$N=hM}$r$9$k!#(B
				 $B%G!<%?$,(B0xffffffff$B$N>l9g$O%G!<%?$G$"$k!#(B*/
				int index;
			   	unsigned short sport,dport;
			   	unsigned long seq , ack;
			   	unsigned char rst,syn,fin,ref;
			   	unsigned long length;
				getheader( buf , &sport , &dport , &seq , &ack , &rst , &syn , &fin , &ref ,&length );

				
				/* $BBP1~$9$k(Bindex$B$r%]!<%H$N>pJs$+$iF@$k!#$J$$$H$-$O(Baccept$BBT$A9TNs$KDI2C$9$k!#(B */
				index = -1;		
				for(i=0;i<MAXCON;i++){
					/* $BAw$j<j$N(BIP$B$,$*$J$8$G$+$D!"(BUDPTCP$B%]!<%H$b$*$J$8(B */
/*					fprintf( stderr , "STbl[%d] %d: addr:%x port:%d cliaddr:%x sport:%d\n" ,
							i, socktable[i].use,
							socktable[i].addr_dest ,
							socktable[i].port_dest , sin.sin_addr.s_addr , sport );*/
					if( socktable[i].use &&
					    socktable[i].addr_dest == sin.sin_addr.s_addr &&
					    socktable[i].port_dest == dport ){
						/* dport$B$,85$N$3$A$i$N%]!<%H$G(Bsport$B$,$`$3$&$G(B */
						/* $B$o$j$"$F$i$l$?$"$?$i$7$$%]!<%H$G$"$k(B */
						index = i;
						break;
					}
					
				}
				fprintf( stderr , "INDEX:%d sp:%d dp:%d\n" , index ,sport , dport );
			
				if( syn ){
					/* SYN$B$G$"$k!#(B */
					if( index == -1 ){
						fprintf( stderr, "SYN!\n");
						/* $B:#$^$G$K$J$$%"%I%l%9$@(B($B%5!<%P!<$K$H$C$F$OEvA3(B)$B!#(Baccept$BBT$A%=%1%C%H$r$D$$$+(B */
						for(i=3;i<MAXCON;i++){
							if( socktable[i].use == 0 ){
								bzero( &socktable[i] , sizeof( socktable[0] ) );
								socktable[i].use = 1;
								socktable[i].listen = 0;
								socktable[i].acceptwait = 1;
								socktable[i].read_syn = 1;	/* syn$B$,$-$?$+$i(B */
								socktable[i].addr_dest = sin.sin_addr.s_addr;
								socktable[i].port_dest = sport;	/* $BAw$j<j$O!"(Bsport$B$K%]!<%HHV9f(B(index)$B$rF~$l$FAw?.$7$F$/$k$N$@(B */
								socktable[i].read_addr = 0;
								socktable[i].write_addr = 0;
								
								break;
							}
						}
						if( i == MAXCON ){
							/* $B$3$l0J>e2?$b$G$-$J$$$+$i%Q%1%C%H$r<N$F$k!#(B
							   $B%/%i%$%"%s%H$+$i$O%5!<%P!<$,1~Ez$7$J$$$h$&$K8+$($k(B
							   $B$+$i!"%?%$%`%"%&%H$9$k$3$H$@$m$&!#(B*/
						} else {
							/* SYN$B$r$+$($9(B */
							makeheader( buf , i, sport , 0 , 0, 0 , 1 , 0, 0 ,0 );
							sender( buf , HEADERSIZE , sin.sin_addr.s_addr );		   
						}
						
						
					} else {
						/* $B:#$^$G$KB8:_$7$F$$$?%=%1%C%H$K(BSYN$B$,$-$?$i!"(B */
						/* $B$=$l$O%/%i%$%"%s%H$G$OEvA3$G$"$k(B */
						if( ack == ACK_NOUSE ){
							/* ACK_NOUSE$B$O%G!<%?Aw?.$N$H$-$K$D$+$&$N$G(BSYN$B$O(B2$B2s$3$J$$(B.
							 $B$=$s$J8m$C$?%G!<%?$O$9$F$k(B*/
							fprintf( stderr , "AA\n");
						} else {
							/* SYN$B$N(BACK$B$G$"$k$+$i(Bsender$B$7$J$$(B */
							socktable[index].read_syn = 1;
							fprintf( stderr , "fd%d read_syn on!\n",index );
							fprintf( stderr , "SS\n");
						}
					}
				} else if( ack == ACK_NOUSE ){
					/* $B%G!<%?$G$"$k!#(B*/
					int i;
					if( index == -1 ){
						/* $BB8:_$7$J$$%=%1%C%H$K$?$$$7$F%G!<%?$,$/$k$3$H$O$J$$!#(B */
					} else {
						/* $B$9$G$K3NN)$7$F$$$k%3%M%/%7%g%s$N$?$a$N%G!<%?$@$+$i!"(Breadbuf$B$K$/$o$($k!#(B */
						for(i=0;i<WINDOWCOUNT;i++){
							if( socktable[index].readbuflen[i]==0){
								int ii;
								bcopy( buf + HEADERSIZE , socktable[index].readbuf[i] , length );
								socktable[index].readbufaddr[i] = seq;
								socktable[index].readbuflen[i] = length;
								fprintf( stderr,"AddReadBuf: ind:%d len:%d seq:%d\n",index,length,seq);

/*								for(ii=0;ii<WINDOWCOUNT;ii++){
									fprintf( stderr, "ADDREADB:fd:%d addr:%d len:%d raddr:%d\n" ,
											index,
										socktable[index].readbufaddr[ii] ,
										socktable[index].readbuflen[ii] ,
										socktable[index].read_addr );
								}*/
								
								break;
							}
						}
					}
					if( i == WINDOWCOUNT ){
						/* $B%&%$%s%I%&$,$^$s%?%s$G<u$1$H$k$3$H$,$G$-$J$+$C$?!#(B
						 $B$@$+$i(BACK$B$O$+$($5$:!"%G!<%?$r$9$F$k(B */
					} else {
						/* $B%G!<%?MQ(BACK$B$r$+$($9(B */
						makeheader( buf , index , sport , 0xffffffff ,
								   seq /* seq$B$r(BACK$B$KF~$l$FJV$9(B */ , 0 , 0, 0 ,0 ,0 );
						sender( buf , HEADERSIZE , sin.sin_addr.s_addr );
					}

				} else {
					/* ACK$B$G$"$k(B. */
					/* ACK$B$r$&$1$H$C$?$i!"(B ACK$B$NCM$r$b$D(Bwritebuf$B$,$"$k$+8!:w$7$F!"$"$C$?$i$=$N%P%C%U%!!<$r$1$9!#(B */
					if( index == -1 ){
						/* $BB8:_$7$J$$%=%1%C%H$K(BACK$B$-$F$b0UL#$J$7!#L5;k$9$k(B */
					} else{
						for(i=0;i<WINDOWCOUNT;i++){
							if( socktable[index].writebufaddr[i] == ack ){
								socktable[index].writebuflen[i] = 0;
								break;
							}
						}
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
		if( ! socktable[i].use  ){
			bzero( &socktable[i] , sizeof( socktable[0]  ) );
			socktable[i].use = 1;
			socktable[i].listen = 0;
			socktable[i].acceptwait = 0;
			socktable[i].read_syn = 0;
			socktable[i].addr_dest = 0;
			socktable[i].port_dest = 0;
			socktable[i].read_addr = 0;
			socktable[i].write_addr = 0;
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
	if( index < 0 || index >= MAXCON ){
		udptcp.errno = EBADFDNUM;
		return -1;
	}
	
	if( ! socktable[index].use  ){
		bzero( &socktable[index] , sizeof( socktable[0]  ) );
		socktable[index].use = 1;
		socktable[index].listen = 0;
		socktable[index].acceptwait = 0;
		socktable[index].read_syn = 0;
		socktable[index].addr_dest = 0;
		socktable[index].port_dest = 0;
		socktable[index].read_addr = 0;
		socktable[index].write_addr = 0;
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
	if( fd < 0 || fd >= MAXCON || socktable[fd].use == 0 ){
		udptcp.errno = EBADFDNUM;
		return -1;
	}

	socktable[fd].listen = 1;

	return 0;

}

/*
   usock$B$G%"%/%;%W%H(B

   int fd : socket$BI=$X$N(Bindex
   unsigned long cliaddr : $B%/%i%$%"%s%H$N(BIP$B%"%I%l%9(B
   unsigned short port : $B%/%i%$%"%s%H$N(BUDPTCP $B%]!<%H(B


   fd$B$O(Blisten$B$7$F$$$J$1$l$P$J$i$J$$!#(B
   

   return value:
   >= 0 OK
   <0 Error
   

   $B%=%1%C%HI=$+$i8!:w$7$F!"(Bacceptwait$B$N>uBV$K$J$C$F$$$k$N$r$5$,$9(B

   cliaddr $B$O(B htonl$B8e!"(B cliport $B$O(Bhtonl$B8e$N%G!<%?$K$J$C$F$$$k!#(B
   
   
 */
int uaccept( int fd , unsigned long *cliaddr,  unsigned short *cliport )
{
	int i;
	
	if( fd < 0 || fd >= MAXCON || socktable[fd].use == 0 ){
		udptcp.errno = EBADFDNUM;
		return -1;
	}

	if( ! socktable[fd].listen ){
		udptcp.errno = ENOLISTEN;
		return -1;
	}
	
	for(i=0;i<MAXCON;i++){
		if( socktable[i].use && socktable[i].acceptwait ){
			socktable[i].acceptwait = 0;
			*cliaddr = socktable[i].addr_dest;
			*cliport = socktable[i].port_dest;
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

   $B@\B3$9$k!#<B:]$N(BUDP$BMQ$N%"%I%l%9$O!"(B (addr , htons( port))$B$G$"(B
   $B$k!#(B

   addr$B$NCM$O!"(B htonl$B8e$NCM$G$"$k!#(B
   port$B$b$=$&(B

   return values:
   0 OK
   <0 Error

   uproc$B$O!"2?$+%Q%1%C%H$,$-$?$iJV$jCM$G$7$i$;$k$N$G!"(B
   uproc$B$,2?$+$&$1$H$k$?$S$K%F%9%H$7$F!"(Back$B$,$-$?$i(Bconnect$B@.8y!"(B
   $B$H$$$&$h$&$K$7$J$1$l$P$J$i$J$$!#%V%m%C%/$9$k!#(B
   
*/
int uconnect( int fd , unsigned long addr , unsigned short port )
{

	int sr;
	char tmpdata[ELEMENTSIZE];
	
	if( fd < 0 || fd >= MAXCON || socktable[fd].use ==0){
		udptcp.errno = EBADFDNUM;
		return -1;
	}

	/* $B<+J,$N@_Dj$r$9$k(B */
	socktable[fd].addr_dest = addr;
/*	socktable[fd].port_dest = port;  $B%]!<%H$O<u?.B&$,<+F0$G$o$j$U$k(B*/
	
	/* connect$BMQ$N%Q%1%C%H$rAw?.!#%X%C%@$N$_(B */
	makeheader( tmpdata, 
			   fd ,
			   ntohs(port) ,	/* $B$3$3$,%$%d(B */
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
   read$B$9$k!#(Bfd$B$N%P%C%U%!$K$?$^$C$F$$$kJ,$rFI$_$3$`$@$1$J$N$G!"(Buproc$B$7$?$"$H$G$J$$$H2?$b(B
   $B5/$3$i$J$$!#(B
 */
int uread( int fd, char *data , int len )
{
	char tmpdata[ELEMENTSIZE];
	int i ,k;
	int read_total = 0;
	int found,found_once;
	if( fd <0 || fd >= MAXCON || socktable[fd].use ==0 ){
		fprintf( stderr , "AAAAAAAAho\n" );
		udptcp.errno = EBADFDNUM;
		return -1;
	}

	/* WINDOW$B$rA4It8!:w$7$F!"(Bread_addr$B$HF1$8(Baddr$B$r$b$D$b$N$,$"$C$?$i$=$l$+$i(B
	   $BJV$7$F$$$/!#(B read$B$,$*$o$C$?$i!"%P%C%U%!!<$r$1$7$F$7$^$&!#$N$G!"$+$J$i$:!"(B
	   read_addr$B$h$j$b?J$s$@$b$N$7$+;D$i$J$$(B */

	fprintf( stderr, "UREAD: fd:%d radr:%d\n",	fd , socktable[fd].read_addr );
	found_once = 0;
	while(1){
		found = 0;
		for(k=0;k<WINDOWCOUNT;k++){
			if( socktable[fd].readbufaddr[k] == socktable[fd].read_addr &&
			    socktable[fd].readbuflen[k] > 0
			   ){
				
				/* data$B$N:GBg$r%A%'%C%/!#1[$($F$?$i0lIt$r%3%T!<(B(len-read_total byte$B$@$1(B)
				 $B$7$F%P%C%U%!$r$$$8$k(B */
				if( read_total >= len ){
					break;
				}
				if( (read_total +socktable[fd].readbuflen[k]) >= len ){
					int copy_size = len - read_total;
					int a;
					fprintf( stderr , "copysiz:%d rtot:%d len:%d\n",copy_size , read_total ,len);
					bcopy( socktable[fd].readbuf[k] , data + read_total , copy_size );
					/*$B%P%C%U%!$r$:$i$7$FD9$5$rJQ99(B */
					for(a=(read_total+copy_size);a<socktable[fd].readbuflen[k];a++){
						socktable[fd].readbuf[k][a-(read_total+copy_size)] =
						socktable[fd].readbuf[k][a];
					}
					socktable[fd].readbuflen[k] -= copy_size;
					read_total += copy_size;
					found = 1;
					found_once = 1;
/*					fprintf( stderr ,"Data:[%s] after:[%s]\n", data ,socktable[fd].readbuf[k] );*/
					break;
				} 
				/* $B%3%T!<(B */
				fprintf( stderr, "COPPPPPPPPPY\n");
				bcopy( socktable[fd].readbuf[k] , data + read_total , socktable[fd].readbuflen[k] );
				socktable[fd].read_addr += socktable[fd].readbuflen[k];
				
				read_total += socktable[fd].readbuflen[k];
				found = 1;
				found_once = 1;

				/* $B%P%C%U%!!<$r$1$9(B */
				socktable[fd].readbuflen[k]=0;


				break;
			}
		}
		if( found == 0 )break;
	}
	if( found_once == 0 ){
		udptcp.errno = EREADNODATA;
		return -1;
	}

	return read_total;
}

/*

   write$B$9$k!#(Bfd$B$N%P%C%U%!$K$?$^$k$@$1$J$N$G!"$3$N$"$H(Buproc$B$7$J$1$l$P$J$i$J$$!#(B

   return values:
   0 OK
   <0 Error
   */
int uwrite( int fd , char *data , int len )
{

	char tmpdata[ELEMENTSIZE];
	int i;
	int write_total = 0;
	
	if( fd <0 || fd >= MAXCON || socktable[fd].use ==0 ){
		udptcp.errno = EBADFDNUM;
		return -1;
	}


	/* $B%5%$%:$,JQ(B */
	if(  len < 0 ){
		udptcp.errno = EBADDATALEN;
		return -1;
	}


	/* $B%=%1%C%H$N%P%C%U%!$K$?$9(B */

	for(i=0;i<WINDOWCOUNT;i++){
		if( socktable[fd].writebuflen[i] == 0 ){
			/* $B%P%C%U%!$,$"$$$F$?(B */
			int copysize=0;
			if( (len-write_total) >= ELEMENTSIZE ){
				copysize = ELEMENTSIZE;
			} else {
				copysize = (len-write_total);
			}
			
			bcopy( (char*)(data + write_total) , (char*)socktable[fd].writebuf[i] , copysize );
			socktable[fd].writebuflen[i] = copysize;
			socktable[fd].writebufaddr[i] = socktable[fd].write_addr;
			socktable[fd].write_addr += copysize;
			write_total += copysize;
			
			fprintf( stderr , "appended %d bytes to writebuf %d write_addr:%d\n" ,
						copysize , i ,socktable[fd].writebufaddr[i] );
			if( write_total == len ){
				/* len $B$HF1$8$@$1=q$-$3$a$?$i(Bbreak */
				break;
			}
		}
	}
	udptcp.errno = 0;
	return write_total;
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
	
	if( fd < 0 || fd >= MAXCON || socktable[fd].use ==0){
		udptcp.errno = EBADFDNUM;
		return 0;
	}

	if( issockinited(fd ) != 1 ){
		udptcp.errno = ENOTINIT;
/*		fprintf( stderr, "AAAA %d\n",issockinited(fd));*/
		return 0;
	}

	/* uread$B$NCf$G$7$i$Y$k$N$HF1$8J}K!$G!"(Bread$B$G$-$k$b$N$,$"$k$+$7$i$Y(B */
	/* $B$k(B */

	for(i=0;i<WINDOWCOUNT;i++){
/*		fprintf( stderr, "UREADABLE: fd%d addr:%d len:%d raddr:%d\n" ,
				fd,
				socktable[fd].readbufaddr[i] ,
				socktable[fd].readbuflen[i] ,
				socktable[fd].read_addr );*/
		if( socktable[fd].readbufaddr[i] == socktable[fd].read_addr &&
		   socktable[fd].readbuflen[i] > 0 ){
			return 1;
		}
	}
	return 0;
}

/*******************************************************************************/
  
/*
   $B%=%1%C%H$,=i4|2=$5$l$F$k$+$I$&$+$7$i$Y$k(B

   return values:
   0 Not yet
   1 OK
   -1 Error
   
 */
static issockinited( int fd )
{
	if( fd < 0 || fd >= MAXCON ||socktable[fd].use ==0){
		udptcp.errno = EBADFDNUM;
		return -1;
	}


/*	fprintf( stderr, "ISSINI: fd:%d syn:%d\n",
			fd,socktable[fd].read_syn );*/
	
	if( socktable[fd].read_syn ){
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
					   unsigned short *sport ,
					   unsigned short *dport ,
					   unsigned long *seq ,
					   unsigned long *ack ,
					   unsigned char *rst ,
					   unsigned char *syn ,
					   unsigned char *fin ,
					   unsigned char *ref ,
					   unsigned long *length)
{

	*sport = ntohs( *(short*)(data + SRC_PORT_OFS) );
	*dport = ntohs( *(short*)(data + DEST_PORT_OFS) );
	*seq = ntohl( *(long*)(data + SEQ_OFS) );
	*ack = ntohl( *(long*)(data + ACK_OFS) );
	*rst = *( (char*)(data + RESET_OFS ) );
	*syn = *( (char*)(data + SYNCRONIZE_OFS ) );
	*fin = *( (char*)(data + FINISH_OFS ) );
	*ref = *( (char*)(data + REFUSE_OFS ) );
	*length = ntohl( *(long*)(data + LENGTH_OFS) );
	
	fprintf( stderr, "GETH (host)SP:%d DP:%d SEQ:%d ACK:%d RST:%d SYN:%d FIN:%d REF:%d LEN:%d\n",
				*sport , *dport , *seq , *ack , *rst , *syn, *fin , *ref, *length );
}
static void makeheader( char* data,
					   unsigned short sport ,
					   unsigned short dport ,
					   unsigned long seq ,
					   unsigned long ack ,
					   unsigned char rst ,
					   unsigned char syn ,
					   unsigned char fin ,
					   unsigned char ref ,
					   unsigned long length )
{
	fprintf( stderr, "MAKEH (host)SP:%d DP:%d SEQ:%d ACK:%d RST:%d SYN:%d FIN:%d REF:%d LEN:%d\n",
				sport , dport , seq , ack , rst , syn, fin , ref, length );

	sport = htons( sport );
	dport = htons( dport );
	seq = htonl( seq );
	ack = htonl( ack );
	length = htonl( length );
	
	bcopy((char*)&sport , (char*)(data + SRC_PORT_OFS) , sizeof(unsigned short ) );
	bcopy((char*)&dport , (char*)(data + DEST_PORT_OFS) , sizeof(unsigned short ) );
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

void ustat(void)	
{
	int i;

	for(i=0;i<MAXCON;i++){
		if( socktable[i].use ){
			int k;
			fprintf( stderr ,
					"[%d] USE:%d READSYN:%d LISTEN:%d ACPW:%d AD_DEST:%x PT_DEST:%d "
					"R_ADDR:%d W_ADDR:%d\n",
					i , socktable[i].use ,socktable[i].read_syn ,socktable[i].listen ,
					socktable[i].acceptwait , 
					socktable[i].addr_dest ,socktable[i].port_dest ,
					socktable[i].read_addr,socktable[i].write_addr  );
#if 0
			for(k=0;k<WINDOWCOUNT;k++){
				fprintf( stderr ,"%d:RLEN:%d WLEN:%d RADDR:%d WADDR:%d\n",
						k,
						socktable[i].readbuflen[k] ,
						socktable[i].writebuflen[k] ,
						socktable[i].readbufaddr[k] ,
						socktable[i].writebufaddr[k] );
			}
#endif
			
		}
	}
}
