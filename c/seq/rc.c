#define _RC_C_

#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>

#include <malloc.h>
#include <strings.h>

#include "rc.h"

struct rc
{
	int maxslot;
	int sockfd;
} rc;


#define MTU 256
struct sendp			/* 送信用パケット */
{
	int state;
#define SENDP_NOUSE		0
#define SENDP_SENDWAIT	109
#define SENDP_ACKWAIT	209

	double sendtime;		/* retry用。addsendpのときには初期化され
							 ない。*/
	double nexttime;
	int retrytimes;

	int seq,cmd, len, sfd , rfd;
	
	char buf[MTU];
};
struct recvp			/* 受信用パケット */
{
	int state;
#define RECVP_NOUSE 	0
#define RECVP_SENDACK	1112
	/* 値のコピーではなく、送信するときに以下の情報をもとに
	   パケットヘッダを作成する。 */
	int seq,cmd, len, sfd , rfd;	
	char buf[MTU];
};

#define COMMAND_OPEN		0x01010101
#define COMMAND_OPENACK		0x02020202
#define COMMAND_CLOSE		0x03030303
#define COMMAND_CLOSEACK	0x04040404
#define COMMAND_DATA		0x05050505
#define COMMAND_DATAACK		0x06060606

#define SEQ_NOUSE  	0xffffffff		/* addsendp , addrecvpの引数で不使用を意味する値 */
#define LEN_NOUSE	0x00000000
#define SFD_NOUSE	0xffffffff
#define RFD_NOUSE 	0xffffffff
#define BUF_NOUSE	0x00000000
	



#define SEQ_OFS 0
#define CMD_OFS 4
#define LEN_OFS 8
#define SFD_OFS 12
#define RFD_OFS 16
#define HEADERSIZE 20

struct slot
{

	int use;
	unsigned long ip_net;		/* 相手のIPアドレス*/
	unsigned short port_net;		/* 相手のポート*/
	int dslot;					/* 相手のスロットindex */
	
	unsigned int send_i;		/* 最後にregしたパケットの seq(addr)*/
	unsigned int recv_i;		/* 次に読みこむパケットの seq(addr)*/

	int open_send;				/* openを送信したら1 */
	int open_ack;				/* openのackがきたら1それまで0 */
	int accept_wait;			/* acceptされるのをまっているなら1 */
	int close_wait;				/* closeされるのをまっている状態なら1。
								   CLOSEコマンドがおくられてきたら1にし、
								   ぜんぶのデータをよみこむまでは実際には
								   クローズしない。このフラグが1の状態で
								   読みこめるデータも書きこめるデータも
								   なくなったら、useを0にする。
								   closeできるかもしれないタイミングは、
								   CLOSEコマンドがきたとき、
								   CLOSEACKコマンドがきたとき、
								   rp_recvしたとき(recvqを消化)、
								   DATAACKをうけとってsendqを消化した
								   ときである。
								   それらのときにチェックしてclose可能
								   であれば、closeする。
								   */

#define SENDQSIZE	8
#define RECVQSIZE	8
	struct sendp sendq[SENDQSIZE];
	struct recvp recvq[RECVQSIZE];
};


struct slot *slot;

#define FDINVALID(fd)	( (fd)<0 || (fd) >= rc.maxslot || slot[fd].use==0)


#define RETRY_BASE (4.0*1000*1000)
double timetable[] = {
	    RETRY_BASE * 1 ,
		    RETRY_BASE * 2 ,
		    RETRY_BASE * 3 ,
		    RETRY_BASE * 4 ,
		    RETRY_BASE * 6 ,
		    RETRY_BASE * 8 ,
		    RETRY_BASE * 12 ,
		    RETRY_BASE * 20
	};

static int findslot(void);
static int addrecvp( int fd , int seq , int cmd , int len , int sfd ,
			 int rfd , char *buf );

static int addsendp( int fd ,int seq,int cmd , int len, int sfd ,
			  int rfd ,char *buf );

static void deletesendp( struct sendp *sp );
static void deleteopensendp( int fd );
static void deleterecvp( struct recvp *rp );
static void getheader( char *buf , int buflen , int *seq , int *cmd , int *len ,
			   int *sfd, int *rfd , char **data );
