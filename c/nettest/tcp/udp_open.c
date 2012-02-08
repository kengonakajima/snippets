/*
	「UNIX ネットワークプログラミング 」に載っていた、UDPを使うための
	ユーティリティプログラム。これでソケットを開くことができる

*/

#include "netdefs.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef INADDR_NONE
#define INADDR_NONE	0xffffffff
#endif

/*
	次の大域変数は、必要であれば呼び出し側で参照することができる。
*/
struct sockaddr_in	udp_srv_addr;	/* サーバのInternetソケットアドレス */
struct sockaddr_in	udp_cli_addr;	/* クライアントの ・・・ */
struct servent		udp_serv_info;	/* getservbyname() の戻り値 */
struct hostent		udp_host_info;  /* gethostbyname() の戻り値 */



/*
   関数 udp_open
   host ...  交信する他システムの名前
   service ... 要求されているサービスの名前,port>0の時だけ、NULLでも構わない
   port ...  ==0なら、serviceで定義されているポートを用いる
    	     <0 なら、ローカルな予約済みポートをバインドする
	     >0 なら、その値をサーバのポート番号として用いる( ホストバイト順序)
   dontconn ... 0のときだけ、connectを呼びだす。

*/
int udp_open( char *host , char *service , int port , int dontconn )
{
	int fd ;
	unsigned long inaddr;
	char *host_err_str();
	struct servent *sp;
	struct hostent *hp;

	/* サーバのInternetアドレスを初期化する。ポートも含む。 */

	bzero(( char *) &udp_srv_addr , sizeof( udp_srv_addr ));
	udp_srv_addr.sin_family = AF_INET;

	if( service != NULL ){
		if( ( sp = getservbyname( service , "udp")) == NULL ){
			perror("udp_open:unknown service: %s/udp" , service );
			return (-1);
		}
		udp_serv_info = *sp;
		if( port > 0 )
			udp_srv_addr.sin_port = htons( port );
		else
			udp_srv_addr.sin_port = sp->s_port;
	} else {
		if( port <= 0 ){
			perror("udp_open: must specify either service or port " );
			return -1;
		}
		udp_srv_addr.sin_port = htons( port );
	}

	/* 最初は、 192.168.54.48 のような形式であるとして変換する。失敗したら、名前とする */

	if( ( inaddr = inet_addr( host  )) != INADDR_NONE ){
		bcopy(( char * ) &inaddr , (char *) &udp_srv_addr.sin_addr , sizeof( inaddr ));
		udp_host_info.h_name = NULL;
	} else {
		if( ( hp = gethostbyname(host ) ) == NULL ){
			perror("udp_open:host name error: %s %s " , host , host_err_str());
			return -1;
		}
		udp_host_info = *hp;
		bcopy( hp->h_addr , (char * ) &udp_srv_addr.sin_addr , hp->h_length );
	}

	if( port < 0 ){
		perror("udp_open: reserved ports not implemented yet");
		exit(-1);
	}

	if( ( fd = socket( AF_INET , SOCK_DGRAM ,  0 ) ) < 0 ){
		perror("udp_open: can't create UDP socket.");
		return (-1);
	}

	/* 適当なローカルアドレスをバインドする */

	bzero( ( char *) &udp_cli_addr , sizeof( udp_cli_addr ) );
	udp_cli_addr.sin_family 	= AF_INET;
	udp_cli_addr.sin_addr.s_addr 	= htonl(INADDR_ANY );
	udp_cli_addr.sin_port 		= htons( 0 );
	if( bind( fd , (struct sockaddr * ) &udp_cli_addr ,sizeof( udp_cli_addr )) < 0 ){
		perror("udp_open: bind error." );
		close(fd );
		return -1;
	}
	
	/* 必要なら、connectを呼ぶ。相手が変わることはないので、通常は呼ぶ。
	   これを呼んでおくと、呼び出し側では、 send() と recv を使えるようになる。
	*/

	if( dontconn == 0 ){
		if( connect( fd , (struct sockaddr *) &udp_srv_addr , sizeof(udp_srv_addr ))< 0 ){
			perror(" udp_open: connect error.");
			return -1;
		}
	}
	return fd;
}

