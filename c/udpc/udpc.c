
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

#define VS_ACCEPTWAIT	100		/* acceptされるのをまってる状態のあたらしい接続 */
#define VS_ALIVE		101		/* acceptされた。使える。 */
#define VS_CONNECTING	102		/* connect処理中である。 */
#define VS_FINWAIT		103		/* finを送信するのをまっている */
#define VS_FINACKWAIT	104		/* finのackをまってる */
	
	unsigned long ipaddr;	/* 相手の ipaddr */
	unsigned short dest_port;	/* 相手の port */
	int vfd_d;			/* 相手のvfd */

	int recv_syn_ack;	/* SYNのACKを受けとったらSYN送信側のこのフラグ */
						/* がたたう。*/
	double fin_time;	/* FINを送信した時間。finの再送のために使う */
	
	int write_addr;		/* 最新のseqの位置。			   */
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
	int sblen[BUFNUM];			/* length of each buffer 0だったら未使用*/
	int rblen[BUFNUM];
	
#define SB_NODATA	0			/* sbの状態 */
#define SB_SENDWAIT	1
#define SB_ACKWAIT	2


#define RB_NODATA	0			/* rbの状態 */
#define RB_RECVOK	1	
	
};

#define MAXCON 128


double connectretry[]={ 4 , 6 , 8 , 12, 18 ,26 , 38 };	/* connectのリトライの間隔(秒) */


struct vsock sock[MAXCON];


#define RST_BIT 0x01
#define SYN_BIT 0x02
#define FIN_BIT	0x04
#define REF_BIT	0x08

#define SEQ_OFS 0	/* int */
#define ACK_OFS 4	/* int ここが-1だったらACKではなく、-1以外だったらACKパケットだ*/
#define LENGTH_OFS 8	/* int */
#define CODE_OFS 12		/* char 3バイトはうめうめ*/
#define SVFD_OFS 16		/* int 送り手のvfd*/
#define RVFD_OFS 20		/* int 受け手のvfd*/
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

   closeするには、FINを送信するのをまつFINWAITの状態になる。
   FINを送信すると同時にFINACKWAITな状態になる。
   FINをうけとった側は即消滅する。送信がわはFINACKをうけとったら即消滅する。
   

   
 */
int UDPCClose( int vfd )
{
	VFDCHECK( vfd );

	sock[vfd].state = VS_FINWAIT;
	
}

/*
   読みこみ

   
 */
int UDPCRead( int vfd , char *buf , int len )
{
	int totallen = 0;
	int i,j;


	VFDCHECK( vfd );
	
	for(i=0;i<BUFNUM;i++){
		int least_seq;
		
		/* 最も小さいrbseqをもとめる */

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
						/* rblenだけappendできた。バッファをけしてしまう */
						sock[vfd].rbstate[j] = RB_NODATA;
						sock[vfd].rbseq[j] = 0;
						sock[vfd].rblen[j] -= appended;
					} else {
						/* lenだけappendできた。バッファはまだなくならない */
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
			/* 今はreadできるバッファない */
			break;
		}
	}
	
	return totallen;
	
}
/*
   バッファアペンド

   実際にappendしたバイト数をかえす

   src : もと
   srclen : もとのバッファのながさ
   dest : さき
   destofs : さきのおふせっと
   destlen : さきのさいだいながさ

   
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
   読みこみすることができる最小のアドレスのもののseqをかえす

   int vfd

   return
   -1 : そんなseqはない(読みこみ不可)
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
   書きこみ

   int vfd : vfd
   
   char *buf : 送信するバッファ
   int len: 送信する最大長さ

   実際に書きこんだ長さを返す。負はエラー
	0もありうるぞ
   
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
		/* totallen からはじまる copylenをコピーする */
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
   sbの空きをさがす

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
   UDPCのネットワーク処理
   
 */