static void deleteclosesendp( int fd );
static int closable( int fd );
static void closeslot( int fd );
static int sender(  unsigned long ip_net , unsigned short port_net,
				   int seq  , int cmd , int len , int sfd , int rfd, char *buf );
static int select_readable(void );
static int select_writable(void );

/***************** インターフェイス関数群 *****************/

int rc_init( unsigned short port ,int maxslot )
{
	int i;
	struct sockaddr_in sin;
	int ret;
	
	if( (rc.sockfd = socket( AF_INET , SOCK_DGRAM , 0 ))<0)return -1;

    sin.sin_family = AF_INET;
    sin.sin_port = htons( port );
    sin.sin_addr.s_addr = INADDR_ANY;
    if((ret = bind( rc.sockfd ,(struct sockaddr*)&sin,sizeof(sin)))<0){
		return i;
	}

	rc.maxslot = maxslot;

	slot = (struct slot*) malloc( sizeof( struct slot ) * maxslot );
    if( slot == NULL ) return -2;

	return 0;
}

int rc_open( unsigned long ip_net ,unsigned short port_net )
{

	int fd;

	if( (fd = findslot())< 0 ) return -2;

	slot[fd].ip_net = ip_net;
	slot[fd].port_net = port_net;

	if(addsendp( fd , SEQ_NOUSE , COMMAND_OPEN ,
				LEN_NOUSE , fd , RFD_NOUSE , BUF_NOUSE )<0){
		return -100;
	}

	slot[fd].open_send = 1;

	while(1){
		rc_proc();
		if( slot[fd].open_ack){
			break;
		}
	}
	return fd;
}

/*
   acceptできたら0以上のfdをかえす。できんかったら負。

   
*/   
int rc_accept( void )
{
	int i;
	
	for(i=0;i<rc.maxslot;i++){
		if( slot[i].use && slot[i].accept_wait ){
			return i;
		}
	}
	return -1;
}

/*
   書きこみ。COMMAND_DATAのパケットを作るのはここだけ

   いつでもwriteできる。そして0バイト以上のかきこみが可能である。
	エラーになることはない。
	
*/

int rc_write( int fd ,char *buf , int len )
{
	int ret;
	
	if( FDINVALID( fd ) ) return -100;

	/* この制限はとっぱらいたい。 PENDING すぐだろう */
	if( len > MTU ) return -303;

	ret = addsendp( fd ,
			 slot[fd].send_i ++ ,
			 COMMAND_DATA ,
			 len ,
			 fd ,
			 slot[fd].dslot ,
			 buf );

	if( ret < 0 ) return -808;

	return len;
	
}

/*
   lenを0にすると、読みこみ可能かどうかのテストをする
   モードになり、読みこみ可能な長さを返す。0をかえしたら読みこみ不可。
   
 */
int rc_read( int fd,  char *buf , int len  )
{
	int i;
	
	if( FDINVALID( fd ) ){
		fprintf( stderr, "AHOAHARUCK\n");
		return -1;
	}

	/* この制限はすぐにでもなくせるだろう。 PENDING */
	if( len < MTU ) return -2;

	/* ACKを送ったもののうちrecv_iと同じものを読みこむ。
	   ストリームらしくするには、複数のものを同時によみこめる
	   ようにするだけ。PENDING
	   それはこの部分を変更するだけでできる。すなわち
	   複数のパケットを通してサーチできるようにするのだ。 */
	
	for(i=0;i<RECVQSIZE;i++){
		if( slot[fd].recvq[i].state == RECVP_SENDACK &&
		   slot[fd].recvq[i].seq == slot[fd].recv_i ){
			int copylen;

			copylen= slot[fd].recvq[i].len;

/*			fprintf( stderr , "READ!!!! fd:%d RQi:%d Len:%d\n",fd , i , copylen );*/
			if( copylen > 0 )
				bcopy( slot[fd].recvq[i].buf , buf , copylen );

			slot[fd].recv_i ++;

			slot[fd].recvq[i].state = RECVP_NOUSE;
			if( closable( fd ) )closeslot(fd);
			
			return copylen;
		}
	}
	
	return 0;
}

