
#include "udpc.h"

#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>





struct udpc
{
	int maxbacklog;
	int errno;
	int mainsockfd;
	unsigned short port;
}udpc;



struct vsock
{
	int use;
	int state;

#define VS_ACCEPTWAIT	100		/* accept$B$5$l$k$N$r$^$C$F$k>uBV$N$"$?$i$7$$@\B3(B */
#define VS_ALIVE		101		/* accept$B$5$l$?!#;H$($k!#(B */
#define VS_CONNECTING	102		/* connect$B=hM}Cf$G$"$k!#(B */
#define VS_FINWAIT		103		/* fin$B$rAw?.$9$k$N$r$^$C$F$$$k(B */
#define VS_FINACKWAIT	104		/* fin$B$N(Back$B$r$^$C$F$k(B */
	
	unsigned long ipaddr;	/* $BAj<j$N(B ipaddr */
	unsigned short dest_port;	/* $BAj<j$N(B port */
	int vfd_d;			/* $BAj<j$N(Bvfd */

	int recv_syn_ack;	/* SYN$B$N(BACK$B$r<u$1$H$C$?$i(BSYN$BAw?.B&$N$3$N%U%i%0(B */
						/* $B$,$?$?$&!#(B*/
	double fin_time;	/* FIN$B$rAw?.$7$?;~4V!#(Bfin$B$N:FAw$N$?$a$K;H$&(B */
	
	int write_addr;		/* $B:G?7$N(Bseq$B$N0LCV!#(B			   */
	int read_addr;
	
#define BUFNUM 	16
#define BUFLEN	512
	
	char sb[BUFNUM][BUFLEN];	/* send buffer */
	char rb[BUFNUM][BUFLEN];	/* recv buffer */
	int sbstate[BUFNUM];		/* buffer state for sb */
	int rbstate[BUFNUM];		/* buffer state for rb */
	double sbtime[BUFNUM];		/* for resending */
	int sbseq[BUFNUM];			/* starting point in the stream */
	int rbseq[BUFNUM];
	int sblen[BUFNUM];			/* length of each buffer 0$B$@$C$?$iL$;HMQ(B*/
	int rblen[BUFNUM];
	
#define SB_NODATA	0			/* sb$B$N>uBV(B */
#define SB_SENDWAIT	1
#define SB_ACKWAIT	2


#define RB_NODATA	0			/* rb$B$N>uBV(B */
#define RB_RECVOK	1	
	
};

#define MAXCON 128


double connectretry[]={ 4 , 6 , 8 , 12, 18 ,26 , 38 };	/* connect$B$N%j%H%i%$$N4V3V(B($BIC(B) */


struct vsock sock[MAXCON];


#define RST_BIT 0x01
#define SYN_BIT 0x02
#define FIN_BIT	0x04
#define REF_BIT	0x08

#define SEQ_OFS 0	/* int */
#define ACK_OFS 4	/* int $B$3$3$,(B-1$B$@$C$?$i(BACK$B$G$O$J$/!"(B-1$B0J30$@$C$?$i(BACK$B%Q%1%C%H$@(B*/
#define LENGTH_OFS 8	/* int */
#define CODE_OFS 12		/* char 3$B%P%$%H$O$&$a$&$a(B*/
#define SVFD_OFS 16		/* int $BAw$j<j$N(Bvfd*/
#define RVFD_OFS 20		/* int $B<u$1<j$N(Bvfd*/
#define HEADERSIZ 24


static double getUTimeDouble(void);
static void shift( char *dat , int len ,int max);
static void memset( char *dat , int data , int len );
static int sender( char *data , int len , unsigned long ipaddr , unsigned short port );
static void getheader( char *in , int *seq , int *ack , int *rst ,
					  int *syn , int*fin , int *ref , int *length ,int *svfd,int *rvfd);
static void makeheader( char *out , int seq , int ack , int rst , int syn , int fin 
					   , int ref, int length ,int svfd , int rvfd);
