/*


   	TCPの性能テスト


	複数のソケットからの入力を受けつけるためにselectを使う。
	
  	しかし、それでも5つまでの接続しかできない。

	ゲームにおいて送信されるデータの量は、

	スクロールしたときの差分 ( 20 x 2 = 40byteくらい)
	他のキャラの位置など ( 一個6byteとして、 キャラの数分だけ。キャラオーバーをするよう
	に設定するとしたら、だいたい10個までくらいか。 )
	自分の体力などの情報(20byteくらい？)
	テキストデータ( max 100byte? )

	だいたい、200byteくらいを見ておく。

	64kbpsだったら、10で割って6.4Kbyteくらい送れることになる。32人でめいっぱいか。
	まあ、それを越えたら遅くなってもしかたがない。


	いくつもの接続を同時にできるUDPを使いたい。FreeWalkもUDPを使っていることだし。
	UDPの信頼性を高めるために、RTTアルゴリズムを使う。うまく再送要求を出していく。


	P.490
*/

#define _T_C_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>


#define HOST "murasaki"
#define MYECHO_SERVICE "myecho"

#define MAXLINE 255

char *pname;
extern int errno;




int main( int argc , char **argv )
{
	int sockfd ;

	pname =argv[0];

	if( ( sockfd = udp_open( HOST , MYECHO_SERVICE , 0 , 0 ) ) < 0 ){
		perror("udp_open error. " );
	}
	
	doit( stdin , sockfd );

	exit(0);

}

#define MAXMESG 2048

/*
   エコールーチン。
   データグラムクライアントの終了は検出することができないので、この関数は呼び出し
   元に、処理を返さない。
*/

void dg_echo( int sockfd , struct sockaddr *pcli_addr , int maxclilen )
{
	int n , clilen;
	char mesg[MAXMESG];

	for(;; ){
		clilen = maxclilen;
		n = recvfrom( sockfd , mesg , MAXMESG , 0 , pcli_addr, &clilen );
		if( n < 0 ){
			perror("dg_echo: recvfrom error.");
		}
		if( sendto( sockfd , mesg , n , 0 , pcli_addr , clilen ) != n ){
			perror("dg_echo: sendto error." );
		}
	}
	

}


#if 0
void doit(FILE *fp , int sockfd )
{
	int n , sendlen;
	long seqsend;
	char sendline[MAXLINE], recvline[MAXLINE];

	seqsend = 0;
	while( fgets( sendline + sizeof(long ) , MAXLINE - sizeof(long) , fp ) != NULL ){

		seqsend++;	/* 順序番号を増加させる */
		bcopy(( char * ) &seqsend , sendline , sizeof( long ) );
		sendlen = strlen( sendline + sizeof( long )) + sizeof( long );

rexmit:
		if( ( n = dgsendrecv( sockfd , sendline , sendlen , recvline , MAXLINE ,
				     ( struct sockaddr *) 0 , 0 )) < 0 ) {
			if( errno == EINTR )
				perror("client: no response from server" );
			else
				perror("client: dgsendrecv error");
		}

		if( bcmp(( char *) &seqsend , recvline , sizeof(long )) != 0){
			perror("incorrect sequence number received . ");
			goto rexmit;
		}
		recvline[n] = 0;	/* NULL character */
		fputs( recvline + sizeof( long ) , stdout );

	}
	if( ferror(fp ) ){
		perror("client: error reading file" );
	}
		


}



#endif