int rc_close( int fd )
{
	if( FDINVALID( fd ) ) return -1;

	addsendp( fd , SEQ_NOUSE , COMMAND_CLOSE ,
			 LEN_NOUSE , fd, slot[fd].dslot , BUF_NOUSE );
	slot[fd].close_wait = 1;

	while(1){
		rc_proc();
		if( slot[fd].use == 0 ){
			break;
		}
	}
	return 0;
}

int do_first = 0;	/* こんかいのよびだしでは、どのslotから処理をはじめるのか */
double lasttime;

#define INTERVAL_MS ( 100.0 )


int rc_proc(void)
{
	int i;
	int ll;
	double now_time = getUTimeDouble();

	if( ( now_time - lasttime ) < (INTERVAL_MS*1000)) return 0;
	lasttime = now_time;


	fprintf(stderr , "!");

	for(ll=0;ll<rc.maxslot;ll++){
		int j;
		struct sendp *sq;
		
		/* 誰を処理するのか */
		do_first++;
		if( do_first == rc.maxslot ) do_first = 0;
		
		if( slot[do_first].use == 0 ) continue;
		sq = slot[do_first].sendq;

		
		/* 書きこみ不可になるか、全部のスロットを処理しきるまで書きつづける */
		if(  ! select_writable()) break;
		
		
		for(j=0;j<SENDQSIZE;j++){
			if( sq[j].state == SENDP_SENDWAIT ){
				/* 送信まちになっているぞ */
				if(sender( slot[do_first].ip_net , slot[do_first].port_net ,
						  sq[j].seq , sq[j].cmd , sq[j].len,
						  sq[j].sfd , sq[j].rfd ,sq[j].buf )<=0){
					/* 送信できなかったらとりあえずやめる */
					break;
				} else {
				}
					
				sq[j].sendtime = now_time;
				sq[j].nexttime = now_time + timetable[0];
					
				switch( sq[j].cmd ){
					case COMMAND_DATA:
					case COMMAND_OPEN:
					case COMMAND_CLOSE:
					sq[j].state = SENDP_ACKWAIT;
					break;
					case COMMAND_DATAACK:
					case COMMAND_OPENACK:
					case COMMAND_CLOSEACK:
					deletesendp( &sq[j] );
					break;
					default:	/* ERROR!!!!!!*/
					break;
				}
				if( select_writable() ) continue; else break;
			} else if( sq[j].state == SENDP_ACKWAIT){
				/* 1回送信して、ACKをまっているのだ。時間次第で再送 */
					
				if( sq[j].nexttime < now_time ){
					sq[j].retrytimes++;
					if( sq[j].retrytimes == sizeof( timetable)/sizeof(timetable[0])){
						closeslot( do_first );
						fprintf( stderr, "TIMEOUT on slot %d\n" , do_first);
					} else {

						fprintf( stderr, "RETRY Qi:%d fd:%d\n" ,j , do_first );
						if( sender( slot[do_first].ip_net , slot[do_first].port_net ,
								   sq[j].seq , sq[j].cmd , sq[j].len,
								   sq[j].sfd , sq[j].rfd ,sq[j].buf )<=0){
							break;
						}
						sq[j].nexttime = sq[j].sendtime +
						timetable[sq[j].retrytimes];
					}
				}
					
			} else {
				/* 何もせん */
			}
		}
	}		/* while */



	while(1){
		struct sockaddr_in sin;
		int clilen = sizeof( sin );
		int n;
		int ret;
		char buf[MTU];
		int seq , cmd , len , sfd , rfd;
		char *data;
		unsigned long ip_net;
		unsigned short port_net;

		/* 読みこみ可能なら永久に読みつづける */
		if( select_readable()){
			n = recvfrom( rc.sockfd , buf , sizeof( buf ), 0 ,
						 ( struct sockaddr*)&sin , &clilen );
			if( n <= 0 )break;
		} else {
			break;
		}

		getheader( buf , n , &seq , &cmd, &len , &sfd , &rfd , &data );
		ip_net = sin.sin_addr.s_addr;
		port_net = sin.sin_port;

		fprintf( stderr ,"PACKET!  SEQ:%x CMD:%x LEN:%x SFD:%x RFD:%x\n",
				seq,cmd,len,sfd,rfd);


		/* COMMANDによってわける */
		if( cmd == COMMAND_DATA ){

			if( FDINVALID( rfd ) ) continue;
				
			/* 2重チェックをここでする */
			for(i=0;i<RECVQSIZE;i++){
				if( slot[rfd].recvq[i].seq == seq &&
				   slot[rfd].recvq[i].state == RECVP_SENDACK )break;
			}
			if( i != RECVQSIZE ){
				fprintf( stderr, "Ignored datapacket. seq:%d\n" , seq );
				continue;
			}

			addrecvp( rfd , seq , cmd , len , sfd , rfd , data );

			/* ACKを送信 */
			addsendp( rfd , seq , COMMAND_DATAACK , LEN_NOUSE , sfd , rfd , BUF_NOUSE );
				
		} else if( cmd == COMMAND_OPEN ){
				
			int si;

				
			/* 2重openをチェック */
			for(i=0;i<rc.maxslot;i++){
				if( 
				   slot[i].ip_net == ip_net &&
				   slot[i].port_net == port_net &&
				   slot[i].dslot == sfd &&
				   slot[i].use  ){
					break;
				}
			}
			if( i != rc.maxslot) continue;
				
			if( (si = findslot())<0) continue;
			if( addsendp( si , SEQ_NOUSE , COMMAND_OPENACK , LEN_NOUSE ,
						 sfd , si , BUF_NOUSE )<0){
				/* ACKおくれん */
				fprintf( stderr , "CLOSE BECAUSE OF OPENACK FUCK\n");
				closeslot(si);
			} else {
				slot[si].ip_net = ip_net;
				slot[si].port_net = port_net;
				slot[si].dslot = sfd;
				slot[si].send_i = slot[si].recv_i = 0;
				slot[si].open_ack = 0;
				slot[si].accept_wait = 1;

				fprintf(stderr,"NASUNASU Si:%d\n", si );
					
			}
				
		} else if( cmd == COMMAND_CLOSE ){
			if( FDINVALID( rfd ) ) continue;
		} else if( cmd == COMMAND_DATAACK ){
			if( FDINVALID( sfd ) ) continue;

			fprintf( stderr , "RECEIVED DATA ACK. SEQ:%d\n" , seq );

			/* 到着したパケットと同じseqをもつsendqをさがす */
			for(i=0;i<SENDQSIZE;i++){
				if( slot[sfd].sendq[i].seq == seq &&
				   slot[sfd].sendq[i].state == SENDP_ACKWAIT ){
					/* 到着を確認！ 確認の条件はseqが同じでかつ */
					/* ACKWAITしていること。おなじseqをもつものはすべて消してしまう。*/
					deletesendp( & slot[sfd].sendq[i] );
					fprintf( stderr,  "Confirmed seq %d\n", seq );
				}

			}
			/* ACKまちを消せたら、閉じれる状態かもしれない。 */
			if( closable( sfd ) )closeslot(sfd);

				
		} else if( cmd == COMMAND_OPENACK ){
			if( FDINVALID( sfd ) )continue;

			slot[sfd].open_ack = 1;
			slot[sfd].dslot = rfd;
			fprintf( stderr, "BBBBBBBBBBBBBBBBB RFD:%d\n" , rfd );
			deleteopensendp( sfd );
				
		} else if( cmd == COMMAND_CLOSEACK ){
			if( FDINVALID(  sfd ) )continue;
			deleteclosesendp( sfd );
			slot[sfd].close_wait = 1;
		}
	}
}