static int receivepacket( unsigned long ip , int vfd , int seq , char *buf , int len );
static int testnewsock( unsigned long ip , unsigned short port,int sender_vfd );
static int getblanksbindex( int vfd );
static int receiveack( int svfd , int ack );
static int getleastrbseq( int vfd );
static int appendbuf( char *src , int srclen , char *dest , int destofs , int destlen  );

#define VFDCHECK(vfd) if( (vfd)<0 || (vfd)>=MAXCON || sock[vfd].use ==0 ){return -1;}

/*
   close

   close$B$9$k$K$O!"(BFIN$B$rAw?.$9$k$N$r$^$D(BFINWAIT$B$N>uBV$K$J$k!#(B
   FIN$B$rAw?.$9$k$HF1;~$K(BFINACKWAIT$B$J>uBV$K$J$k!#(B
   FIN$B$r$&$1$H$C$?B&$OB(>CLG$9$k!#Aw?.$,$o$O(BFINACK$B$r$&$1$H$C$?$iB(>CLG$9$k!#(B
   

   
 */
int UDPCClose( int vfd )
{
	VFDCHECK( vfd );

	sock[vfd].state = VS_FINWAIT;
	
}

/*
   $BFI$_$3$_(B

   
 */
int UDPCRead( int vfd , char *buf , int len )
{
	int totallen = 0;
	int i,j;


	VFDCHECK( vfd );
	
	for(i=0;i<BUFNUM;i++){
		int least_seq;
		
		/* $B:G$b>.$5$$(Brbseq$B$r$b$H$a$k(B */

		least_seq = getleastrbseq( vfd );

		if( least_seq != -1 ){
			for(j=0;j<BUFNUM;j++){
				if( sock[vfd].rblen[j] > 0 &&
				    sock[vfd].rbstate[j] == RB_RECVOK &&
				    sock[vfd].rbseq[j] == least_seq ){

					int appended;
					
#if 0
					{
						char buffer[10000];
						bcopy( sock[vfd].rb[j] , buffer , sock[vfd].rblen[j] );
					    buffer[ sock[vfd].rblen[j] ] =0 ;
						fprintf( stderr ,"copy from buffer %d len:%d [%s]\n", j , 
							sock[vfd].rblen[j] ,buffer );
					}
#endif
					
					appended = appendbuf( sock[vfd].rb[j] , sock[vfd].rblen[j] ,
										 buf , totallen , len );
					fprintf( stderr , "appended:%d\n",appended );
					if( appended == sock[vfd].rblen[j] ){
						/* rblen$B$@$1(Bappend$B$G$-$?!#%P%C%U%!$r$1$7$F$7$^$&(B */
						sock[vfd].rbstate[j] = RB_NODATA;
						sock[vfd].rbseq[j] = 0;
						sock[vfd].rblen[j] -= appended;
					} else {
						/* len$B$@$1(Bappend$B$G$-$?!#%P%C%U%!$O$^$@$J$/$J$i$J$$(B */
						sock[vfd].rblen[j] -= appended;
						shift( sock[vfd].rb[j] , appended , BUFLEN);
					}
					totallen += appended;
					break;
				}
			}
			if( totallen == len ){
				break;
			}
		} else {
			/* $B:#$O(Bread$B$G$-$k%P%C%U%!$J$$(B */
			break;
		}
	}
	
	return totallen;
	
}
/*
   $B%P%C%U%!%"%Z%s%I(B

   $B<B:]$K(Bappend$B$7$?%P%$%H?t$r$+$($9(B

   src : $B$b$H(B
   srclen : $B$b$H$N%P%C%U%!$N$J$,$5(B
   dest : $B$5$-(B
   destofs : $B$5$-$N$*$U$;$C$H(B
   destlen : $B$5$-$N$5$$$@$$$J$,$5(B

   
*/
static int appendbuf( char *src , int srclen , char *dest , int destofs , int destlen  )
{
	int i;
	for(i=0;;i++){
		int d = destofs + i;
		if( d == destlen ) break;
		if( i == srclen ) break;
		dest[d] = src[i];
		
	}
	return i;
}
/*
   $BFI$_$3$_$9$k$3$H$,$G$-$k:G>.$N%"%I%l%9$N$b$N$N(Bseq$B$r$+$($9(B

   int vfd

   return
   -1 : $B$=$s$J(Bseq$B$O$J$$(B($BFI$_$3$_IT2D(B)
   >=0 seq
   
 */
