#include <stdio.h>

#include "udptcp.h"


/*

   用意するインターフェイス


   ufd fd = usocket( );  ソケット表をひとつ埋める

   ulisten( ufd fd);
   uaccept( ufd fd , unsigned long *cliaddr , unsigned short *port );
   uwrite( ufd fd, char *buf , int siz );
   uread( ufd fd , char *buf , int siz );
   uclose( ufd fd );
   uconnect( ufd fd , addr,port)

   uinit(ul) UDPのソケットを初期化する。
   uproc();     ネットワークIOを使うところ(これを頻繁に呼びだす必要がある。)

   クライアントが接続をしようとして、サーバーがそれ以上多くの接続ができない
   という状態になったら、UDpのパケットをかえして、REFビットをたてる。
  REFビットがたってたら、クライアントのconnectは失敗する。
   connectはSYNビットがたっているデータのないパケットを送信する。


   クライアントがconnectするとSYNをおくる。それをうけとるとSYNを返答するとどうじにread_synをたてる。
   クライアントはその返答をうけとると、read_synを立てる。それがconnectである。
   connectされているあいだは、サーバーもクライアントもread_synがたっているわけだ。
   
   


   
*/


/*   UDPTCPのヘッダー用定数 */
#define SRC_PORT_OFS 	0		/* unsigned short htons() 送信元ポート */
#define DEST_PORT_OFS 	2		/* unsigned short htons() 宛先ポート */
#define SEQ_OFS			4		/* ulong	htonl() 送信用シーケンス番号 */
#define ACK_OFS			8		/* ulong 	htonl() 応答確認番号 */
#define RESET_OFS		12		/* char 再送しても通信を回復できなかった。もうやめじゃ！ コマンド*/
#define SYNCRONIZE_OFS	13		/* char 1が指定されたら、シーケンス番号を0に初期化 */
#define FINISH_OFS		14		/* char 1だったら終了処理して、データを送信しきってしまう。 */
#define REFUSE_OFS		15		/* char 1で接続を拒否するのだ。(backlogがいっぱい)
								   のこりを全部送信できたらFINを指定したパケットを送信する。 */
#define LENGTH_OFS		16		/* htonl() データの長さ */
#define DATA_OFS		20		/* データのはじまる位置 */
#define HEADERSIZE		DATA_OFS/* ヘッダーのサイズ */



struct usock
{
	int use;             
	int listen;       /* listenするかどうか */
	int acceptwait;		/* acceptされるのを待っているなら1。 */
	int read_syn;		/* SYNコマンドを受けとったか */
	
	unsigned long addr_dest;	/* 相手のIP htonlされたあとのデータ */	
	unsigned short port_dest;	/* 相手のポート番号(index) ,htonsされたあとのでーた*/

	unsigned long read_addr;		/* 何バイト書きこめたか */
	unsigned long write_addr;	

#define WINDOWCOUNT 8
#define ELEMENTSIZE 512			/* バッファーエレメントのsize*/
	
	int readbuflen[WINDOWCOUNT];		/* readbuf/writebufを使ってるかどうか */
	int writebuflen[WINDOWCOUNT];		
	unsigned long readbufaddr[WINDOWCOUNT];		/* read/writeバッファが何バイト目から用のデータなのかを記憶 */
	unsigned long writebufaddr[WINDOWCOUNT];	
	char readbuf[WINDOWCOUNT][ELEMENTSIZE];		/* 読みこみバッファー。*/
	char writebuf[WINDOWCOUNT][ELEMENTSIZE];		/* 書きこみバッファー。 */
	unsigned long writebuf_sent[WINDOWCOUNT];		/* 書きこんだらこのフラグをたて、ackまちをする */
	struct timeval writebuf_sent_when[WINDOWCOUNT];	/* それぞれの書きこみバッファで、いつ送信したか */
};