/************** 下位ルーチン群 *********************/


/*
   実際に送信する
   
*/   
static int sender(  unsigned long ip_net , unsigned short port_net,
				   int seq  , int cmd , int len , int sfd , int rfd, char *buf )
{

	int ret;
	char data[HEADERSIZE + MTU];
	struct sockaddr_in sin;
	int datalen;
	
	sin.sin_family = AF_INET;
	sin.sin_port = port_net;
	sin.sin_addr.s_addr = ip_net;


	datalen = len;
	
	fprintf( stderr , "SENDER SEQ:%x CMD:%x LEN:%x SFD:%x RFD:%x\n",
			seq,cmd,len,sfd,rfd);
	
	
	seq = htonl( seq );
	cmd = htonl( cmd );
	len = htonl( len );
	sfd = htonl( sfd );
	rfd = htonl( rfd );
	
	bcopy((char*)&seq , (char*)(data + SEQ_OFS) , 4 );
	bcopy((char*)&cmd , (char*)(data + CMD_OFS) , 4 );
	bcopy((char*)&len , (char*)(data + LEN_OFS) , 4 );
	bcopy((char*)&sfd , (char*)(data + SFD_OFS) , 4 );
	bcopy((char*)&rfd , (char*)(data + RFD_OFS) , 4 );
	bcopy( buf , data + HEADERSIZE ,datalen );	/* lenを変換する前に */
	
	ret = sendto( rc.sockfd , data , datalen + HEADERSIZE , 0 ,
				 (struct sockaddr*)&sin , sizeof( sin )) ;


	return ret;
	

}
				   