static int getleastrbseq( int vfd )
{
	int j;

	int least_seq = 0x7fffffff;
	
	VFDCHECK( vfd );


	for(j=0;j<BUFNUM;j++){
		if( sock[vfd].rblen[j] > 0 &&
		    sock[vfd].rbstate[j] == RB_RECVOK ){
			if( sock[vfd].rbseq[j] < least_seq ){
			    least_seq = sock[vfd].rbseq[j];
			}
	   }
	}
	if( least_seq != 0x7fffffff ){
		return least_seq;
	} else {
		return -1;
	}
}

/*
   $B=q$-$3$_(B

   int vfd : vfd
   
   char *buf : $BAw?.$9$k%P%C%U%!(B
   int len: $BAw?.$9$k:GBgD9$5(B

   $B<B:]$K=q$-$3$s$@D9$5$rJV$9!#Ii$O%(%i!<(B
	0$B$b$"$j$&$k$>(B
   
*/
int UDPCWrite( int vfd , char *buf , int len )
{

	int totallen = 0;
	int i;

	VFDCHECK( vfd );
	
	for(i=0;;i++){
		int sbind;
		int copylen;
		
		sbind = getblanksbindex(vfd );
		if( sbind < 0 ){
			break;
		}
		if( len > totallen ){
			if( ( len - totallen ) >BUFLEN ){
				copylen = BUFLEN;
			} else {
				copylen =  len - totallen;
			}
		} else {
			break;
		}
		/* totallen $B$+$i$O$8$^$k(B copylen$B$r%3%T!<$9$k(B */
		bcopy( buf +totallen ,sock[vfd].sb[sbind] , copylen );
		sock[vfd].sblen[sbind] = copylen;
		sock[vfd].sbtime[sbind] = 0;
		sock[vfd].sbstate[sbind] = SB_SENDWAIT;
		sock[vfd].sbseq[sbind] = sock[vfd].write_addr;
		totallen += copylen;
		sock[vfd].write_addr += copylen;

		fprintf( stderr , "sock[%d].write_addr:%d\n",vfd,sock[vfd].write_addr );
						
	}
	
	fprintf( stderr , "[Len Seq State Time]\n" );
	for(i=0;i<BUFNUM;i++){
		fprintf( stderr , "[%d %d %d %f ]\n",
				sock[vfd].sblen[i], sock[vfd].sbseq[i], sock[vfd].sbstate[i], sock[vfd].sbtime[i]);
	}
	

	return totallen;
	
	
}
/*
   sb$B$N6u$-$r$5$,$9(B

   -1 : no blank sb
   >=0 : index of a blank sb
   
 */
static int getblanksbindex( int vfd )
{
	int i;

	VFDCHECK( vfd );
	for(i=0;i<BUFNUM;i++){
		if( sock[vfd].sblen[i] == 0 ){
			return i;
		}
	}
	return -1;
}


/*
   UDPC$B$N%M%C%H%o!<%/=hM}(B
   
 */
