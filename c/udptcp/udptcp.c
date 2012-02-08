#include <stdio.h>

#include "udptcp.h"


/*

   用意するインターフェイス


   ufd fd = usocket( );  ソケット表をひとつ埋める

   ulisten( ufd fd);
   uaccept( ufd fd , unsigned long *cliaddr  );
   uwrite( ufd fd, char *buf , int siz );
   uread( ufd fd , char *buf , int siz );
   uclose( ufd fd );
   uconnect( ufd fd , addr)

   uinit(ul) UDPのソケットを初期化する。
   uproc();     ネットワークIOを使うところ(これを頻繁に呼びだす必要がある。)

   クライアントが接続をしようとして、サーバーがそれ以上多くの接続ができない
   という状態になったら、UDpのパケットをかえして、REFビットをたてる。
  REFビットがたってたら、クライアントのconnectは失敗する。
   connectはSYNビットがたっているデータのないパケットを送信する。


   クライアントがconnectするとSYNをおくる。それをうけとるとSYNを返答するとどうじにread_synをたてる。
   クライアントはその返答をうけとると、read_synを立てる。それがconnectである。
   connectされているあいだは、サーバーもクライアントもread_synがたっているわけだ。
   
   

   1マシンに1コネクションというように固定すると、アドレスのみを識別に
   使えるので、ひじょうにシンプルになる。
   
   
*/


/*   UDPTCPのヘッダー用定数 */

#define SEQ_OFS			0		/* ulong	htonl() 送信用シーケンス番号 */
#define ACK_OFS			4		/* ulong 	htonl() 応答確認番号 */
#define RESET_OFS		8		/* char 再送しても通信を回復できなかった。もうやめじゃ！ コマンド*/
#define SYNCRONIZE_OFS	9		/* char 1が指定されたら、シーケンス番号を0に初期化 */
#define FINISH_OFS		10		/* char 1だったら終了処理して、データを送信しきってしまう。 */
#define REFUSE_OFS		11		/* char 1で接続を拒否するのだ。(backlogがいっぱい)
								   のこりを全部送信できたらFINを指定したパケットを送信する。 */
#define LENGTH_OFS		12		/* htonl() データの長さ */
#define DATA_OFS		16		/* データのはじまる位置 */
#define HEADERSIZE		DATA_OFS/* ヘッダーのサイズ */



struct usock
{
	int use;             
	int listen;       /* listenするかどうか */
	int acceptwait;		/* acceptされるのを待っているなら1。 */
	int read_syn;		/* SYNコマンドを受けとったか */
	
	unsigned long addr_dest;	/* 相手のIP htonlされたあとのデータ */	

	unsigned long ws_top;		/* 何バイト書きこんできたか */
	unsigned long rs_top;		/* 何バイト読みこんできたか */

#define SEGMENTSIZ 512
#define BUFFERSIZ (	SEGMENTSIZ * 8)	

	unsigned long rb_use , wb_use;
	char rb[BUFFERSIZ];		/* 読みこみバッファー。*/
	char wb[BUFFERSIZ];		/* 書きこみバッファー。 */
	char rlog[BUFFERSIZ];	/* 読みこみ状況を記録。バイトの位置が完全に対応しているのだ。 */
	char wlog[BUFFERSIZ];	/* 書きこみ状況を記録 */

#define WLOG_ACKOK		3	/* ACKされたという状況のバイト */
#define WLOG_ACKWAIT	2	/* あとはACKをまつのみという状況のバイト。 */
#define WLOG_SENDWAIT	1	/* 送信されるのを待つのみという状況のバイ */
							/* ト。wbについては、バイトの命は、0->1->2->3 */
							/* と移動する。 */
#define WLOG_NOUSE		0	/* 未使用バッファー */

#define RLOG_ACKOK		1	/* ネットワークから読みこんだ。ACKを送信したということ */
#define RLOG_NOUSE		0	/* 未使用。つまりRLOGでは 0->1と移行する */
	
	
};

#define MAXCON 16
struct usock stb[ MAXCON ];  /* 最大の接続 */

#define MAXACCEPTWAIT	5		/* アクセスがあったがacceptされていない接続のかず  */

#define UDPPORT 9000

#define ACK_NOUSE	0xffffffff	/* ACKフィールドが無効であるという意味。 */

struct udptcp
{
	int maxbacklog;      /* 最大にたまるバックログ */
	int errno;              /* エラー番号 */
	int mainsockfd;				/* UDPのソケット */
	unsigned long ipaddr;		/* 自分のIPアドレス */
	
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
   UDPのソケットを初期化する。

   ipaddrはhtonl後のもの
   
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
   socket表へのインデクスをかえす

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
   何番のバーチャルポートで待つのかを指定するサーバー用。
   
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
	listenする