void UDPC(void)
{
	int read_ok = 0;
	int write_ok =0;
	int send_more = 0;
	int sent_one = 0;		/* 1個でもパケットを送信すると1 */
	
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
		
		/* かきこみ可能 */
		if( FD_ISSET( udpc.mainsockfd , &wfd )){
			static int send_first = 0;

			
			write_ok = 1;
			
			/* SENDWAITを送信してACKWAITの状態にする */
			/* パケットひとつ送信 */
			

			for(i=0;i<MAXCON;i++){
				int j;
				char buf[BUFLEN + HEADERSIZ];
				
				send_first++;
				if( send_first == MAXCON )send_first =0;


				/* FIN送信。再送はまだなし */
				if( sock[send_first].state == VS_FINWAIT ){
					makeheader( buf , 0,-1,  0,0,1,0, 0 , send_first , sock[send_first].vfd_d );
					if(sender( buf,HEADERSIZ , sock[send_first].ipaddr,sock[send_first].dest_port )
					   == HEADERSIZ ){
						sock[send_first].state = VS_FINACKWAIT;
						sock[send_first].fin_time = getUTimeDouble();
						break;
					}
				}
				
				/* データ送信 */
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
							
							/* まだ書きこむものがあるぞー */
							send_more = 1;
							break;
						}
					}
				}
				if( send_more == 1 )break;
			}
				
		}

		/* 読みこみ可能 */
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
				
				

				/* ACKかそうでないかで大きくわかれる */
				
				if( ack != -1 ){		/* ACKである */
					if( syn ){
						/* Connectのackだ */
						if( svfd >=0 && svfd < MAXCON  && sock[svfd].use &&
						   sock[svfd].state == VS_CONNECTING ){
							sock[svfd].recv_syn_ack = 1;
						}
					}
					if( syn==0 && rst==0 && fin==0 && ref==0 ){
						/* データパケットのACKである */
						receiveack( svfd,ack );
					}
				} else {			/* ACKでない */
					if( syn ){			
						/* Connectしてきた。ACKかえす */
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
						/* データパケットだ */
						int r;
						r = testnewsock( sin.sin_addr.s_addr, sin.sin_port ,svfd );
						if( r >= 0 ){
							/* そんなパケットはない！ */
						} else {
							int r;
							r = receivepacket( sin.sin_addr.s_addr , rvfd,seq,buf+HEADERSIZ,len );
							if( r == 0 ){
								/* きちんとパケット受けとり処理ができたので、ACKをかえす */
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
   UDPCを初期化する

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
   ACKを受信したときの処理

   int svfd : 送信したfd
   int ack : ack ( 送信するときのseq)

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
			/* ACKがかえってきたからけしてしまう */
			sock[svfd].sbstate[i] = SB_NODATA;
			sock[svfd].sblen[i]=0;
			fprintf( stderr , "Ack ok! [%d] %d\n", i , ack );
			return 0;
		}
	}
	return -1;
}


/*
   Connectする。

   Connectできたらvfdをかえす。

   ipaddr : htonl後のデータ
   port : htons後のデータ
   
   -1 : 表にあきがない。
   -3 : サーバーが返事してない
   
*/
int UDPCConnect( unsigned long ipaddr , unsigned short port )
{
	int i;
	int vfd;
	char buffer[BUFLEN + HEADERSIZ];

	/* 最初に、空きをさがす */
	for(i=3;i<MAXCON;i++){
		if( sock[i].use == 0 ){
			vfd = i;
			break;
		}
	}
	if( i == MAXCON ){
		return -1;
	}

	/* コネクト中であるという居み */
	sock[vfd].state = VS_CONNECTING;
	sock[vfd].use = 1;
	sock[vfd].write_addr = 0;
	sock[vfd].read_addr = 0;
	sock[vfd].ipaddr = ipaddr;
	sock[vfd].dest_port = port;
	
	/* SYNのACKがかえってくるのを期待してSYNを送信 */
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
						return -3;/* リトライしつくした */
					}
					sender( buffer , HEADERSIZ , ipaddr , port );
					break;
				}
			}
		}

		UDPC();

		/* ここでrecv_syn_ackがたってたら返事をうけとったということに */
		/* なる */
		if( sock[vfd].recv_syn_ack ){
			sock[vfd].state = VS_ALIVE;
			fprintf( stderr , "return vfd\n");
			return vfd;
		}
	}	
	return -3;
}

/*
   Acceptする。

   return values:
   -1 acceptしんかった。
   >=0 acceptした。vfd
   
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
   IP , sender_vfdの組が新規かどうかしらべる
   return values:
   >=0 新規であるので、accept待ちをとうろくし、とうろくしたvfdをかえす。
   -1 新規だがエラーのため新規登録できなかった。REFをかえせ。
   -2 新規だがbacklogの最大数を越えてるのでacceptに登録できなかった。
   REFをかえせ。
   -8 新規でないので何もしなかった。
   
   新規だったらaccept待ちとして登録する。
   
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

	
	/* 新規だった */
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
   ひとつのvfdにパケットを受けとった。

   return values:
   0 OK ( ACKを返してよい )
   <0 Error ( エラーなのでACKは返してはならない)

   buf , lenにかんしては信用する。
   
 */
static int receivepacket( unsigned long ip , int vfd , int seq , char *buf , int len )
{
	int i;
	
	VFDCHECK( vfd );

	/* 同じvfdに違うアドレスからやってきた。これはブブー */
	if( sock[vfd].ipaddr != ip ){
		return -1;
	}
	
	for(i=0;i<BUFNUM;i++){
		if( sock[vfd].rblen[i] == 0 ){
			/* あいてるrbをみつけた */

			
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
	
	/* 読みこみバッファがいっぱいである */
	return -1;
	
}


/*
   ヘッダをつくる
   
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
   ipaddr: htonl後のデータ
   port : htons後のデータ
   
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
