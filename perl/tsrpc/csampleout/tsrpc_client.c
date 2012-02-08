/*

  このコードもrpcgenが吐き出す。

  関数の内容を定義しているのだ。







*/

#define _TSRPC_CLIENT_C_

#include "tsrpc.h"
#include "tsrpc_client.h"      /* これは関数のプロトタイプが入って言る。 */




int remotesum2( CLIENT *ch, char *arg1 , int arg2[10] )
{

/* メモリ確保フェーズ */
	char *buf1 = (char*)malloc( MKSTR_MINSIZE );
	char *retbuf = (char*)malloc( MKSTR_MINSIZE );
	char *callbuf = (char*)malloc( MKSTR_MINSIZE );


/* 文字列作成 */
	callbuf = joinString( callbuf , "remotesum2" );
	callbuf = joinString( callbuf , mkstr_string( arg1 ) );
	callbuf = joinString( callbuf , buf1 = mkstr_int32_array( buf1 , 10 , arg2 ) );
	callbuf = joinString( callbuf , "\n" );

/* 呼びだしフェース */
	call_tsrpctcp( ch ,callbuf , retbuf );

/* メモリ解放フェーズ */
	free( (char*)retbuf );
	free((char*) buf1 );
	free((char*) callbuf );

/* 返り値変換 */

	return demkstr_int32(retbuf);
}









/************

  クライアントを初期化する

*************/
CLIENT * tsrpcCreateClient( char *hostname , int port )
{
	struct sockaddr_in sin;
	struct hostent *h;
	struct in_addr ina;

    /* メモリ確保 */
	CLIENT *ch = (CLIENT * ) malloc( sizeof( CLIENT ) );
	if( ch == NULL ){
		fprintf( stderr ,"cannot allocate CLIENT struct.\n" );
		return ch;	
	}

	/* 全部ゼロにする */
	bzero( ch , sizeof( CLIENT ) );

    /* 変数初期化 */
	ch->port = port;
	strcpy( ch->hostname , hostname );



	/* アドレスを知る */
	h = gethostbyname( hostname );
	if( h == NULL ){
		fprintf( stderr , "cannot gethostbyname %s.\n" , hostname );
		return NULL;
	}
	bcopy( h->h_addr_list[0] , &ina.s_addr , 4 );
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr = ina;

    /* TCP/UDP ソケット作成 */


	ch->tcp_sockfd = socket( AF_INET , SOCK_STREAM , 0 );
	if( ch->tcp_sockfd < 0 ){ 
		fprintf( stderr, "tcp socket() error.errno:%d\n" , errno );
		return NULL;
	}
	ch->udp_sockfd = socket( AF_INET , SOCK_DGRAM , 0 );
	if( ch->udp_sockfd < 0 ){
		fprintf( stderr , "udp socket() error.errno:%d\n" , errno );
		close( ch->tcp_sockfd );
		return NULL;
	}

	/* connectできなかったら何もできないので全部クローズして返す */
	if( connect( ch->tcp_sockfd , (struct sockaddr *) &sin , sizeof( struct sockaddr_in))<0){
		close( ch->tcp_sockfd );
		close( ch->udp_sockfd );
		fprintf( stderr , "connect error.errno: %d\n" ,errno);
		return NULL;
	}

	/*  要求タイムアウトの設定 */
	ch->timeout = 40;

	/* 準備完了 */
	strcpy( ch->errmsg , "" );
	return ch;
}

/*************
   クライアントを破壊する

*************/
int tsrpcDestroyClient( CLIENT*ch )
{

	/* Close sockets */
	close( ch->tcp_sockfd );
	close( ch->udp_sockfd );

	/* free memory */
	free( ch );

	return 0;
}


/************
  クライアントに関するエラーを得る

************/
char* tsrpcGetLastError( CLIENT *ch , char *err )
{
	if( err ) strcpy( err , ch->errmsg );

	return ch->errmsg;
}