   int fd : socket表へのindex

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
   usockでアクセプト

   int fd : socket表へのindex
   unsigned long cliaddr : クライアントのIPアドレス

   fdはlistenしていなければならない。
   

   return value:
   >= 0 OK
   <0 Error
   

   ソケット表から検索して、acceptwaitの状態になっているのをさがす

   cliaddr は htonl後、 
   
   
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
   UDPTCPソケットをとじる(FINを送信)
   
*/
int uclose( int fd )
{
	
}

/*



   addrの値は、 htonl後の値である。


   return values:
   0 OK
   <0 Error

   uprocは、何かパケットがきたら返り値でしらせるので、
   uprocが何かうけとるたびにテストして、ackがきたらconnect成功、
   というようにしなければならない。ブロックする。
   
*/
int uconnect( int fd , unsigned long addr )
{

	int sr;
	char tmpdata[HEADERSIZE];

	CHECKFD(fd);

	/* 自分の設定をする */
	stb[fd].addr_dest = addr;
	
	/* connect用のパケットを送信。ヘッダのみ */
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
		/* errnoはセットされてる */
		return -1;
	}

	/* ブロックして、返事がくるまで待つ */
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

   writeする。fdのバッファにたまるだけなので、このあとuprocしなければならない。

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

	/* サイズが変 */
	if(  len < 0 ){
		udptcp.errno = EBADDATALEN;
		return -1;
	}


	/* ソケットのバッファにたす。単にアペンドするのみ */
	if( (stb[fd].wb_use + len) >= BUFFERSIZ){
		/* BUFFERSIZをこえてるぞ */
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
   readする。fdのバッファにたまっている分を読みこむだけなので、uprocしたあとでないと何も
   起こらない。
 */
int uread( int fd, char *data , int len )
{

	int i ,k;
	int read_total = 0;
	int rs_end;
	int completesiz;

	CHECKFD(fd);

	/* ストリームバッファのどこまでが完成したのかを計算する */
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
 	ソケットが読みこみ可能かどうかしらべる

   return values:
   0 Not readable
   1 OK
   -1 エラー
   
   
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

	/* トップの位置と終わりの位置が同じでないとき、読みこみ可能 */
	return !( ( getrsend(fd) - stb[fd].rs_top ) ==0 );

}


/*
   頻繁に呼びだされる、I/Oする関数。パケットの内容を見て、

   return values:

   いくつのパケットを受信したか。
   
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

		/* 書きこみ可能 */
		if( FD_ISSET( udptcp.mainsockfd , &wfd )){
			int i,sr;
			unsigned long start,end,datalen;
			char buf[SEGMENTSIZ+HEADERSIZE];

			for(i=0;i<MAXCON;i++){
				/* start からendまで書きこみたい。長さは適切になってい */
				/* る。 */

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

					/* 送信 */
					sr = sender( buf , datalen +HEADERSIZE , stb[i].addr_dest );
					if( sr != ( datalen+HEADERSIZE) ){
						break;
					} else {
						/* ちゃんと送れたからバッファを更新 */
						setsend( i, start , end );
					}
				}
			}

			write_more = 0;		/* 今はいつでもwrite_more = 0 PENDING */
		}

		/* 読みこみ可能 */
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
				
				/* パケットを読みこむことができた。 ackフィールドにデータがある場合はackの処理をする。
				 データが0xffffffffの場合はデータである。*/
				int index;
			   	unsigned long seq , ack;
			   	unsigned char rst,syn,fin,ref;
			   	unsigned long length;
				getheader( buf , &seq , &ack , &rst , &syn , &fin , &ref ,&length );

				
				/* 対応するindexをポートの情報から得る。ないときは */
				/* accept待ち行列に追加する。 */
				index = getindexbyaddr( sin.sin_addr.s_addr );
			
