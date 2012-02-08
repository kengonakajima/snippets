#define _RP_C_

#include "rp.h"

#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>

#include <malloc.h>
#include <strings.h>

struct rp
{

	int maxslot;		/* $B:GBg$N%9%m%C%H?t(B */
	int pbsiz;
	unsigned short port;	/* $B<+J,$N%7%9%F%`$N%]!<%H(B */

	int sockfd;
	
} rp;

struct slot
{
	int use;					/* $B;HMQCf$+$I$&$+(B */
	
	unsigned long di_net;		/* $BAj<j$N(BIP$B%"%I%l%9(B( net byte order ) */
	unsigned short dp_net;		/* $BAj<j$N%]!<%H(B( net byte order ) */
	int dslot;					/* $BAj<j$N%9%m%C%H(B */
	
	unsigned int send_i;		/* $B=q$-$3$_HV9f(B $B:G8e$K(Breg$B$7$?%Q%1%C%H$N(B*/
	unsigned int recv_i;		/* $BFI$_$3$_HV9f(B $B<!$KFI$_$3$`%Q%1%C%H$N(B */

	int open_ack;				/* open$B$N(Back$B$,$-$?$i(B1 */
	int accept_wait;			/* accept$B$5$l$k$N$r$^$C$F$$$k(B */

	int close_wait;				/* close$B$5$l$k$N$r$^$C$F$$$k>uBV$J$i(B1$B!#(B
								   CLOSE$B%3%^%s%I$,$*$/$i$l$F$-$?$i(B1$B$K$7!"(B
								   $B$<$s$V$N%G!<%?$r$h$_$3$`$^$G$O<B:]$K$O(B
								   $B%/%m!<%:$7$J$$!#$3$N%U%i%0$,(B1$B$N>uBV$G(B
								   $BFI$_$3$a$k%G!<%?$b=q$-$3$a$k%G!<%?$b(B
								   $B$J$/$J$C$?$i!"(Buse$B$r(B0$B$K$9$k!#(B
								   close$B$G$-$k$+$b$7$l$J$$%?%$%_%s%0$O!"(B
								   CLOSE$B%3%^%s%I$,$-$?$H$-!"(B
								   CLOSEACK$B%3%^%s%I$,$-$?$H$-!"(B
								   rp_recv$B$7$?$H$-(B(recvq$B$r>C2=(B)$B!"(B
								   DATAACK$B$r$&$1$H$C$F(Bsendq$B$r>C2=$7$?(B
								   $B$H$-$G$"$k!#(B
								   $B$=$l$i$N$H$-$K%A%'%C%/$7$F(Bclose$B2DG=(B
								   $B$G$"$l$P!"(Bclose$B$9$k!#(B

								   */
/*	unsigned int close_i;			*/
								   
	
	double last_in;				/* $B:G8e$K%Q%1%C%H$,$-$?$N$O$$$D$+(B */


#define QSIZE	8
	int sendq[QSIZE];			/* $B=q$-$3$_(Bbuf$B$X$N(Bindex$B$NI=(B -1$B$OIT;HMQ(B */
	int recvq[QSIZE];			/* $BFI$_$3$_(Bbuf$B$X$N(Bindex$B$NI=(B -1$B$OIT;HMQ(B */

	int open_pbi , close_pbi;	/* open , close$B$N%Q%1%C%H$rAw?.$7$?$i(B
								   $B$I$N%Q%1%C%H%P%C%U%!$r$D$+$C$?$+$3(B
								   $B$3$K5-21(B */
	
};
struct slot *slot;


#define SEQ_OFS 0				
#define CMD_OFS ( SEQ_OFS + 4 )
#define LEN_OFS ( CMD_OFS + 4 )		/* $B$3$ND9$5$K$O%G!<%?ItJ,$ND9$5$r(B */
									/* $B$$$l$k(B */
#define SFD_OFS ( LEN_OFS + 4 )
#define RFD_OFS	( SFD_OFS + 4 )
#define HEADERSIZE ( RFD_OFS + 4 )	/* $B$3$N%*%U%;%C%H$+$i%G!<%?$,$O$8$^$k(B */

#define COMMAND_DATA		0x00000001		/* $B%G!<%?MQ%Q%1%C%H(B */
#define COMMAND_DATAACK		0x00000002		/* ACK$BMQ(B */
#define COMMAND_OPEN		0x00000008		/* $B$R$i$/;~$K$D$+$&(B */
#define COMMAND_CLOSE		0x0000000a		/* $B$H$8$k$H$-$K$D$+$&(B */
#define COMMAND_OPENACK		0x00000080		/* open$B$N(BACK */
#define COMMAND_CLOSEACK	0x000000a0		/* close$B$N(BACK */


/* $B:FAw$N%?%$%_%s%0$NI=!#(B $B$3$NI=$OBg$-$5$rE,Ev$KJQ$($?$j$7$F$h$$!#(B
   $B$3$NI=$r$d$j$D$/$9$^$G%j%H%i%$$9$k!#(B
 */
#define RETRY_BASE (4.0*1000*1000)
double timetable[] = {
	RETRY_BASE * 1 ,
	RETRY_BASE * 2 ,
	RETRY_BASE * 3 ,
	RETRY_BASE * 4 ,
	RETRY_BASE * 6 ,
	RETRY_BASE * 8 ,
	RETRY_BASE * 12 ,	
	RETRY_BASE * 20 };		/* $B$3$N;~4V$r$9$.$k$H@\B3$r@Z$k!#(B */