static void getheader( char *buf , int buflen , int *seq , int *cmd , int *len ,
			   int *sfd, int *rfd , char **data )
{
	*seq = ntohl(  *(long*)(buf + SEQ_OFS ));
	*cmd = ntohl(  *(long*)(buf + CMD_OFS ));	
	*len = ntohl(  *(long*)(buf + LEN_OFS ));
	*sfd = ntohl(  *(long*)(buf + SFD_OFS ));
	*rfd = ntohl(  *(long*)(buf + RFD_OFS ));
	*data = buf + HEADERSIZE;

/*	fprintf( stderr ,"GETHEAD "
			"%x %x %x %x  "
			"%x %x %x %x  "
			"%x %x %x %x  "
			"%x %x %x %x  "
			"%x %x %x %x  " ,
			buf[0] & 0xff ,	buf[1] & 0xff ,	buf[2] & 0xff ,	buf[3] & 0xff ,
			buf[4+0] & 0xff ,	buf[4+1] & 0xff ,	buf[4+2] & 0xff ,	buf[4+3] & 0xff ,
			buf[8+0] & 0xff ,	buf[8+1] & 0xff ,	buf[8+2] & 0xff ,	buf[8+3] & 0xff ,
			buf[12+0] & 0xff ,	buf[12+1] & 0xff ,	buf[12+2] & 0xff ,	buf[12+3] & 0xff ,
			buf[16+0] & 0xff ,	buf[16+1] & 0xff ,	buf[16+2] & 0xff ,	buf[16+3] & 0xff );
*/
	
}


/*
   sendpをくわえる。
   負は失敗。0は成功。
   indexの情報は上位には隠されているべきで、
   この関数は返さない。また、 fdの値は信用する。lenはMTU
   を越えないことを仮定している。addrecvpでも同じ。
   lenはbufの長さであるよ。
   
   下位のルーチンは上位が正しい値を入れてくることを仮定する。
   
   
*/   
static int addsendp( int fd ,int seq,int cmd , int len, int sfd ,
			  int rfd ,char *buf )
{
	int i;

	for(i=0;i<SENDQSIZE;i++){
		if( slot[fd].sendq[i].state == SENDP_NOUSE){
			slot[fd].sendq[i].state = SENDP_SENDWAIT;
			slot[fd].sendq[i].seq = seq;
			slot[fd].sendq[i].cmd = cmd;
			slot[fd].sendq[i].len = len;
			slot[fd].sendq[i].sfd = sfd;
			slot[fd].sendq[i].rfd = rfd;
			if( buf != BUF_NOUSE) bcopy( buf , slot[fd].sendq[i].buf , len );
			slot[fd].sendq[i].sendtime = 0.0;
			slot[fd].sendq[i].nexttime = timetable[0];
			slot[fd].sendq[i].retrytimes = 0;

			fprintf(stderr,"ADDed sendp. Qi:%d fd:%d sfd:%d rfd:%d\n",i,fd,sfd,rfd);
			/* ここまででsendpは完全に初期化された。 */
			return 0;
		}
	}
	return -1;
}
static void deletesendp( struct sendp *sp )
{
	bzero( (char*)sp , sizeof( struct sendp ) );
}
static void deleteopensendp( int fd )
{
	int i;
	for(i=0;i<SENDQSIZE;i++){
		if( slot[fd].sendq[i].state == SENDP_ACKWAIT &&
		    slot[fd].sendq[i].cmd == COMMAND_OPEN ){
			deletesendp( & slot[fd].sendq[i] );
			break;
		}
	}
}
static void deleteclosesendp( int fd )
{
	int i;
	for(i=0;i<SENDQSIZE;i++){
		if( slot[fd].sendq[i].state == SENDP_ACKWAIT &&
		    slot[fd].sendq[i].cmd == COMMAND_CLOSE ){
			deletesendp( & slot[fd].sendq[i] );
			break;
		}
	}
}