				if( syn ){
					/* SYNである。 */
					if( index == -1 ){
						fprintf( stderr, "SYN!\n");
						/* 今までにないアドレスだ(サーバーにとっては当然)。
						   accept待ちソケットをついか */
						for(i=3;i<MAXCON;i++){
							if( stb[i].use == 0 ){
								bzero( &stb[i] , sizeof( stb[0] ) );
								newsock(i);
								stb[i].acceptwait = 1;
								stb[i].read_syn = 1;	/* synがきたから */
								stb[i].addr_dest = sin.sin_addr.s_addr;
								break;
							}
						}
						if( i == MAXCON ){
							/* これ以上何もできないからパケットを捨てる。
							   クライアントからはサーバーが応答しないように見える
							   から、タイムアウトすることだろう。*/
						} else {
							/* SYNをかえす */
							makeheader( buf , 0 , 0, 0 , 1 , 0, 0 ,0 );/*ACK->0*/
							sender( buf , HEADERSIZE , sin.sin_addr.s_addr );		   
						}
						
						
					} else {
						/* 今までに存在していたソケットにSYNがきたら、 */
						/* それはクライアントでは当然である */
						if( ack == ACK_NOUSE ){
							/* ACK_NOUSEはデータ送信のときにつかうのでSYNは2回こない.
							 そんな誤ったデータはすてる*/
							fprintf( stderr , "Bad SYN code\n");
						} else {
							/* SYNのACKであるからsenderしない */
							stb[index].read_syn = 1;
							fprintf( stderr , "fd%d read_syn on!\n",index );
						}
					}
				} else if( ack == ACK_NOUSE ){
					/* データである。*/
					int i;
					if( index == -1 ){
						/* 存在しないソケットにたいしてデータがくることはない。 */
						fprintf( stderr, "Bad Data\n" );
					} else {
						/* すでに確立しているコネクションのためのデータだから、readbufにくわえる。 */
						int ret = addrb( index , buf + HEADERSIZE ,seq , length );
						if( ret == 0 ){
							/* 読みこみ成功したからACKかえす。
							 長さもそのまま返す。*/
							makeheader( buf , 0xffffffff , seq , 0 , 0, 0 ,0 ,length );
							sender( buf , HEADERSIZE , sin.sin_addr.s_addr );
						} else {
							/* 失敗したからACKかえさない(再送させんと) */
						}
					}
				} else {
					/* ACKである. */
					/* ACKをうけとったら、 ACKの値をもつwritebufがあるか検索 */
					if( index == -1 ){
						/* 存在しないソケットにACKきても意味なし。無視する */
						fprintf( stderr , "Bad ack code %\n" );
					} else{
						/* そのまま返ってきた長さがここに生かされる。
						   ACKパケットの場合だけ、length分のデータをもたない */
						setack( index , ack, length );
					}
				}
			}
			rcount ++;
			fprintf( stderr, "R ");
		}

		/* もう読みこむものがなくなって、書きこむものもなくなったら返す */
		if( read_ok == 0 && write_more == 0 ){
/*			fprintf( stderr , "%d %d\n" , read_ok , write_more );*/
			break;
		}
	}

	return rcount;
}



/*******************************************************************************/
/*
   read読みこんだときのしょり

   読みこみログをRLOG_ACKOKにする
   成功したら0をかえす。

   startからlen分だけ.
   
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
   ackがきたときの処理
*/
static int setack( int fd , unsigned long ack_start , unsigned long ack_length )
{
	int i;
	int ai;
	
	CHECKFD( fd );

	ai = ack_start - stb[fd].ws_top;
	if( ai < 0 || ai >= BUFFERSIZ ){
		/* そりゃおかしいなり。*/
		return -1;
	}
	if( (ai+ack_length) >= BUFFERSIZ ){
		return -1;
	}
	
	for(i=ai;i<ai+ack_length;i++ ){
		stb[fd].wlog[i] = WLOG_ACKOK;
	}

	/* aiが0のときは、バッファーの最初の部分をACKできたということだから、
	   シフトしてよい。 */
	if( ai == 0 ){
		stb[fd].wb_use -= ack_length;
		shift( stb[fd].wb , ack_length ,BUFFERSIZ );
		shift( stb[fd].wlog , ack_length ,BUFFERSIZ );
		memset( stb[fd].wlog + ack_length , WLOG_NOUSE , BUFFERSIZ - ack_length );
	}
	
	return 0;
}

/*
   indexをアドレスから得る
 */
static int getindexbyaddr( unsigned long addr )
{
	int i;
	int index = -1;		
	for(i=0;i<MAXCON;i++){
		/* 送り手のIPがおなじ */
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
   あたらしいソケットを予約
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
   書きこんだときの処理
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
   あるfdを指定して、ネットワークに書きこみたいデータの次のかたまりの
   位置を教える。タイムアウトの処理もここでできる。
   書きこみたい次のかたまりとは、WLOG_SENDWAITの状態の場所のことである。

   それに長さの制限をSEGMENTSIZでかけるので、
   

   
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
		if( find == 1 && stb[fd].wlog[i] != WLOG_SENDWAIT ){	/* WLOG_NOUSEだろう */
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
   受信ログをみて、ストリームがどこまで完成しているのかをしらべる
   かならず有効な値をかえす。
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
   ソケットが初期化されてるかどうかしらべる

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
   ipaddr: htonl後のデータ


   これはできた。
   
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
   doubleのgettimeofday
   
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