/* $B%a%b%j$N$`$@;H$$$r$5$1$k$?$a$N$h$/$"$k$O$$$l$D(B */
struct pb
{
	int use;

	int state;
#define PBS_SENDWAIT	1			/* $BAw?.$r$^$C$F$$$k(B $B=q$-$3$_MQ(B*/
#define PBS_ACKWAIT		2			/* ACK$B$r$^$C$F$$$k(B  $B=q$-$3$_MQ(B*/
#define PBS_SENDACK		8			/* ACK$B$rAw?.$7$?!#(Brecv$B$r$^$C$F$$$k(B $BFI$_$3$_MQ(B */

	double sendtime;		/* $B$$$DAw?.$7$?$+!#(B0$B$G$J$$4V$8$e$&!"(B
							   $B:FAw$7$D$E$1$k!#(BACK$B$,$-$?$i<+F0E*$KIT;HMQ$K$J$k!#(B*/
	double next_time;		/* $B<!$KAw?.$9$k;~4V!#(B */
	int retry_times;		/* $B:FAw$r$7$?2s?t!#(B $BEvA3:G=i$O(B0$B$+$i$O$8$^(B */
							/* $B$k!#:FAw$r$9$k$?$S$K(Bincrement*/
	
	unsigned long ip_net;
	unsigned short port_net;
	
	int totallen;

	unsigned long seq;			/* $B%Q%1%C%H$N%X%C%@$N%3%T!<(B */
	int cmd , len , sfd , rfd;
	
#define MTU		512
	char buf[MTU+HEADERSIZE];
};
struct pb *pb;

#define PBSIZ(slotnum)  ( (slotnum)*8 )		/* PB$B$N8D?t$r$-$a$k%^%/%m(B */


static int findpb( void );
static void makeheader( char *data , unsigned long seq ,
					   int cmd , int len , int sfd , int rfd );
static void getheader( char*data , unsigned long *seq ,
					  int *cmd , int *len , int *sfd , int *rfd );
static void modifyheader_len( char *data , int len );
static int regpacket_send( unsigned long ip_net , unsigned short port_net ,
					 char *data , int len );
static int sender( char *data , int len , unsigned long ip_net , unsigned short port_net);
static int regpacket_recv( unsigned long ip_net , unsigned short port_net,
						  char *data , int len );
static int findslot(void);
static void removepb( int ind );


static int findblanksendq( int fd );
static int findblankrecvq( int fd );
static void closeslot( int fd );
static int checkclosable( int fd);
static void printheader( char*s ,char*d);
static int calcpbuse(void);
static int calcrecvquse(int fd );
static int calcsendquse( int fd );

/*
   $B=i4|2=$9$k(B 
*/

