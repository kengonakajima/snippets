/*
	LifestormのCONNECTION型をテスト   


*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<string.h>

#define		PORT	(u_short)80
#define		BUF_LEN	1000

char hostname[] = "lifestorm";
char buf[BUF_LEN];
char charlist[] = "CHARLIST type=login start=1 mode=visual\n";

//	select する時のビットマスクの準備用
fd_set mask;
fd_set readok;

void main( void ){
	struct hostent *servhost;
	struct sockaddr_in server;
	struct timeval timeout;

	int s;
	int n;
	char c;

	timeout.tv_sec = 0 ;
	timeout.tv_usec = 0 ;

	//	相手のアドレスを server という構造体にセットする
	servhost = gethostbyname( hostname );
	bzero( (char *)&server, sizeof( server ) );
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	bcopy( servhost->h_addr, (char *)&server.sin_addr, servhost->h_length );

	//	ソケットを獲得する
	s = socket( AF_INET, SOCK_STREAM, 0 );

	//	相手のアドレスを指定して接続の要求
	connect( s, &server, sizeof( server ) );

	//	ソケットディスクプリタのマスク
	FD_ZERO( &mask );
	FD_SET( 0, &mask );
	FD_SET( s, &mask );

	
/*	write( s, "adm\n", sizeof( "adm\n" ) ); */
	write( s, "NeWbu!1D\n", sizeof( "NeWbu!1D\n" ) ); 

	//	メインループ
	while( 1 ){

		//	ソケットを調べる
		//	select の前に毎回 readok を初期化する 
		readok = mask;
		select( s+1, &readok, NULL, NULL, &timeout );

		//	ソケットにデータは来ているか
		if( FD_ISSET( s, &readok ) != 0 ){

			int	ret;
			ret = read( s , &buf , sizeof( buf ) );
			write( 1  , buf , ret );

		}
		//	標準入力にデータは来ているか
		if( FD_ISSET( 0, &readok ) ){
			write( s, "QUIT\n", sizeof( "QUIT\n" ) );		
			break;

		}

		sleep( 2 );
		write( s, charlist, strlen(charlist) );
	}		
	close( s );
}


/********************
  ASCII文字のIPアドレスとポートを指定して、サーバーとの接続を
  得る。

  char *hostname ... ホスト名( ASCII )
  int port ... ポート番号 
  
  return vlaues:
  	失敗すると、負の値が返るぞ成功するとsockfd
	-1 ... gethostbyname error
	-2 ... socket error
	-3 ... connect error
	
********************/
int getServerSocket( char *hostname , int port )
{
	int sockfd ;
	char emsg[1000];
	struct hostent *servhost;
	struct sockaddr_in server;

	servhost = gethostbyname( hostname );
	if( servhost == NULL ){
		sprintf( emsg , "getServerSocket: gethostbyname() failed. host:[%s] errno:[%d]\n" , hostname  , errno );
		return -1;
	}
	bzero( (char*) &server ,sizeof( server ) );
	server.sin_family = AF_INET;
	server.sin_port = htons( port );
	bcopy( servhost->h_addr , (char *)&server.sin_addr , servhost->h_length );
	sockfd = socket( AF_INET , SOCK_STREAM , 0 );
	if( sockfd < 0 ){
		sprintf( emsg , "getServerSocket: socket() failed. errno:[%d]\n" , errno );
		return -2;
	}
	if( connect( sockfd  , &server , sizeof( server ) ) < 0 ){
		sprintf( emsg ,"getServerSocket: connect() failed. errno:[%d] hostname:[%s]\n" , errno , hostname );
		return -3;
	}

	return sockfd;
	
}


/********************
  CONNECTION型のコンストラクタ関数。

  arg[4] ... STRING IPアドレス(lifestorm.titan.co.jp/202.228.85.17両方受けつけるようにしたいPENDING)
  arg[3] ... NUMBER ポート番号
  arg[2] ... ソケットオプション
  arg[1] ... reserved
  arg[0] ... reserved
********************/
int sys_CONNECTION( char **arg , char **ret )
{
	int sockfd;

	sockfd = getServerSocket( arg[4] , getNumberFromNUMBERType( arg[3] ) );
	if( sockfd < 0 ){
		sprintf( ret[0] , "error ( getServerSocket returns:%d errno:%d )" , sockfd , errno );
		return -1;
	}

#if 0
	constructCONNECTIONType(
#endif

	
	return 0;
}



int main( int argc ,char **argv )
{

	
}