/*
   ACKを送信したあとにrecvpをくわえる。
   負は失敗。0は成功。
*/
static int addrecvp( int fd , int seq , int cmd , int len , int sfd ,
			 int rfd , char *buf )
{
	int i;
	for(i=0;i<RECVQSIZE;i++){
		if( slot[fd].recvq[i].state == RECVP_NOUSE ){
			slot[fd].recvq[i].state = RECVP_SENDACK;
			slot[fd].recvq[i].seq = seq;
			slot[fd].recvq[i].cmd = cmd;
			slot[fd].recvq[i].len = len;
			slot[fd].recvq[i].sfd = sfd;
			slot[fd].recvq[i].rfd = rfd;
			if( buf != BUF_NOUSE) bcopy( buf , slot[fd].recvq[i].buf , len );

#if 0
			fprintf( stderr, "Added recvp. Qi:%d fd:%d\n" , i , fd );

			{
				int k;
				for(k=0;k<RECVQSIZE;k++){
					fprintf( stderr,"RECVQ[%d].state=%d seq=%d len=%d\n",
							k , slot[fd].recvq[k].state , slot[fd].recvq[k].seq,
							slot[fd].recvq[k].len );
				}
				
			}
#endif
			
			/* ここまででrecvqは完全に初期化された。 */
			return 0;
		}
	}
	return -1;
}
static void deleterecvp( struct recvp *rp )
{
	bzero( (char*)rp , sizeof( struct recvp ) );
	
}



static int findslot(void)
{
	int i;
	for(i=0;i<rc.maxslot;i++){
		if( slot[i].use == 0 ){
			bzero( &slot[i] , sizeof( slot[0]));
			slot[i].use = 1;

			fprintf(stderr, "FINDSLOT: return %d\n" , i );
			return i;
		}
	}
	return -1;
}

/*
   閉じれる条件とは close_waitが立っていてかつ、ACK待ちものこってなくて、
   読みのこしもない状態である。
   

   0 : not able to close it
   1 : ok to close
   
*/   
static int closable( int fd )
{
	int i;
	
	if( FDINVALID( fd ) ) return  0;

	if( ! slot[fd].close_wait )return 0;
	
	for(i=0;i<SENDQSIZE;i++){
		if( slot[fd].sendq[i].state != SENDP_NOUSE )return 0;
	}
	for(i=0;i<RECVQSIZE;i++){
		if( slot[fd].recvq[i].state != RECVP_NOUSE )return 0;
	}

	return 1;
}


static void closeslot( int fd )
{
	fprintf( stderr ,"CLOSING FD:%d\n", fd );
	bzero( &slot[fd] , sizeof( struct slot ));
	
}



double getUTimeDouble(void)
{
	struct timeval tm;

	gettimeofday( &tm , NULL );

	return (double)( tm.tv_usec ) + (double)( tm.tv_sec) *
	1000000.0 ;
}

static int select_readable(void )
{
	fd_set rfd;
	struct timeval tm;
	
	tm.tv_sec = 0;
	tm.tv_usec = 0;

	FD_ZERO( &rfd );
	FD_SET( rc.sockfd , &rfd );
	select( 10 , &rfd , (fd_set*)0,(fd_set*)0, &tm );

	return FD_ISSET( rc.sockfd , &rfd );
}

static int select_writable(void )
{
	fd_set wfd;
	struct timeval tm;
	
	tm.tv_sec = 0;
	tm.tv_usec = 0;

	FD_ZERO( &wfd );
	FD_SET( rc.sockfd , &wfd );
	select( 10 , (fd_set*)0, &wfd , (fd_set*)0, &tm );

	return FD_ISSET( rc.sockfd , &wfd );
}