int rp_init( unsigned short port , int maxslot )
{
	struct sockaddr_in sin;
	int i;

	rp.sockfd = socket( AF_INET , SOCK_DGRAM , 0 );
	if( rp.sockfd < 0 ){
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons( port );
	sin.sin_addr.s_addr = INADDR_ANY;
	i = bind( rp.sockfd ,(struct sockaddr*)&sin,sizeof(sin));
	if( i < 0 ){
		return i;
	}

	rp.port = port;
	rp.maxslot = maxslot;
	
	/* $B%a%b%j$r3NJ]$7$F=i4|2=(B */
	slot = (struct slot*)malloc( sizeof( struct slot ) * maxslot );
	if( slot == NULL ){
		return -2;
	}

	rp.pbsiz = PBSIZ( maxslot );
	pb = (struct pb*) malloc( sizeof( struct pb ) * rp.pbsiz );
	if( pb == NULL ){
		free( slot );
		return -3;
	}

	return 0;
}


/*

   $BAj<j%7%9%F%`$K@\B3$9$k(B($B%V%m%C%/$9$k(B)
*/
int rp_open( unsigned long ip_net ,unsigned short port_net )
{
	int si;
	int pbi;
	
	char data[HEADERSIZE];

	si = findslot();
	if( si <0 ) return -2;

	slot[si].di_net = ip_net;
	slot[si].dp_net = port_net;
	
	makeheader( data , 0 , COMMAND_OPEN , 0, si ,0 );
	if( ( pbi = regpacket_send( ip_net , port_net , data , HEADERSIZE ))<0){
		return -34;
	}

	slot[si].open_pbi = pbi;
	
	while(1){
		rp_proc();

		if( slot[si].open_ack ){
			break;
		}
	}

	return si;
}


/*
   $B?7$7$$Aj<j$rG'<1$9$k(B
*/

int rp_accept( unsigned long *ip_net , unsigned short *port_net )
{
	int i;

	for(i=0;i<rp.maxslot;i++){
		if( slot[i].use && slot[i].accept_wait){
			return i;
		}
	}

	return -1;
}
/*
   accept$B$G$-$k@\B3$,$"$k$+$I$&$+$7$i$Y$k!#(B
*/   
int rp_acceptable(void )
{
	int i;
	for(i=0;i<rp.maxslot;i++){
		if( slot[i].use && slot[i].accept_wait){
			return 1;
		}
	}
	return 0;
}

/*
   $B@\B3$,JD$8$i$l$F$7$^$C$F$$$k$+$I$&$+$r$7$i$Y$k!#(B

*/
int rp_isclosed( int fd )
{
	if( fd < 0 || fd >= rp.maxslot ) return 1;

	if( slot[fd].use == 0) return 1;

	return 0;
}


/*
   $B=q$-$3$`(B
*/
int current_send_i;/*for debug */
int rp_send( int fd, char *buf , int len )
{
	char data[MTU+HEADERSIZE];
	int ind;
	int pbind;


	if( fd < 0 || fd >= rp.maxslot || slot[fd].use == 0) return -1;



	/* $B$3$N@)8B$b$H$C$Q$i$&$3$H$,$G$-$k!#(B PENDING */
	if( len > MTU )return -3;


	/* $B%9%m%C%H$NCf$r8!:w$7$F!"(BSENDWAIT$B$K$J$C$F$$$k$b$N$N$&$A:G8e$N$b$N(B
	   $B$,(BMTU$B$KC#$C$7$F$J$$>l9g$O!"$=$3$KB-$7$3$_!"(B
	   $BM>$C$?J,$rIaDL$K=hM}$9$k!#$3$3$NItJ,$rF/$+$J$/$9$k$@$1$G!"(B
	   $B05=L$J$7E>Aw%b!<%I$K$J$k!#(B*/
#if 0

	for(i=0;i<QSIZE;i++){
		int sq = slot[fd].sendq[i];
		if( sq != -1 && pb[sq].use &&
		    pb[sq].state == PBS_SENDWAIT &&
		    pb[sq].len < MTU ){
			/* $B$3$3$G(Bpb$B$NFbMF$r$$$8$k$>(B */
			int left = MTU - pb[sq].len;
			int copylen;
			if( len <= left ){
				copylen = len;
			} else {
				copylen = left;
			}
			bcopy( buf , pb[sq].buf + HEADERSIZE + pb[sq].len , copylen );

			
			/* $B%P%C%U%!$r$:$i$9(B */
			buf += copylen;
			len -= copylen;

			/* pb$B$r$$$8$k(B */
			pb[sq].len += copylen;
			pb[sq].totallen += copylen;
			modifyheader_len( pb[sq].buf  , pb[sq].len );



			
			/* len$B$rA4It$3$T!<$G$-$F$7$^$C$?$i!"$3$N8e$O$$$i$s!#(B */
			if( len == 0 ){
/*				fprintf( stderr , "appended %d bytes to pb %d( That's all! ) extended:%d\n" ,
						copylen	 , sq, pb[sq].len );*/
				return copylen;
			} else {
				fprintf( stderr , "appended %d bytes extended:%d\n" ,
						copylen , pb[sq].len );
				break;
			}
		}
	}
	
	
#endif
	
	
	/* $B$^$:!"%9%m%C%H$N=q$-$3$_%P%C%U%!I=$N$"$-$r$5$,$9!#(B */
	ind = findblanksendq( fd );
	if( ind < 0 ) return 0;

	makeheader( data , slot[fd].send_i , COMMAND_DATA,  len , fd,
			   slot[fd].dslot );
	bcopy( buf ,data + HEADERSIZE ,len );

	current_send_i = slot[fd].send_i;
	
	if(( pbind =
	regpacket_send( slot[fd].di_net , slot[fd].dp_net, data,HEADERSIZE+len))<0){
		return 0;
	}


	
	slot[fd].sendq[ind] = pbind;
	
	/* $B%+%&%s%?!<$r$R$H$D$9$9$a$k(B */
	slot[fd].send_i++;

	fprintf( stderr, "SUCCESSFUL_RP_SEND! now send_i : %d\n" ,
			slot[fd].send_i );
	return len;
}

/*
   $BFI$_$3$`(B

   $BFI$_$3$s$@D9$5$r$+$($9!#(Blen$B$G;XDj$7$?CM$,%a%C%;!<%8$ND9$5$h$j(B
   $BC;$$>l9g$O!"$"$^$C$?%G!<%?$O<N$F$i$l$k!#(B
   
*/
int current_seq;/* for debug */
int rp_recv( int fd , char *buf , int len )
{
	int i;
	int ret;
	
	if( fd < 0 || fd >= rp.maxslot || slot[fd].use == 0) return -1;

	
	
	for(i=0;i<QSIZE;i++){
		int rq = slot[fd].recvq[i];

/*		fprintf(stderr , "AHOAHO pb[%d] : use %d state %d seq %d\n",
		rq , pb[rq].use , pb[rq].state , pb[rq].seq );*/

		
		if( rq != -1 &&
		   pb[rq].use &&
		   pb[rq].state == PBS_SENDACK ){
			
		    if( pb[rq].seq == slot[fd].recv_i ){

				if(  len < pb[rq].len ){
					bcopy( pb[rq].buf+HEADERSIZE , buf , ret=len );
				} else {
					bcopy( pb[rq].buf+HEADERSIZE , buf , ret=pb[rq].len );
				}

				current_seq = pb[rq].seq;

				/* $BFI$_$3$_=*$o$C$?$i$=$N(Bpb$B$O2rJ|$9$k$H$H$b$K(B-1$B$r$$$l$k(B */
				removepb( rq );
				slot[fd].recvq[i] = -1;
				{
					FILE *fp;
					fp = fopen( "setsendack.txt" , "a+");
					fprintf( fp , "UNSET %d RQi:%d\n" , rq ,i);
					fprintf( stderr , "UNSET %d RQi:%d\n" , rq ,i);
					fclose(fp);
				}
			
				/* $BFI$_$3$_%]%$%s%?$r(B1$B$9$9$a$k(B */
				slot[fd].recv_i++;

				fprintf( stderr, "RR CORRECT RECVI slot[%d].recv_i is now %d.\n",
						fd,slot[fd].recv_i);

				if( checkclosable(fd))closeslot(fd);
			
				return ret;
			} else {
				fprintf( stderr, "RR NOTCORRECT RECVI: slot:%d pb.seq:%d\n",
						slot[fd].recv_i , (int)pb[rq].seq );
			}
		}
	}

	return 0;
}


/*
   $B$=$N(Bslot$B$+$iFI$_$3$`$3$H$,$G$-$k$+$I$&$+$r$7$i$Y$k!#(B
   1$B$J$i$h$_$3$_2DG=!#(B0$B$J$iFI$_$3$a$J$$!#(B
   
   
*/
int rp_recvable(int fd )
{
	int i;

/*	fprintf(stderr,"RECVABLE %d %d\n",fd,slot[fd].use);*/
	if( fd < 0 || fd >= rp.maxslot || slot[fd].use == 0) return 0;

	for(i=0;i<QSIZE;i++){
		int rq = slot[fd].recvq[i];
/*		fprintf(stderr , "HOGEHOGE pb[%d] : use %d state %d seq %d\n",
				rq , pb[rq].use , pb[rq].state , pb[rq].seq );*/

		if( rq != -1 &&
		    pb[rq].use &&
		    pb[rq].state == PBS_SENDACK &&
		    pb[rq].seq == slot[fd].recv_i ){
			return 1;
		}
	}
	return 0;
}
/*
   $B$+$-$3$_2DG=$+$I$&$+$r$7$i$Y$k!#(B
   1$B$J$i=q$-$3$a$k!#(B0$B$J$i=q$-$3$a$J$$!#(B

   sendq$B$K6u$-$,$"$k$+$I$&$+$r$7$i$Y$k$@$1$G$h$$!#(Bpb$B$,$$$C$Q$$$K$J$k$+$I$&$+(B
   $B$O9M$($K$$$l$J$$!#$=$l$O(Bsend$B$N<:GT$H$7$F$"$i$o$l$k!#(B
   
   
   
   
*/
int rp_sendable( int fd )
{
	int i;


	if( fd < 0 || fd >= rp.maxslot || slot[fd].use == 0) return 0;

	for(i = 0 ;i < QSIZE ; i++ ){
		if( slot[fd].sendq[i] == -1 ){
			return 1;
		}
	}
	return 0;
}

/*
   $BJD$8$k(B($B%V%m%C%/$9$k(B)
*/
int rp_close( int fd )
{
	char data[HEADERSIZE];
	int pbi;
	
	if( fd < 0 || fd >= rp.maxslot || slot[fd].use == 0) return -1;

	makeheader( data , 0 , COMMAND_CLOSE , 0 , fd , slot[fd].dslot );
	pbi = regpacket_send(
		slot[fd].di_net , slot[fd].dp_net,data,HEADERSIZE );

	slot[fd].close_pbi = pbi;
	
	while(1){
		rp_proc();
		if( slot[fd].use == 0 ){
			break;
		}
	}

	return 0;
}



/*
   $B<B:]$N=hM}$r$5$;$k(B
*/

int send_first = 0;
double last_proc;		/* $BA02s=hM}$7$?;~4V(B */
int proc_count = 0;		/* rp_proc$B$r<B9T$7$?2s?t(B */


#define INTERVAL_MSEC ( 100.0 )

int rp_proc( void)
{
	int i;
	int recvcount = 0 , sendcount=0;


	
	/* $B;~4V@)8B(B */
	if( INTERVAL_MSEC == 0.0 ){
	} else {
		double now_time = getUTimeDouble();
		if( ( now_time - last_proc ) < (INTERVAL_MSEC*1000)) return 0;
		last_proc = now_time;
	}

	proc_count ++;

	fprintf( stderr , "!");
	while(1){
		fd_set rfd , wfd;
		struct timeval tm;
		double now_time = getUTimeDouble();
		int a;
		int sent_one = 0;	/* $B$R$H$D$G$bAw?.$9$k$H(B1 */
		int is_next_send = 0;	/* $B$b$C$HAw?.$7$?$$$J$i(B1 */
		int read_ok = 0;		/* $BFI$_$3$_2DG=$J$i(B1 */
		
		tm.tv_sec = 0;
		tm.tv_usec = 0;


        FD_ZERO( &rfd );
		FD_ZERO( &wfd );

		FD_SET( rp.sockfd , &rfd );
		FD_SET( rp.sockfd , &wfd );

		a = select( 100, &rfd , &wfd , (fd_set*)0,&tm);


		
		if( FD_ISSET( rp.sockfd,&wfd )){
			sendcount++;
			
			for(i=0;i<rp.pbsiz+100 ;i++){
				send_first++;
				if( send_first == rp.pbsiz ) send_first = 0;

				if( pb[send_first].use && pb[send_first].state == PBS_SENDWAIT ){
					/* $BAw?.$r$^$C$F$$$k>uBV$@$C$?(B */
					if( sent_one == 0 ){
						if( sender( pb[send_first].buf ,pb[send_first].totallen,
							   pb[send_first].ip_net , pb[send_first].port_net) ==
						   pb[send_first].totallen ){
							fprintf( stderr, "ANYWAY_SEND\n" );
							pb[send_first].sendtime = now_time;
							/* $B<!$KAw?.$9$k;~4V$r@_Dj(B */
							pb[send_first].next_time =
								pb[send_first].sendtime + timetable[0];
							pb[send_first].retry_times = 0;
							
							/* ACK$B0J30$O!"(BACK$B$r$^$D>uBV$KA+0\(B */
							if( pb[send_first].cmd == COMMAND_DATA ||
							    pb[send_first].cmd == COMMAND_OPEN ||
							    pb[send_first].cmd == COMMAND_CLOSE ){
								pb[send_first].state = PBS_ACKWAIT;
							} else {
								/* ACK$B$N>l9g$OAw?.$7$?$i$1$9(B */
								removepb( send_first );
							}
							sent_one = 1;
							break;
						}
					} else {
						is_next_send = 1;		/* $BAw?.$G$-$k$b$N$,$^$@$"$k!#(B */
					}
				} else if(  pb[send_first].use && pb[send_first].state == PBS_ACKWAIT ){
					/* $B;~4V$r$_$F:FAw(B  */
					/* $B:FAw$O(B COMMAND_DATA , COMMAND_CLOSE , COMMAND_OPEN$B$N$_(B */

					if( pb[send_first].cmd == COMMAND_DATA ||
					    pb[send_first].cmd == COMMAND_CLOSE ||
					    pb[send_first].cmd == COMMAND_OPEN ){
						if( pb[send_first].next_time < now_time ){

							if( sender( pb[send_first].buf , pb[send_first].totallen ,
									   pb[send_first].ip_net , pb[send_first].port_net)==
							   pb[send_first].totallen ){
								sent_one = 1;
							}
							pb[send_first].retry_times++;
							fprintf( stderr , "RETRY! %dtimes\n",
								pb[send_first].retry_times );

							if( pb[send_first].retry_times ==
							   sizeof( timetable) / sizeof( timetable[0])){
								/* $B2s?t$,%*!<%P!<$7$?$+$i!"(B
								   $B@\B3$rJD$8$F$7$^$&!#(B */
								
								closeslot( pb[send_first].sfd );
								removepb( send_first );
								
								fprintf( stderr , "TIMEOUT!\n");
								
							} else {
								pb[send_first].next_time = pb[send_first].sendtime +
								timetable[pb[send_first].retry_times ];
							}
						}
						
						
					}
				}
			}
		}
		if( FD_ISSET( rp.sockfd, &rfd )){
			struct sockaddr_in sin;
			int clilen = sizeof( sin );
			int n;
			int ret;
			char buf[MTU];

			recvcount++;
			read_ok = 1;			

			n = recvfrom( rp.sockfd , buf , sizeof( buf ),0,
						 (struct sockaddr*)&sin, &clilen );
			if( n <= 0 ) continue;

			fprintf( stderr,"recvfrom: addr:%x port:%d len:%d pbuse:%d recvquse:%d sendquse:%d\n",
					(int)sin.sin_addr.s_addr,sin.sin_port,n , calcpbuse() ,
					calcrecvquse( 0) , calcsendquse(0));
			printheader( "HOGE" , buf );
			
			if( (ret=regpacket_recv( sin.sin_addr.s_addr,sin.sin_port, buf , n ))<0){
				fprintf( stderr , "FUCKASS: %d\n" , ret );
				continue;
			}
		}



		if( is_next_send == 0 && read_ok == 0 ){
/*			if( ( sendcount + recvcount) > 1 ){
				fprintf( stderr ,"COUNT>1. Cou:%d recvc:%d sendc:%d\n",
						proc_count , recvcount , sendcount  );
			}
*/			
			break;
		}
	}

	return 0;

}	


static int findblanksendq( int fd )
{
	int i;
	for(i=0;i<QSIZE;i++){
		if( slot[fd].sendq[i] == -1 ){
			return i;
			break;
		}
	}
	return -1;
}
static int findblankrecvq( int fd )
{
	int i;
	
	for(i=0;i<QSIZE;i++){
		if( slot[fd].recvq[i] == -1 ){
			return i;
			break;
		}
	}
	return -1;
}


/********************************************************/
static void makeheader( char *data , unsigned long seq ,
					   int cmd , int len , int sfd , int rfd )
{
	seq = htonl( seq );
	cmd = htonl( cmd );
	len = htonl( len );
	sfd = htonl( sfd );
	rfd = htonl( rfd );

	bcopy((char*)&seq, (char*)(data+SEQ_OFS ),4);
	bcopy((char*)&cmd, (char*)(data+CMD_OFS ),4);
	bcopy((char*)&len, (char*)(data+LEN_OFS ),4);
	bcopy((char*)&sfd, (char*)(data+SFD_OFS ),4);
	bcopy((char*)&rfd, (char*)(data+RFD_OFS ),4);

/*	fprintf( stderr ,"makeheader: %x %x %x %x %x\n",
			seq,cmd,len,sfd,rfd);*/
	
}
static void getheader( char*data , unsigned long *seq ,
					  int *cmd , int *len , int *sfd , int *rfd )
{
	*seq = ntohl(  *(long*)(data +SEQ_OFS ));
	*cmd = ntohl(  *(long*)(data +CMD_OFS ));
	*len = ntohl(  *(long*)(data +LEN_OFS ));
	*sfd = ntohl(  *(long*)(data +SFD_OFS ));
	*rfd = ntohl(  *(long*)(data +RFD_OFS ));


		
}
static void modifyheader_len( char *data , int len )
{
	len = htonl( len );
	bcopy((char*)&len, (char*)(data+LEN_OFS ),4);
}

/**************************************  PB routines ******************/

/*
   $B$9$Y$F$NAw?.%Q%1%C%H$O$3$N4X?t$rDL$k$N$@(B
   int len $B$O%Q%1%C%HA4BN$N%5%$%:$@!#(B

   $B$+$($j$A$O!"F~$l$?(Bsendq$B$X$N%$%s%G%/%9$@!#Ii$O<:GT(B
   
*/
static int regpacket_send( unsigned long ip_net , unsigned short port_net ,
					 char *data , int totallen )
{

	unsigned long seq;
	int cmd , len , sfd , rfd;
	int i;
	
	/* $B:GDc$G$b%X%C%@!<$N%5%$%:$O$"$k$O$:$@!#$^$?Bg$-$9$.$F$b%U%!%C%/$@(B */
	if( totallen < HEADERSIZE ) return -1;
	if( totallen > ( HEADERSIZE + MTU) ) return -4;

	/* $B%3%T!<$r$-$*$/(B */
	getheader( data , &seq, &cmd,&len,&sfd,&rfd );

	
	i = findpb();
	if( i <0 )	return -2;
	pb[i].state = PBS_SENDWAIT;
	pb[i].sendtime = 0;	
	pb[i].totallen = totallen;
	pb[i].ip_net = ip_net;
	pb[i].port_net = port_net;
	pb[i].len = len;
	pb[i].sfd = sfd;
	pb[i].rfd = rfd;
	pb[i].cmd = cmd;
	pb[i].seq = seq;
	
	/* $B%P%C%U%!$r%3%T!<(B */
	bcopy( data , pb[i].buf, totallen );

	return i;

}
/*
   $BFI$_$3$s$@$H$-$N(Bregpacket
   
*/   
static int regpacket_recv( unsigned long ip_net , unsigned short port_net,
						  char *data , int totallen )
{
	unsigned long seq;
	int cmd , len , sfd , rfd;
	char senddata[HEADERSIZE];
	int i;
	
	/* $B:GDc$G$b%X%C%@!<$N%5%$%:$O$"$k$O$:$@!#$^$?Bg$-$9$.$F$b%U%!%C%/$@(B */
	if( totallen < HEADERSIZE ) return -1;
	if( totallen > ( HEADERSIZE + MTU) ) return -4;

	getheader( data , &seq, &cmd,&len,&sfd,&rfd );

#if 0
	if( cmd == COMMAND_DATA) {
			FILE *fp;
			fp = fopen( "recvp.txt" , "a+");
			fprintf( fp , "RECVP Seq:%d len:%d cmd:%d\n" , (int)seq,len,cmd);
			fclose(fp);
	}
#endif
	
/*	fprintf( stderr,"regpacket_recv: sfd:%d cmd:%d rfd:%d len:%d seq:%d\n",
			sfd,cmd,rfd,len,seq);*/
	
	/* $B%9%m%C%H$N>pJs$,$*$+$7$+$C$?$i<u$1$H$i$J$$$>(B */
	if( sfd < 0 || sfd >= rp.maxslot ){
		return -55;
	}

	
	/* OPEN$B$,$-$?$iB(Ez$G!"%Q%1%C%H$K$"$?$i$7$$%9%m%C%H$N(Bindex$B$r$$$l$F$+$($9(B */
	if( cmd == COMMAND_OPEN ){
		int si;
		int ret;
		
		/* $B$$$-$J$j6u$-$rC5$7$F3d$j$"$F$k$N$G$O!"(B2$B=E$K(BOPEN$B$,$-$?$H$-$K(B
		   $B%P%0$k!#$J$N$G!"(BOPEN$B$N$H$-$O$9$G$K(Bopen$B$5$l$F$$$k@\B3$N$9$Y$F(B
		   $B$HHf3S$7$F!"%"%I%l%9!"%]!<%H!"(Bsfd$B$,0lCW$7$F$$$k$b$N$,$"$k$+$I$&$+(B
		   $BC5$7$F!"F1$8$@$C$?$iL5;k$9$kI,MW$,$"$k!#(B */

		for(i=0;i<rp.maxslot;i++){
			if( slot[i].use && slot[i].di_net == ip_net &&
			    slot[i].dp_net == port_net && slot[i].dslot == sfd ){
				/* $B0lCW$7$F$?$N$G!"L5;k$9$k!#(B */
	/*			fprintf( stderr , "Ignored duplicate OPEN command!\n");*/
				return 0;
			}
		}
		
		
		si = findslot();
		if( si<0)return -100;
		
		makeheader( senddata, 0 , COMMAND_OPENACK,0,sfd, si );
		ret = regpacket_send( ip_net , port_net ,senddata , HEADERSIZE );
		if( ret < 0 ){
			/* ACK$B$rAw$l$J$$(B */
			closeslot( si );
			return -1001;
		}
		

		/* $B$"$?$i$7$$%9%m%C%H$r@_Dj$9$k(B */
		slot[si].di_net = ip_net;
		slot[si].dp_net = port_net;
		slot[si].dslot = sfd;
		slot[si].send_i = 0;
		slot[si].recv_i = 0;
		slot[si].open_ack = 0;
		slot[si].accept_wait = 1;
		slot[si].last_in = getUTimeDouble();
		return 0;
	}
	
	/* OPEN$B$NJV;v$,$+$($C$F$-$?$N$@(B */
	if( cmd == COMMAND_OPENACK &&	slot[sfd].use ){
		slot[sfd].open_ack = 1;
		slot[sfd].dslot = rfd;	/* $BAj<j$N%9%m%C%H(Bindex */
		slot[sfd].last_in = getUTimeDouble();
		fprintf( stderr , "regpacket_recv: slot %d received openack!\n", sfd );
		/* $B$b$H$K$J$C$?(Bpb$B$r%j%j!<%9(B */
		removepb( slot[sfd].open_pbi );
		slot[sfd].open_pbi = -1;
		return 0;
	}

	if( cmd == COMMAND_DATA ){
		int pbi;
		int recvqind;
		int ret;
		
		/* $B$^$:!"4{B8$N@\B3$KBP$9$kAw?.$+$I$&$+$r$7$i$Y$k!#(B */
		if( rfd < 0 || rfd >= rp.maxslot || slot[rfd].use == 0) return -4033;

		/* $BAw$i$l$F$-$?$b$N$r$=$N$^$^?.MQ$9$k$H(B2$B=E$KAw?.$5$l$F$-$?$H$-(B */
		/* $B$KLdBj$,@8$8$k$N$G!"$3$l$^$G$K<u$1$H$C$?$b$N$9$Y$F$HHf3S$7(B */
		/* $B$F!"(Bseq$B$,F1$8$b$N$,$"$C$?$iL5;k$9$k(B */
		for(i=0;i<QSIZE;i++){
			if( slot[rfd].recvq[i] != -1 && pb[slot[rfd].recvq[i]].seq == seq ){
				/* seq$B$,F1$8$N$,$"$C$?$+$iL5;k$9$k!#(B */
				fprintf( stderr , "Ignored data pacekt! seq:%d\n",(int)seq);
				return 0;
			}
		}
				
			
		recvqind = findblankrecvq( rfd );
		if( recvqind < 0 ){
			fprintf( stderr, "findblankrecvq fail!\n");
if(0	)			{
				int i;
				for(i=0;i<QSIZE;i++){
					fprintf( stderr, "Slot %d recvq %d : PB%d" , rfd , i ,
							slot[rfd].recvq[i] );
					fprintf( stderr ,"use:%d state:%d seq:%d cmd:%d\n",
							pb[slot[rfd].recvq[i]].use ,
							pb[slot[rfd].recvq[i]].state ,
							(int)pb[slot[rfd].recvq[i]].seq ,
							pb[slot[rfd].recvq[i]].cmd );
				}
				exit(0);	
			}
			return -54;
		}
		
		pbi = findpb();
		if( pbi <  0 ) return -2333;		
		pb[pbi].state = PBS_SENDACK;
		{
			FILE *fp;
			fp = fopen( "setsendack.txt" , "a+" );
			fprintf( fp , "SET_ %d RQi:%d\n" , pbi , recvqind );
			fprintf( stderr , "SET_ %d RQi:%d\n" , pbi , recvqind );
			fclose(fp);
		}
		pb[pbi].sendtime = 0;
		pb[pbi].totallen = totallen;
		pb[pbi].ip_net = ip_net;
		pb[pbi].port_net = port_net;
		pb[pbi].len = len;
		pb[pbi].seq = seq;
		pb[pbi].cmd = cmd;
		pb[pbi].sfd = sfd;
		pb[pbi].rfd = rfd;
	
		bcopy( data , pb[pbi].buf , totallen );

		/* $B$b$A$L$7$NI=$K=q$-$3$`$N$@(B */
		slot[rfd].recvq[recvqind] = pbi;
/*		fprintf( stderr,"Added PBI:%d RECVQIND:%d RFD:%d\n",
				pbi , recvqind , rfd );*/

		/* ACK$B$r$+$($9(B */
		makeheader( senddata, seq, COMMAND_DATAACK , 0 , sfd , rfd );
		ret = regpacket_send( ip_net , port_net ,senddata, HEADERSIZE );
		if( ret < 0 ){
			fprintf( stderr , "regpacket_send fail!\n" );
			removepb( pbi );
			slot[rfd].recvq[recvqind]=-1;
			return -2111;
		}
		return 0;
	}
	
	if( cmd == COMMAND_DATAACK ){
		
		/* $B%G!<%?$KBP$9$k(BACK$B$,$-$?!#(B ACK$B$O2?=E$K$-$F$b(B2$B8DL\0J>e$O(B
		 $BL5;k$9$k(B(use$B$r$_$k$+$i(B)$B$+$iBg>fIW$G$"$k!#(B*/
		if( sfd < 0 || sfd >= rp.maxslot || slot[sfd].use == 0) return -4192;

		/* $B%9%m%C%H$NCf$N(Bsendq$B$r8!:w$7$F!"(Bseq$B$r$5$,$9(B */
		for(i=0;i<QSIZE;i++){
			int s = slot[sfd].sendq[i];

/*			fprintf( stderr , "slot[%d].sendq[%d]=%d\n",sfd,i,
					slot[sfd].sendq[i] );*/
			
			if( s != -1 && pb[s].use && pb[s].seq == seq ){
				/* $B0lCW$7$?$N$G>C$7$F$7$^$&!#(B */
				removepb( s );
				slot[sfd].sendq[i] = -1;
/*				fprintf( stderr, "Removed Pb[%d]\n" , s);*/

				if( checkclosable(sfd)){
					closeslot(sfd);
				}
				break;
			}
		}
		return 0;
	}
	
	/* CLOSE$B$,$-$?$iB(Ez!#$3$C$A$O(Bclose$B$N=hM}$r$7$F$7$^$&!#(B*/
	/* CLOSE$B$O(B2$B=E$K$-$F$b$9$G$K=hM}$5$l$F$$$?$i<+F0E*$KL5;k$5$l$k(B(use$B$r$_$k$+$i(B) */
	if( cmd == COMMAND_CLOSE ){
		
		if( rfd < 0 || rfd >= rp.maxslot || slot[rfd].use == 0) return -43344;

		makeheader( senddata , 0 , COMMAND_CLOSEACK , 0 , sfd , rfd );
		regpacket_send( ip_net , port_net , senddata , HEADERSIZE );

/*		fprintf( stderr , "send a closeack. close flag on.\n");*/
		slot[rfd].close_wait = 1;

		/* close$B2DG=$@$C$?$iJD$8$k(B */
		if( checkclosable( rfd ) ){
			closeslot(rfd);
		}
		return 0;
	}

	/* CLOSE_ACK$B$,$-$?$i(Bclose_wait$B$r$?$F$k!#(B */
	/* CLOSE_ACK$B$b(B2$B=EAw?.$K$D$$$F$OBg>fIW$G$"$k!#(B */
	if( cmd == COMMAND_CLOSEACK ){

/*		fprintf( stderr , "received close ack.\n");*/
		
		if( sfd < 0 || sfd >= rp.maxslot || slot[sfd].use==0 ||
		   slot[sfd].open_ack == 0 ) return -478878;

		removepb(  slot[sfd].close_pbi );
		slot[sfd].close_pbi = -1;
		slot[sfd].close_wait = 1;
		if( checkclosable(sfd)){
			closeslot(sfd);
		}
		return 0;
	}

	return 0;
}

/*
   $B%9%m%C%H$rJD$8$k$3$H$,$G$-$k$+$I$&$+$r$7$i$Y$k!#(B
   $BJD$8$k$3$H$,$G$-$k>r7o$O!"(B
   1 close_wait $B$,(B 1
   2 $BFI$_$3$_IT2D(B ( recvq$B$,(B $B$<$s$V(B -1 )
   3 $BAw?.BT$A$N%G!<%?$,$J$$(B ( sendq$B$,$<$s$V(B -1 )

   $B:#$9$0JD$8$k$3$H$,$G$-$k$J$i(B1$B!"$=$&$G$J$$$J$i(B0$B$r$+$($9(B
   
*/   
static int checkclosable( int fd)
{
	int i;

	if( slot[fd].close_wait == 0 ) return 0;

	for(i=0;i<QSIZE;i++){
		if( slot[fd].sendq[i] != -1 ||
		    slot[fd].recvq[i] != -1 ){
			return 0;
		}
	}
	return 1;
}


int now_pbi = 0;

static int findpb( void )
{
	int i;

	for(i=0;i<rp.pbsiz;i++){
		now_pbi++;
		if( now_pbi == rp.pbsiz ) now_pbi = 0;

		if( pb[now_pbi].use == 0 ){
			pb[now_pbi].use = 1;
			return now_pbi;
		}
	}
	fprintf( stderr , "FINDPB RUNSHORT!\n");
#if 1
	{
		FILE *fp;
		fp = fopen( "pb.txt" , "w");
		for(i=0;i<rp.pbsiz;i++){
			fprintf( fp ,"%d use:%d state:%d \n",
					i,pb[i].use ,pb[i].state);
		}
		fclose(fp);
		exit(0);
	}	
#endif
	
	return -1;
}
static void removepb( int ind )
{
	pb[ind].use = 0;
}

static int findslot(void)
{
	int i;
	for(i=0;i<rp.maxslot;i++){
		if( slot[i].use == 0 ){
			int j;
			
			bzero( &slot[i] , sizeof( slot[0]));
			slot[i].use = 1;
			for(j=0;j<QSIZE;j++){
				slot[i].sendq[j]=-1;
				slot[i].recvq[j]=-1;
			}
			return i;
		}
	}

	return -1;
}

/*
   $B%9%m%C%H$r$H$8$k(B
 */
static void closeslot( int fd )
{
	
	if( fd < 0 || fd >= rp.maxslot || slot[fd].use == 0  ) return;

	bzero( &slot[fd] , sizeof( slot[0] ));
	
}

static void printheader( char *st , char *buf )
{
	int a,b,c,d,e;
	getheader( buf , (unsigned long*)&a,&b,&c,&d,&e );
	fprintf( stderr,"%s SEQ:%d CMD:%d LEN:%d SFD:%d RFD:%d\n", st ,
					a,b,c,d,e );
}
static int calcpbuse(void)
{
	int i;
	int c = 0;
	for(i=0;i<rp.pbsiz;i++){
		if( pb[i].use )c++;
	}
	return c;
}
static int calcrecvquse(int fd )
{
	int i;
	int c = 0;
	for(i=0;i<QSIZE;i++){
		if( slot[fd].recvq[i] != -1 ) c++;
	}
	return c;
}
static int calcsendquse( int fd )
{
	int i;
	int c = 0;
	for(i=0;i<QSIZE;i++){
		if( slot[fd].sendq[i] != -1 ) c++;
	}
	return c;	
}

/*****************************************/

static int sender( char *data , int len , unsigned long ip_net , unsigned short port_net)
{
	int ret;
	
	struct sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port =  port_net ;
	to.sin_addr.s_addr = ip_net;
/*	fprintf( stderr, "Sender: addr:%x port:%d len:%d\n" , ip_net , ntohs(port_net),len );*/

	ret= sendto( rp.sockfd , data, len , 0,
				  (struct sockaddr*)&to, sizeof( struct sockaddr ) );

	if( ret> 0){
		fprintf( stderr , "sendto_worked\n" );
	}
	return ret;
}


double getUTimeDouble(void)
{
	struct timeval tm;

	gettimeofday( &tm , NULL );

	return (double)( tm.tv_usec ) + (double)( tm.tv_sec) *
	1000000.0 ;
}

