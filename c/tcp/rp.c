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

	int maxslot;		/* 最大のスロット数 */
	int pbsiz;
	unsigned short port;	/* 自分のシステムのポート */

	int sockfd;
	
} rp;

struct slot
{
	int use;					/* 使用中かどうか */
	
	unsigned long di_net;		/* 相手のIPアドレス( net byte order ) */
	unsigned short dp_net;		/* 相手のポート( net byte order ) */
	int dslot;					/* 相手のスロット */
	
	unsigned int send_i;		/* 書きこみ番号 最後にregしたパケットの*/
	unsigned int recv_i;		/* 読みこみ番号 次に読みこむパケットの */

	int open_ack;				/* openのackがきたら1 */
	int accept_wait;			/* acceptされるのをまっている */

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
/*	unsigned int close_i;			*/
								   
	
	double last_in;				/* 最後にパケットがきたのはいつか */


#define QSIZE	8
	int sendq[QSIZE];			/* 書きこみbufへのindexの表 -1は不使用 */
	int recvq[QSIZE];			/* 読みこみbufへのindexの表 -1は不使用 */

	int open_pbi , close_pbi;	/* open , closeのパケットを送信したら
								   どのパケットバッファをつかったかこ
								   こに記憶 */
	
};
struct slot *slot;


#define SEQ_OFS 0				
#define CMD_OFS ( SEQ_OFS + 4 )
#define LEN_OFS ( CMD_OFS + 4 )		/* この長さにはデータ部分の長さを */
									/* いれる */
#define SFD_OFS ( LEN_OFS + 4 )
#define RFD_OFS	( SFD_OFS + 4 )
#define HEADERSIZE ( RFD_OFS + 4 )	/* このオフセットからデータがはじまる */

#define COMMAND_DATA		0x00000001		/* データ用パケット */
#define COMMAND_DATAACK		0x00000002		/* ACK用 */
#define COMMAND_OPEN		0x00000008		/* ひらく時につかう */
#define COMMAND_CLOSE		0x0000000a		/* とじるときにつかう */
#define COMMAND_OPENACK		0x00000080		/* openのACK */
#define COMMAND_CLOSEACK	0x000000a0		/* closeのACK */


/* 再送のタイミングの表。 この表は大きさを適当に変えたりしてよい。
   この表をやりつくすまでリトライする。
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
	RETRY_BASE * 20 };		/* この時間をすぎると接続を切る。 */

/* メモリのむだ使いをさけるためのよくあるはいれつ */
struct pb
{
	int use;

	int state;
#define PBS_SENDWAIT	1			/* 送信をまっている 書きこみ用*/
#define PBS_ACKWAIT		2			/* ACKをまっている  書きこみ用*/
#define PBS_SENDACK		8			/* ACKを送信した。recvをまっている 読みこみ用 */

	double sendtime;		/* いつ送信したか。0でない間じゅう、
							   再送しつづける。ACKがきたら自動的に不使用になる。*/
	double next_time;		/* 次に送信する時間。 */
	int retry_times;		/* 再送をした回数。 当然最初は0からはじま */
							/* る。再送をするたびにincrement*/
	
	unsigned long ip_net;
	unsigned short port_net;
	
	int totallen;

	unsigned long seq;			/* パケットのヘッダのコピー */
	int cmd , len , sfd , rfd;
	
#define MTU		512
	char buf[MTU+HEADERSIZE];
};
struct pb *pb;

#define PBSIZ(slotnum)  ( (slotnum)*8 )		/* PBの個数をきめるマクロ */


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
   初期化する 
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
	
	/* メモリを確保して初期化 */
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

   相手システムに接続する(ブロックする)
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
   新しい相手を認識する
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
   acceptできる接続があるかどうかしらべる。
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
   接続が閉じられてしまっているかどうかをしらべる。

*/
int rp_isclosed( int fd )
{
	if( fd < 0 || fd >= rp.maxslot ) return 1;

	if( slot[fd].use == 0) return 1;

	return 0;
}