/************
  メッセージを送信して結果を得る

************/
void call_tsrpctcp( CLIENT *ch , char *callbuf , char *retbuf )
{
	/* 書きこみは単純 */
	write( ch->tcp_sockfd , callbuf , strlen(callbuf ) );
	
	/* 読みこみは自前のライブラリルーチンを使う */
	tsrpcClientReadLine( ch , retbuf );

}
/************
  バッファされた読みこみ関数

  可能なかぎり読みこんで、バッファに1行以上のデータがきたらそこで
やめて、それだけシフトしてあとは残しておく

   selectをつかったノンブロッキング対応である。(タイムアウト設定可能)

************/
void tsrpcClientReadLine( CLIENT *ch , char*retbuf )
{
	
	fd_set rfds;
	time_t storet;
	
	time( &storet );



	while(1){
		char buf[READBUFSIZE];
		int readlen , afterlen;
		int i;
		time_t nowt;

		/* fd_set を作る */
		FD_ZERO( &rfds );
		FD_SET( ch->tcp_sockfd , &rfds );


		/* ひたすらからまわりする */
		select(  2 , &rfds , (fd_set*)NULL , (fd_set*)NULL , 0 );
		
		if( FD_ISSET( ch->tcp_sockfd , &rfds ) ){
			if( ( readlen = read( ch->tcp_sockfd , buf , sizeof( buf ) ) )<= 0 ){
				strcpy( retbuf , "" );
				sprintf( ch->errmsg , "read error. closed socket. errno:%d\n" , errno );
				close(ch->tcp_sockfd );
				return;
			}
			/* 読みこんだ分を加える。そのときにバッファの長さを越えたら増やす */
			afterlen = readlen + ch->readbuf_use;

			if( afterlen  > ch->readbufsize ){
				char *newbuf;

                /* 何も確保してなかったらまず確保する */
				if( ch->readbuf == NULL ){
					ch->readbufsize = READBUFSIZE;
					ch->readbuf = (char*) malloc( ch->readbufsize );
					ch->readbuf_use = 0;
					if( ch == NULL ){
						strcpy( retbuf , "" );
						ch->readbufsize = 0;
						strcpy( ch->errmsg , "cannot malloc readbuf." );
						return;
					}
					printf( "allocated %d bytes.\n" , READBUFSIZE );
				}

				printf( "exceeded. read:%d buf:%d use:%d\n" ,
						readlen , ch->readbufsize , ch->readbuf_use );

				/* あたらしいメモリを確保してからコピーしてfreeする */
				newbuf = (char*)malloc( afterlen );
				if( newbuf == NULL ){
					strcpy( retbuf , "" );
					sprintf( ch->errmsg , "memory shortage." );
					return;
				}
				bcopy( ch->readbuf , newbuf , ch->readbufsize );
				/* もちろん読みこんだ分も加えんとね */
				bcopy( buf , ch->readbuf + ch->readbuf_use , readlen );
				free( ch->readbuf );

				/* バッファを入れかえる */
				ch->readbuf = newbuf;
			}

			/* バッファの先頭からしらべて1行分そろってるかどうかしらべてそろってたらreturn */
			for(i =0 ; ; i++ ){
				if( ch->readbuf[i] == '\n' ){
					/* いっこでもこの記号があったら揃ってるということだ*/
					bcopy( ch->readbuf , retbuf , i  );/* 改行記号の前までコピー */
					retbuf[i+1] = '\0';   /* ヌル文字を付加 */
					strcpy( ch->errmsg , "" );
					return;
				}
			}
		}

		/* 時間をしらべてあきらめ時間になったら出てまう */
		
		time( &nowt );
		if( nowt > ( storet + ch->timeout ) ){
			strcpy( retbuf , "" );
			sprintf( ch->errmsg , "readline timeout(%dsec). " , ch->timeout );
			return;
		}
	}

	/* not reached */

}