void UDPC(void)
{
	int read_ok = 0;
	int write_ok =0;
	int send_more = 0;
	int sent_one = 0;		/* 1$B8D$G$b%Q%1%C%H$rAw?.$9$k$H(B1 */
	
	while(1){
		int a;
		int i;
		fd_set rfd , wfd;
		struct timeval tm;
		
		tm.tv_sec = 0;
		tm.tv_usec = 0;
		
		FD_ZERO( &rfd );
		FD_ZERO( &wfd );

		FD_SET(  udpc.mainsockfd , &rfd);
		FD_SET(  udpc.mainsockfd , &wfd );

		a = select( 100 , &rfd , &wfd , (fd_set*)0 , &tm );

		read_ok = 0;
		write_ok = 0;
		send_more = 0;
		sent_one = 0;
		
		/* $B$+$-$3$_2DG=(B */
		if( FD_ISSET( udpc.mainsockfd , &wfd )){
			static int send_first = 0;

			
			write_ok = 1;
			
			/* SENDWAIT$B$rAw?.$7$F(BACKWAIT$B$N>uBV$K$9$k(B */
			/* $B%Q%1%C%H$R$H$DAw?.(B */
			

			for(i=0;i<MAXCON;i++){
				int j;
				char buf[BUFLEN + HEADERSIZ];
				
				send_first++;
				if( send_first == MAXCON )send_first =0;


				/* FIN$BAw?.!#:FAw$O$^$@$J$7(B */
				if( sock[send_first].state == VS_FINWAIT ){
					makeheader( buf , 0,-1,  0,0,1,0, 0 , send_first , sock[send_first].vfd_d );
					if(sender( buf,HEADERSIZ , sock[send_first].ipaddr,sock[send_first].dest_port )
					   == HEADERSIZ ){
						sock[send_first].state = VS_FINACKWAIT;
						sock[send_first].fin_time = getUTimeDouble();
						break;
					}
				}
				
				/* $B%G!<%?Aw?.(B */
				for(j=0;j<BUFNUM;j++){
					if( sock[send_first].sbstate[j] == SB_SENDWAIT &&
					   sock[send_first].sblen[j] > 0){
						if( sent_one == 0 ){
							makeheader( buf , sock[send_first].sbseq[j] , -1,0,0,0,0,
								   sock[send_first].sblen[j],send_first,sock[send_first].vfd_d );
							bcopy( sock[send_first].sb[j] , buf +
								  HEADERSIZ , sock[send_first].sblen[j]);
							if( sender( buf , HEADERSIZ + sock[send_first].sblen[j] ,
							   sock[send_first].ipaddr, sock[send_first].dest_port ) ==
						    ( HEADERSIZ + sock[send_first].sblen[j] ) ){
								sock[send_first].sbstate[j] = SB_ACKWAIT;
								sock[send_first].sbtime[j] = getUTimeDouble();
								sent_one = 1;
							}
						} else {
							
							/* $B$^$@=q$-$3$`$b$N$,$"$k$>!<(B */
							send_more = 1;
							break;
						}
					}
				}
				if( send_more == 1 )break;
			}
				
		}

		/* $BFI$_$3$_2DG=(B */
		if( FD_ISSET( udpc.mainsockfd , &rfd )){
			struct sockaddr_in sin;
			int clilen = sizeof( sin );
			int n;
			int svfd , rvfd, seq , ack,rst,syn,fin,ref,len;
			char buf[BUFLEN+HEADERSIZ];
			

			read_ok = 1;
			
			n = recvfrom( udpc.mainsockfd, buf ,sizeof(buf),0,(struct sockaddr*)&sin,&clilen);
			if(n<0)continue;
			getheader( buf , &seq , &ack,&rst,&syn,&fin,&ref,&len,&svfd,&rvfd);
			fprintf( stderr ,
					"recvfrom addr:%x len:%d seq:%d ack:%d RSFR:%d %d %d %d l:%d svfd:%d rvfd:%d\n",
					sin.sin_addr.s_addr ,n, seq,ack,rst,syn,fin,ref,len,svfd,rvfd);
			
			if( n > 0 ){
				
				

				/* ACK$B$+$=$&$G$J$$$+$GBg$-$/$o$+$l$k(B */
				
				if( ack != -1 ){		/* ACK$B$G$"$k(B */
					if( syn ){
						/* Connect$B$N(Back$B$@(B */
						if( svfd >=0 && svfd < MAXCON  && sock[svfd].use &&
						   sock[svfd].state == VS_CONNECTING ){
							sock[svfd].recv_syn_ack = 1;
						}
					}
					if( syn==0 && rst==0 && fin==0 && ref==0 ){
						/* $B%G!<%?%Q%1%C%H$N(BACK$B$G$"$k(B */
						receiveack( svfd,ack );
					}
				} else {			/* ACK$B$G$J$$(B */
					if( syn ){			
						/* Connect$B$7$F$-$?!#(BACK$B$+$($9(B */
						int r;
						r = testnewsock( sin.sin_addr.s_addr , sin.sin_port,svfd );
						if( r >= 0 ){
							if( write_ok == 1 ){
								makeheader( buf , seq , 0 , 0 , 1 , 0 , 0 , 0, svfd,r );
								sender( buf , HEADERSIZ ,  sin.sin_addr.s_addr ,sin.sin_port );
								fprintf( stderr , "send syn ack\n");
							}
						}
					}
					if( syn==0 && fin==0 && rst==0 && ref==0 ){
						/* $B%G!<%?%Q%1%C%H$@(B */
						int r;
						r = testnewsock( sin.sin_addr.s_addr, sin.sin_port ,svfd );
						if( r >= 0 ){
							/* $B$=$s$J%Q%1%C%H$O$J$$!*(B */
						} else {
							int r;
							r = receivepacket( sin.sin_addr.s_addr , rvfd,seq,buf+HEADERSIZ,len );
							if( r == 0 ){
								/* $B$-$A$s$H%Q%1%C%H<u$1$H$j=hM}$,$G$-$?$N$G!"(BACK$B$r$+$($9(B */
								if( write_ok == 1 ){
									makeheader( buf , -1 , seq, 0 ,0,0,0,len,svfd,rvfd );
									sender( buf , HEADERSIZ , sin.sin_addr.s_addr,sin.sin_port );
								}
							}
						}
					}
					

				}

			}
		}

		if( read_ok == 0 && send_more == 0 ){
			break;
		}
	}
}
/*
   UDPC$B$r=i4|2=$9$k(B

   return values:
   0 OK
   <0 Error

   
*/
int UDPCInit(unsigned short port)
{
	struct sockaddr_in sin;
	int i;
	
	udpc.mainsockfd = socket( AF_INET, SOCK_DGRAM , 0 );
	if( udpc.mainsockfd < 0 ){
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons( port );
	sin.sin_addr.s_addr = INADDR_ANY;
	i= bind( udpc.mainsockfd ,(struct sockaddr*)&sin , sizeof(sin));
	if( i < 0 ){
		udpc.errno = ESOCKETERR;
		return i;
	}

	bzero( sock , sizeof( sock ) );

	udpc.maxbacklog = 5;
	udpc.port = port;
	return 0;
}


/*
   ACK$B$r<u?.$7$?$H$-$N=hM}(B

   int svfd : $BAw?.$7$?(Bfd
   int ack : ack ( $BAw?.$9$k$H$-$N(Bseq)

   return 0 : ok
   <0 : Error
   
*/   
static int receiveack( int svfd , int ack )
{
	int i;
	
	VFDCHECK( svfd );

	for(i=0;i<BUFNUM;i++){
		if( sock[svfd].sbstate[i] == SB_ACKWAIT &&
		   sock[svfd].sblen[i] > 0 &&
		   sock[svfd].sbseq[i] == ack ){
			/* ACK$B$,$+$($C$F$-$?$+$i$1$7$F$7$^$&(B */
			sock[svfd].sbstate[i] = SB_NODATA;
			sock[svfd].sblen[i]=0;
			fprintf( stderr , "Ack ok! [%d] %d\n", i , ack );
			return 0;
		}
	}
	return -1;
}


/*
   Connect$B$9$k!#(B

   Connect$B$G$-$?$i(Bvfd$B$r$+$($9!#(B

   ipaddr : htonl$B8e$N%G!<%?(B
   port : htons$B8e$N%G!<%?(B
   
   -1 : $BI=$K$"$-$,$J$$!#(B
   -3 : $B%5!<%P!<$,JV;v$7$F$J$$(B
   
*/
int UDPCConnect( unsigned long ipaddr , unsigned short port )
{
	int i;
	int vfd;
	char buffer[BUFLEN + HEADERSIZ];

	/* $B:G=i$K!"6u$-$r$5$,$9(B */
	for(i=3;i<MAXCON;i++){
		if( sock[i].use == 0 ){
			vfd = i;
			break;
		}
	}
	if( i == MAXCON ){
		return -1;
	}

	/* $B%3%M%/%HCf$G$"$k$H$$$&5o$_(B */
	sock[vfd].state = VS_CONNECTING;
	sock[vfd].use = 1;
	sock[vfd].write_addr = 0;
	sock[vfd].read_addr = 0;
	sock[vfd].ipaddr = ipaddr;
	sock[vfd].dest_port = port;
	
	/* SYN$B$N(BACK$B$,$+$($C$F$/$k$N$r4|BT$7$F(BSYN$B$rAw?.(B */
	makeheader( buffer , -1 , -1 , 0 , 1 , 0 , 0 , 0 , vfd , 0 );
	sender( buffer , HEADERSIZ ,ipaddr ,port );

	fprintf( stderr ,"send syn\n");
	while(1){
		unsigned long nowsec=0,nowsecstore;
		nowsecstore = nowsec;
		nowsec = getUTimeDouble() / 1000/1000;

		if( nowsecstore != nowsec ){
			for(i=0;i<sizeof( connectretry )/sizeof( connectretry[0]);i++){
				if( connectretry[i] == nowsec ){
					if( i == (sizeof( connectretry )/sizeof( connectretry[0])-1)){
						return -3;/* $B%j%H%i%$$7$D$/$7$?(B */
					}
					sender( buffer , HEADERSIZ , ipaddr , port );
					break;
				}
			}
		}

		UDPC();

		/* $B$3$3$G(Brecv_syn_ack$B$,$?$C$F$?$iJV;v$r$&$1$H$C$?$H$$$&$3$H$K(B */
		/* $B$J$k(B */
		if( sock[vfd].recv_syn_ack ){
			sock[vfd].state = VS_ALIVE;
			fprintf( stderr , "return vfd\n");
			return vfd;
		}
	}	
	return -3;
}

/*
   Accept$B$9$k!#(B

   return values:
   -1 accept$B$7$s$+$C$?!#(B
   >=0 accept$B$7$?!#(Bvfd
   
*/
int UDPCAccept(void)
{
	int i;

	for(i=0;i<MAXCON;i++){
		if( sock[i].use == 1 && sock[i].state == VS_ACCEPTWAIT ){
			sock[i].state = VS_ALIVE;
			return i;
		}
	}
	return -1;
}

/*
   IP , sender_vfd$B$NAH$,?75,$+$I$&$+$7$i$Y$k(B
   return values:
   >=0 $B?75,$G$"$k$N$G!"(Baccept$BBT$A$r$H$&$m$/$7!"$H$&$m$/$7$?(Bvfd$B$r$+$($9!#(B
   -1 $B?75,$@$,%(%i!<$N$?$a?75,EPO?$G$-$J$+$C$?!#(BREF$B$r$+$($;!#(B
   -2 $B?75,$@$,(Bbacklog$B$N:GBg?t$r1[$($F$k$N$G(Baccept$B$KEPO?$G$-$J$+$C$?!#(B
   REF$B$r$+$($;!#(B
   -8 $B?75,$G$J$$$N$G2?$b$7$J$+$C$?!#(B
   
   $B?75,$@$C$?$i(Baccept$BBT$A$H$7$FEPO?$9$k!#(B
   
 */
static int testnewsock( unsigned long ip , unsigned short port , int sender_vfd )
{
	int i,c;
	for(i=0;i<MAXCON;i++){
		if( sock[i].use && sock[i].ipaddr == ip &&
		    sock[i].dest_port == port &&
	 	    sock[i].vfd_d == sender_vfd ){
			return -8;
		}
	}

	
	/* $B?75,$@$C$?(B */
	c=0;
	for(i=0;i<MAXCON;i++){
		if( sock[i].use == 1 && sock[i].state == VS_ACCEPTWAIT ) c++;
	}
	if( c > udpc.maxbacklog ){
		return -2;
	}
	
	for(i=0;i<MAXCON;i++){
		if( sock[i].use == 0 ){
			bzero( &sock[i] ,sizeof(sock[0]));
			sock[i].use = 1;
			sock[i].ipaddr =ip;
			sock[i].vfd_d = sender_vfd;
			sock[i].dest_port = port;
			sock[i].state = VS_ACCEPTWAIT;
			sock[i].read_addr = 0;
			sock[i].write_addr = 0;
			return i;
		}
	}
	
	return -1;
}


/*
   $B$R$H$D$N(Bvfd$B$K%Q%1%C%H$r<u$1$H$C$?!#(B

   return values:
   0 OK ( ACK$B$rJV$7$F$h$$(B )
   <0 Error ( $B%(%i!<$J$N$G(BACK$B$OJV$7$F$O$J$i$J$$(B)

   buf , len$B$K$+$s$7$F$O?.MQ$9$k!#(B
   
 */
static int receivepacket( unsigned long ip , int vfd , int seq , char *buf , int len )
{
	int i;
	
	VFDCHECK( vfd );

	/* $BF1$8(Bvfd$B$K0c$&%"%I%l%9$+$i$d$C$F$-$?!#$3$l$O%V%V!<(B */
	if( sock[vfd].ipaddr != ip ){
		return -1;
	}
	
	for(i=0;i<BUFNUM;i++){
		if( sock[vfd].rblen[i] == 0 ){
			/* $B$"$$$F$k(Brb$B$r$_$D$1$?(B */

			
			bcopy( buf , sock[vfd].rb[i] , len );
			sock[vfd].rbstate[i] = RB_RECVOK;
			sock[vfd].rblen[i] = len;
			sock[vfd].rbseq[i] = seq;

#if 0
			{
				char buffer[10000];
				bcopy(buf , buffer ,len);
				buffer[len]=0;
				fprintf( stderr , "receivepacket: %dbuf [%s] len:%d\n" , i , buffer ,len );
			}
#endif
			
			
			return 0;
		}
	}
	
	/* $BFI$_$3$_%P%C%U%!$,$$$C$Q$$$G$"$k(B */
	return -1;
	
}


/*
   $B%X%C%@$r$D$/$k(B
   
 */
static void makeheader( char *data , int seq , int ack , int rst , int syn , int fin 
					   , int ref, int length , int svfd, int rvfd )
{
	char code = 0;
	seq = htonl( seq );
	ack = htonl( ack );
	length = htonl( length );
	svfd = htonl( svfd );
	rvfd = htonl( rvfd );
	
	if( rst ) code |= RST_BIT;
	if( syn ) code |= SYN_BIT;
	if( fin ) code |= FIN_BIT;
	if( ref ) code |= REF_BIT;

	data[CODE_OFS] = code;
	bcopy((char*)&seq , (char*)(data + SEQ_OFS) , 4 );
	bcopy((char*)&ack , (char*)(data + ACK_OFS) , 4 );
	bcopy((char*)&length ,(char*)(data + LENGTH_OFS) , 4 );		
	bcopy((char*)&svfd ,(char*)(data + SVFD_OFS) , 4 );
	bcopy((char*)&rvfd ,(char*)(data + RVFD_OFS) , 4 );			
}
static void getheader( char *data , int *seq , int *ack , int *rst ,
					  int *syn , int*fin , int *ref , int *length ,int *svfd , int *rvfd)
{
	char code;
	
	*seq = ntohl( *(long*)(data + SEQ_OFS) );
	*ack = ntohl( *(long*)(data + ACK_OFS) );
	*length = ntohl( *(long*)(data + LENGTH_OFS) );
	*svfd = ntohl( *(long*)(data + SVFD_OFS) );
	*rvfd = ntohl( *(long*)(data + RVFD_OFS) );

	code = (char)data[CODE_OFS];
	if( code & RST_BIT ) *rst = 1; else *rst=0;
	if( code & SYN_BIT ) *syn = 1; else *syn=0;
	if( code & FIN_BIT ) *fin = 1; else *fin=0;
	if( code & REF_BIT ) *ref = 1; else *ref=0;
}


/*
   ipaddr: htonl$B8e$N%G!<%?(B
   port : htons$B8e$N%G!<%?(B
   
*/
static int sender( char *data , int len , unsigned long ipaddr , unsigned short port )
{

	struct sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port =  port ;
	to.sin_addr.s_addr = ipaddr;

	fprintf( stderr, "Sender: addr:%x len:%d\n" , ipaddr , len );
	return sendto( udpc.mainsockfd , data, len , 0,(struct sockaddr*)&to, sizeof( struct sockaddr ) );


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