#define MAXCON 16
struct usock socktable[ MAXCON ];  /* 最大の接続 */

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
	i= bind( udptcp.mainsockfd , (struct sockaddr*)& sin , sizeof( sin ));

	fprintf( stderr, "bind: %d %x\n", i , ipaddr );
	for(i=0;i<MAXCON;i++){
		bzero( &socktable[i] , sizeof( socktable[0]) );
	}
	
	udptcp.ipaddr = ipaddr;
	
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
			int i;

			/* ソケットにデータがたまってたら書きこむ。そしてまだデー */
			/* タがあるようだったらwrite_moreを1にする */

			for(i=0;i<MAXCON;i++){
				int j;
				for(j = 0;j<WINDOWCOUNT;j++){
					if( socktable[i].writebuflen[j] > 0 ){
						int sr;	
						char tmpbuf[ELEMENTSIZE+HEADERSIZE];
						fprintf( stderr, "W \n");
						makeheader( tmpbuf ,
								   i , 	/* indexがポートになるのだ */
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
							/* ただしく書きこめたが、再送のためがあるからデータは消せない。 */


							socktable[i].writebuf_sent[j] = 1;
							gettimeofday( & socktable[i].writebuf_sent_when[j] , NULL );
#if 1
							/* 再送しない場合はここでデータをけしてよい。(lengthを0にするだけ) */
							socktable[i].writebuf_sent[j]= 0;
							socktable[i].writebuflen[j] = 0;
#endif
							
							break;
						}
					}
				}
			}
			write_more = 0;		/* 今はいつでもwrite_more = 0 PENDING */
		}

		/* 読みこみ可能 */
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
				
				/* パケットを読みこむことができた。 ackフィールドにデータがある場合はackの処理をする。
				 データが0xffffffffの場合はデータである。*/
				int index;
			   	unsigned short sport,dport;
			   	unsigned long seq , ack;
			   	unsigned char rst,syn,fin,ref;
			   	unsigned long length;
				getheader( buf , &sport , &dport , &seq , &ack , &rst , &syn , &fin , &ref ,&length );

				
				/* 対応するindexをポートの情報から得る。ないときはaccept待ち行列に追加する。 */
				index = -1;		
				for(i=0;i<MAXCON;i++){
					/* 送り手のIPがおなじでかつ、UDPTCPポートもおなじ */
/*					fprintf( stderr , "STbl[%d] %d: addr:%x port:%d cliaddr:%x sport:%d\n" ,
							i, socktable[i].use,
							socktable[i].addr_dest ,
							socktable[i].port_dest , sin.sin_addr.s_addr , sport );*/
					if( socktable[i].use &&
					    socktable[i].addr_dest == sin.sin_addr.s_addr &&
					    socktable[i].port_dest == dport ){
						/* dportが元のこちらのポートでsportがむこうで */
						/* わりあてられたあたらしいポートである */
						index = i;
						break;
					}
					
				}
				fprintf( stderr , "INDEX:%d sp:%d dp:%d\n" , index ,sport , dport );
			
				if( syn ){
					/* SYNである。 */
					if( index == -1 ){
						fprintf( stderr, "SYN!\n");
						/* 今までにないアドレスだ(サーバーにとっては当然)。accept待ちソケットをついか */
						for(i=3;i<MAXCON;i++){
							if( socktable[i].use == 0 ){
								bzero( &socktable[i] , sizeof( socktable[0] ) );
								socktable[i].use = 1;
								socktable[i].listen = 0;
								socktable[i].acceptwait = 1;
								socktable[i].read_syn = 1;	/* synがきたから */
								socktable[i].addr_dest = sin.sin_addr.s_addr;
								socktable[i].port_dest = sport;	/* 送り手は、sportにポート番号(index)を入れて送信してくるのだ */
								socktable[i].read_addr = 0;
								socktable[i].write_addr = 0;
								
								break;
							}
						}
						if( i == MAXCON ){
							/* これ以上何もできないからパケットを捨てる。
							   クライアントからはサーバーが応答しないように見える
							   から、タイムアウトすることだろう。*/
						} else {
							/* SYNをかえす */
							makeheader( buf , i, sport , 0 , 0, 0 , 1 , 0, 0 ,0 );
							sender( buf , HEADERSIZE , sin.sin_addr.s_addr );		   
						}
						
						
					} else {
						/* 今までに存在していたソケットにSYNがきたら、 */
						/* それはクライアントでは当然である */
						if( ack == ACK_NOUSE ){
							/* ACK_NOUSEはデータ送信のときにつかうのでSYNは2回こない.
							 そんな誤ったデータはすてる*/
							fprintf( stderr , "AA\n");
						} else {
							/* SYNのACKであるからsenderしない */
							socktable[index].read_syn = 1;
							fprintf( stderr , "fd%d read_syn on!\n",index );
							fprintf( stderr , "SS\n");
						}
					}
				} else if( ack == ACK_NOUSE ){
					/* データである。*/
					int i;
					if( index == -1 ){
						/* 存在しないソケットにたいしてデータがくることはない。 */
					} else {
						/* すでに確立しているコネクションのためのデータだから、readbufにくわえる。 */
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
						/* ウインドウがまんタンで受けとることができなかった。
						 だからACKはかえさず、データをすてる */
					} else {
						/* データ用ACKをかえす */
						makeheader( buf , index , sport , 0xffffffff ,
								   seq /* seqをACKに入れて返す */ , 0 , 0, 0 ,0 ,0 );
						sender( buf , HEADERSIZE , sin.sin_addr.s_addr );
					}

				} else {
					/* ACKである. */
					/* ACKをうけとったら、 ACKの値をもつwritebufがあるか検索して、あったらそのバッファーをけす。 */
					if( index == -1 ){
						/* 存在しないソケットにACKきても意味なし。無視する */
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

		/* もう読みこむものがなくなって、書きこむものもなくなったら返す */
		if( read_ok == 0 && write_more == 0 ){
/*			fprintf( stderr , "%d %d\n" , read_ok , write_more );*/
			break;
		}
	}

	return rcount;
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
   何番のバーチャルポートで待つのかを指定するサーバー用。
   
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
	listenする

   int fd : socket表へのindex

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
   usockでアクセプト

   int fd : socket表へのindex
   unsigned long cliaddr : クライアントのIPアドレス
   unsigned short port : クライアントのUDPTCP ポート


   fdはlistenしていなければならない。
   

   return value:
   >= 0 OK
   <0 Error
   

   ソケット表から検索して、acceptwaitの状態になっているのをさがす

   cliaddr は htonl後、 cliport はhtonl後のデータになっている。
   
   
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
   UDPTCPソケットをとじる(FINを送信)
   
*/
int uclose( int fd )
{
	
}

/*

   接続する。実際のUDP用のアドレスは、 (addr , htons( port))であ
   る。

   addrの値は、 htonl後の値である。
   portもそう

   return values:
   0 OK
   <0 Error

   uprocは、何かパケットがきたら返り値でしらせるので、
   uprocが何かうけとるたびにテストして、ackがきたらconnect成功、
   というようにしなければならない。ブロックする。
   
*/
int uconnect( int fd , unsigned long addr , unsigned short port )
{

	int sr;
	char tmpdata[ELEMENTSIZE];
	
	if( fd < 0 || fd >= MAXCON || socktable[fd].use ==0){
		udptcp.errno = EBADFDNUM;
		return -1;
	}

	/* 自分の設定をする */
	socktable[fd].addr_dest = addr;
/*	socktable[fd].port_dest = port;  ポートは受信側が自動でわりふる*/
	
	/* connect用のパケットを送信。ヘッダのみ */
	makeheader( tmpdata, 
			   fd ,
			   ntohs(port) ,	/* ここがイヤ */
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
   readする。fdのバッファにたまっている分を読みこむだけなので、uprocしたあとでないと何も
   起こらない。
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

	/* WINDOWを全部検索して、read_addrと同じaddrをもつものがあったらそれから
	   返していく。 readがおわったら、バッファーをけしてしまう。ので、かならず、
	   read_addrよりも進んだものしか残らない */

	fprintf( stderr, "UREAD: fd:%d radr:%d\n",	fd , socktable[fd].read_addr );
	found_once = 0;
	while(1){
		found = 0;
		for(k=0;k<WINDOWCOUNT;k++){
			if( socktable[fd].readbufaddr[k] == socktable[fd].read_addr &&
			    socktable[fd].readbuflen[k] > 0
			   ){
				
				/* dataの最大をチェック。越えてたら一部をコピー(len-read_total byteだけ)
				 してバッファをいじる */
				if( read_total >= len ){
					break;
				}
				if( (read_total +socktable[fd].readbuflen[k]) >= len ){
					int copy_size = len - read_total;
					int a;
					fprintf( stderr , "copysiz:%d rtot:%d len:%d\n",copy_size , read_total ,len);
					bcopy( socktable[fd].readbuf[k] , data + read_total , copy_size );
					/*バッファをずらして長さを変更 */
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
				/* コピー */
				fprintf( stderr, "COPPPPPPPPPY\n");
				bcopy( socktable[fd].readbuf[k] , data + read_total , socktable[fd].readbuflen[k] );
				socktable[fd].read_addr += socktable[fd].readbuflen[k];
				
				read_total += socktable[fd].readbuflen[k];
				found = 1;
				found_once = 1;

				/* バッファーをけす */
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

   writeする。fdのバッファにたまるだけなので、このあとuprocしなければならない。

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


	/* サイズが変 */
	if(  len < 0 ){
		udptcp.errno = EBADDATALEN;
		return -1;
	}


	/* ソケットのバッファにたす */

	for(i=0;i<WINDOWCOUNT;i++){
		if( socktable[fd].writebuflen[i] == 0 ){
			/* バッファがあいてた */
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
				/* len と同じだけ書きこめたらbreak */
				break;
			}
		}
	}
	udptcp.errno = 0;
	return write_total;
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
	
	if( fd < 0 || fd >= MAXCON || socktable[fd].use ==0){
		udptcp.errno = EBADFDNUM;
		return 0;
	}

	if( issockinited(fd ) != 1 ){
		udptcp.errno = ENOTINIT;
/*		fprintf( stderr, "AAAA %d\n",issockinited(fd));*/
		return 0;
	}

	/* ureadの中でしらべるのと同じ方法で、readできるものがあるかしらべ */
	/* る */

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
   ソケットが初期化されてるかどうかしらべる

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
   doubleのgettimeofday
   
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