/*
   書きこむ
*/
int current_send_i;/*for debug */
int rp_send( int fd, char *buf , int len )
{
	char data[MTU+HEADERSIZE];
	int ind;
	int pbind;


	if( fd < 0 || fd >= rp.maxslot || slot[fd].use == 0) return -1;



	/* この制限もとっぱらうことができる。 PENDING */
	if( len > MTU )return -3;


	/* スロットの中を検索して、SENDWAITになっているもののうち最後のもの
	   がMTUに達っしてない場合は、そこに足しこみ、
	   余った分を普通に処理する。ここの部分を働かなくするだけで、
	   圧縮なし転送モードになる。*/
#if 0

	for(i=0;i<QSIZE;i++){
		int sq = slot[fd].sendq[i];
		if( sq != -1 && pb[sq].use &&
		    pb[sq].state == PBS_SENDWAIT &&
		    pb[sq].len < MTU ){
			/* ここでpbの内容をいじるぞ */
			int left = MTU - pb[sq].len;
			int copylen;
			if( len <= left ){
				copylen = len;
			} else {
				copylen = left;
			}
			bcopy( buf , pb[sq].buf + HEADERSIZE + pb[sq].len , copylen );

			
			/* バッファをずらす */
			buf += copylen;
			len -= copylen;

			/* pbをいじる */
			pb[sq].len += copylen;
			pb[sq].totallen += copylen;
			modifyheader_len( pb[sq].buf  , pb[sq].len );



			
			/* lenを全部こぴーできてしまったら、この後はいらん。 */
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
	
	
	/* まず、スロットの書きこみバッファ表のあきをさがす。 */
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
	
	/* カウンターをひとつすすめる */
	slot[fd].send_i++;

	fprintf( stderr, "SUCCESSFUL_RP_SEND! now send_i : %d\n" ,
			slot[fd].send_i );
	return len;
}

/*
   読みこむ

   読みこんだ長さをかえす。lenで指定した値がメッセージの長さより
   短い場合は、あまったデータは捨てられる。
   
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

				/* 読みこみ終わったらそのpbは解放するとともに-1をいれる */
				removepb( rq );
				slot[fd].recvq[i] = -1;
				{
					FILE *fp;
					fp = fopen( "setsendack.txt" , "a+");
					fprintf( fp , "UNSET %d RQi:%d\n" , rq ,i);
					fprintf( stderr , "UNSET %d RQi:%d\n" , rq ,i);
					fclose(fp);
				}
			
				/* 読みこみポインタを1すすめる */
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
   そのslotから読みこむことができるかどうかをしらべる。
   1ならよみこみ可能。0なら読みこめない。
   
   
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
   かきこみ可能かどうかをしらべる。
   1なら書きこめる。0なら書きこめない。

   sendqに空きがあるかどうかをしらべるだけでよい。pbがいっぱいになるかどうか
   は考えにいれない。それはsendの失敗としてあらわれる。
   
   
   
   
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
   閉じる(ブロックする)
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
   実際の処理をさせる
*/

int send_first = 0;
double last_proc;		/* 前回処理した時間 */
int proc_count = 0;		/* rp_procを実行した回数 */


#define INTERVAL_MSEC ( 100.0 )

int rp_proc( void)
{
	int i;
	int recvcount = 0 , sendcount=0;


	
	/* 時間制限 */
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
		int sent_one = 0;	/* ひとつでも送信すると1 */
		int is_next_send = 0;	/* もっと送信したいなら1 */
		int read_ok = 0;		/* 読みこみ可能なら1 */
		
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
					/* 送信をまっている状態だった */
					if( sent_one == 0 ){
						if( sender( pb[send_first].buf ,pb[send_first].totallen,
							   pb[send_first].ip_net , pb[send_first].port_net) ==
						   pb[send_first].totallen ){
							fprintf( stderr, "ANYWAY_SEND\n" );
							pb[send_first].sendtime = now_time;
							/* 次に送信する時間を設定 */
							pb[send_first].next_time =
								pb[send_first].sendtime + timetable[0];
							pb[send_first].retry_times = 0;
							
							/* ACK以外は、ACKをまつ状態に遷移 */
							if( pb[send_first].cmd == COMMAND_DATA ||
							    pb[send_first].cmd == COMMAND_OPEN ||
							    pb[send_first].cmd == COMMAND_CLOSE ){
								pb[send_first].state = PBS_ACKWAIT;
							} else {
								/* ACKの場合は送信したらけす */
								removepb( send_first );
							}
							sent_one = 1;
							break;
						}
					} else {
						is_next_send = 1;		/* 送信できるものがまだある。 */
					}
				} else if(  pb[send_first].use && pb[send_first].state == PBS_ACKWAIT ){
					/* 時間をみて再送  */
					/* 再送は COMMAND_DATA , COMMAND_CLOSE , COMMAND_OPENのみ */

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
								/* 回数がオーバーしたから、
								   接続を閉じてしまう。 */
								
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
   すべての送信パケットはこの関数を通るのだ
   int len はパケット全体のサイズだ。

   かえりちは、入れたsendqへのインデクスだ。負は失敗
   
*/
static int regpacket_send( unsigned long ip_net , unsigned short port_net ,
					 char *data , int totallen )
{

	unsigned long seq;
	int cmd , len , sfd , rfd;
	int i;
	
	/* 最低でもヘッダーのサイズはあるはずだ。また大きすぎてもファックだ */
	if( totallen < HEADERSIZE ) return -1;
	if( totallen > ( HEADERSIZE + MTU) ) return -4;

	/* コピーをきおく */
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
	
	/* バッファをコピー */
	bcopy( data , pb[i].buf, totallen );

	return i;

}
/*
   読みこんだときのregpacket
   
*/   
static int regpacket_recv( unsigned long ip_net , unsigned short port_net,
						  char *data , int totallen )
{
	unsigned long seq;
	int cmd , len , sfd , rfd;
	char senddata[HEADERSIZE];
	int i;
	
	/* 最低でもヘッダーのサイズはあるはずだ。また大きすぎてもファックだ */
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
	
	/* スロットの情報がおかしかったら受けとらないぞ */
	if( sfd < 0 || sfd >= rp.maxslot ){
		return -55;
	}

	
	/* OPENがきたら即答で、パケットにあたらしいスロットのindexをいれてかえす */
	if( cmd == COMMAND_OPEN ){
		int si;
		int ret;
		
		/* いきなり空きを探して割りあてるのでは、2重にOPENがきたときに
		   バグる。なので、OPENのときはすでにopenされている接続のすべて
		   と比較して、アドレス、ポート、sfdが一致しているものがあるかどうか
		   探して、同じだったら無視する必要がある。 */

		for(i=0;i<rp.maxslot;i++){
			if( slot[i].use && slot[i].di_net == ip_net &&
			    slot[i].dp_net == port_net && slot[i].dslot == sfd ){
				/* 一致してたので、無視する。 */
	/*			fprintf( stderr , "Ignored duplicate OPEN command!\n");*/
				return 0;
			}
		}
		
		
		si = findslot();
		if( si<0)return -100;
		
		makeheader( senddata, 0 , COMMAND_OPENACK,0,sfd, si );
		ret = regpacket_send( ip_net , port_net ,senddata , HEADERSIZE );
		if( ret < 0 ){
			/* ACKを送れない */
			closeslot( si );
			return -1001;
		}
		

		/* あたらしいスロットを設定する */
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
	
	/* OPENの返事がかえってきたのだ */
	if( cmd == COMMAND_OPENACK &&	slot[sfd].use ){
		slot[sfd].open_ack = 1;
		slot[sfd].dslot = rfd;	/* 相手のスロットindex */
		slot[sfd].last_in = getUTimeDouble();
		fprintf( stderr , "regpacket_recv: slot %d received openack!\n", sfd );
		/* もとになったpbをリリース */
		removepb( slot[sfd].open_pbi );
		slot[sfd].open_pbi = -1;
		return 0;
	}

	if( cmd == COMMAND_DATA ){
		int pbi;
		int recvqind;
		int ret;
		
		/* まず、既存の接続に対する送信かどうかをしらべる。 */
		if( rfd < 0 || rfd >= rp.maxslot || slot[rfd].use == 0) return -4033;

		/* 送られてきたものをそのまま信用すると2重に送信されてきたとき */
		/* に問題が生じるので、これまでに受けとったものすべてと比較し */
		/* て、seqが同じものがあったら無視する */
		for(i=0;i<QSIZE;i++){
			if( slot[rfd].recvq[i] != -1 && pb[slot[rfd].recvq[i]].seq == seq ){
				/* seqが同じのがあったから無視する。 */
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

		/* もちぬしの表に書きこむのだ */
		slot[rfd].recvq[recvqind] = pbi;
/*		fprintf( stderr,"Added PBI:%d RECVQIND:%d RFD:%d\n",
				pbi , recvqind , rfd );*/

		/* ACKをかえす */
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
		
		/* データに対するACKがきた。 ACKは何重にきても2個目以上は
		 無視する(useをみるから)から大丈夫である。*/
		if( sfd < 0 || sfd >= rp.maxslot || slot[sfd].use == 0) return -4192;

		/* スロットの中のsendqを検索して、seqをさがす */
		for(i=0;i<QSIZE;i++){
			int s = slot[sfd].sendq[i];

/*			fprintf( stderr , "slot[%d].sendq[%d]=%d\n",sfd,i,
					slot[sfd].sendq[i] );*/
			
			if( s != -1 && pb[s].use && pb[s].seq == seq ){
				/* 一致したので消してしまう。 */
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
	
	/* CLOSEがきたら即答。こっちはcloseの処理をしてしまう。*/
	/* CLOSEは2重にきてもすでに処理されていたら自動的に無視される(useをみるから) */
	if( cmd == COMMAND_CLOSE ){
		
		if( rfd < 0 || rfd >= rp.maxslot || slot[rfd].use == 0) return -43344;

		makeheader( senddata , 0 , COMMAND_CLOSEACK , 0 , sfd , rfd );
		regpacket_send( ip_net , port_net , senddata , HEADERSIZE );

/*		fprintf( stderr , "send a closeack. close flag on.\n");*/
		slot[rfd].close_wait = 1;

		/* close可能だったら閉じる */
		if( checkclosable( rfd ) ){
			closeslot(rfd);
		}
		return 0;
	}

	/* CLOSE_ACKがきたらclose_waitをたてる。 */
	/* CLOSE_ACKも2重送信については大丈夫である。 */
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
   スロットを閉じることができるかどうかをしらべる。
   閉じることができる条件は、
   1 close_wait が 1
   2 読みこみ不可 ( recvqが ぜんぶ -1 )
   3 送信待ちのデータがない ( sendqがぜんぶ -1 )

   今すぐ閉じることができるなら1、そうでないなら0をかえす
   
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
   スロットをとじる
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

