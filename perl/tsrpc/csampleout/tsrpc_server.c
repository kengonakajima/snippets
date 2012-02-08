#define _TSRPC_SERVER_C_


#include "tsrpc.h"



/****************
    ディスパッチルーチン

	char * buffer : バッファ(readbuf )
	int sockfd;     対象fd 
	
	bufferには普通の

 bufferを読んでメモリを確保して関数を呼びだしてsockfdでwriteLineする

****************/
void tsrpcServerDispatcher( char *buffer , int sockfd )
{

	char **tk;

	tk = modifySplitString( buffer );
	
	if( strcmp( tk[0], "remotesum2" ) == 0 ){
		char *str = (char*)malloc( MKSTR_MINSIZE );
		int ret;
		int *intbuf;
		intbuf = (int*)malloc( sizeof( int ) * 10 );
		ret = remotesum2(
						demkstr_string( tk[1] ) ,
						demkstr_int32_array( tk , intbuf, 2 , 10 ) );

		str = mkstr_int32( str , ret );
		writeLine( sockfd , str );
		free( intbuf );
		free( str );

	} else {
		writeLine( sockfd , "server:undefind function." );
	}
}



/************
  サーバーを作る

  たくさんサーバーをつくってマルチスレッドでたくさんサービスするとかも可
************/
SERVER* tsrpcCreateServer( char *local , int port ,
					void(*loopfunc)(void))
{
	struct sockaddr_in sin;

	SERVER *s = (SERVER*)malloc( sizeof( SERVER ) );
	if(s == NULL ){
		fprintf( stderr , "cannot malloc SERVER struct.\n" );
		return (SERVER*)NULL;
	}
	bzero( s , sizeof( SERVER ) );

	/* ソケットを作ってバインドしてlistenする */

	s->tcp_sockfd = socket( AF_INET , SOCK_STREAM , 0 );
	if( s->tcp_sockfd < 0 ){
		fprintf( stderr, "socket(tcp) failed.\n" );
		return NULL;
	}
	s->udp_sockfd = socket( AF_INET , SOCK_DGRAM , 0 );
	if( s->udp_sockfd < 0 ){
		fprintf( stderr , "socket(udp) failed.\n" );
		return NULL;
	}
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons( port );

	if( local == (char*)NULL ){
		sin.sin_addr.s_addr = INADDR_ANY;		
	} else {
		/* なまえ をつけるぞ */
		struct hostent *h = gethostbyname( local );
		if( h == NULL ){
			fprintf( stderr , "gethostbyname failed. errno: %d\n" , errno );
			return NULL;
		}
		bcopy( h->h_addr_list[0] , &sin.sin_addr , 4 );
	}
	s->loopfunc = loopfunc;
	listen( s->tcp_sockfd , 5 );

	return s;
}

/************
  サーバーのメインループ

  char *local : ローカルホストの名前づけをするかどうか。 NULL を指定すると
  INADDR_ANYになる
  int port : port
  void (*loopfunc)(void) : ループごとに呼びだす関数

  読みこみも書きこみも可能なかぎりやって、1行分そろっているところをみつけると
  (それはかならずあたらしく読みこみがあったところであるから高速化になるぞ)
  そのソケットについて処理をする

************/
void tsrpcServerMainLoop( SERVER*sv )
{
	int i;

	while(1){
		fd_set rfds , wfds;


		FD_ZERO( &rfds );
		FD_ZERO( &wfds );
		FD_SET( sv->tcp_sockfd , &rfds );
		FD_SET( sv->udp_sockfd , &rfds );
		FD_SET( sv->tcp_sockfd , &wfds );
		FD_SET( sv->udp_sockfd , &wfds );
		for(i =0 ;i<MAXSOCKFD;i++){
			if( sv->sockuse[i] != 0 ){
				FD_SET( i , &rfds );
				FD_SET( i , &wfds );
			}
		}

		select( 30 , &rfds , &wfds, (fd_set *)NULL , 0 );

		/* メインソケットに入力がきたならacceptするのだ */
		if( FD_ISSET( sv->tcp_sockfd ,&rfds ) ){
			struct sockaddr_in c;
			int len;
			int newsockfd = accept(sv->tcp_sockfd , (struct sockaddr *) &c ,  &len );
			if( newsockfd > 0 ){
				sv->sockuse[newsockfd] = 1;       /* フラグだけでよい */
				sv->cli[newsockfd] = c;    
			}
		}

		/* こどもソケットにきたなら読みこむのだ */
		for(i=0;i<MAXSOCKFD;i++){
			if( sv->sockuse[i] && FD_ISSET( i ,&rfds ) ){
				char buf[READBUFSIZE];
				int readlen, newbuflen , linelen;
				readlen = read( i , buf , sizeof( buf  ) );
				if( readlen <= 0 ){
					/* ソケットがふぁっくしタ */
					close( i );
					continue;
				}
				/* ちゃんと読みこめたからバファにくわえる */


				newbuflen = ( readlen + sv->readbuf_use[i] );
				/* サイズを越えたら再確保 */
				if( newbuflen > sv->readbufsize[i] ){     				
					/* 旧バッファがない場合は確保 */
					if( sv->readbuf[i] = (char*) NULL){
						sv->readbuf[i]= (char*)malloc( READBUFSIZE ); /* 初期バッファ */
						if( sv->readbuf[i] == (char*)NULL) continue;
						bzero( sv->readbuf[i] , READBUFSIZE );
						bcopy( buf ,sv->readbuf[i] , readlen );
						sv->readbuf_use[i] = readlen;
					} else {
						/* 旧バッファがある場合は新にコピーしてappend */
						char *newbuf = (char*) malloc( newbuflen ) ;
						if( newbuf == (char*)NULL ) continue;    /* メモリたりんかったらdata捨てる */
											
						bcopy( sv->readbuf[i] , newbuf , sv->readbuf_use );
						bcopy( buf , newbuf + sv->readbuf_use[i] , readlen );  /* ここでnewbuf完成*/
						free( sv->readbuf[i] );   /* フリーしてバッファ入れかえ */
						sv->readbuf[i] = newbuf;
						sv->readbuf_use[i] += readlen;
					}
				} else {
					/* サイズを越えなかったらそのままアペンド */
					bcopy( buf , sv->readbuf[i] + sv->readbuf_use[i] , readlen );
					sv->readbuf_use[i] += readlen;
				}

				/*バッファを変更できたら、1行そろってるかどうか調べる。改行記号までをシフト */
				if( ( linelen =  checkLineEnough( sv->readbuf[i] , sv->readbuf_use[i])) ){
					int ll;


					/*改行記号をヌル文字にしてから呼びだし */
					sv->readbuf[linelen-1] = '\0';
					
					tsrpcServerDispatcher( sv->readbuf[i] , i );
					for(ll=0;ll<linelen;ll++){
						sv->readbuf[i][ll] = sv->readbuf[i][ll+linelen];
					}
					sv->readbuf_use[i]-=linelen;
				}
			}
		}

		/* UDP
		if( FD_ISSET( sv->udp_sockfd , &rfds ) ){
		}

		/* ループ関数を呼びだす */
		if( sv->loopfunc) sv->loopfunc();

	}
}
/************
   サーバーを破壊する

************/
void tsrpcDestroyServer( SERVER*s )
{
	int i;
	close( s->tcp_sockfd );	
	close( s->udp_sockfd );
	for(i=0;i<MAXSOCKFD;i++){
		if( s->sockuse[i] != 0 ){
			close( i );
		}
	}
}

/************
  バッファに1行分以上そろっているかどうかしらべる
  return value : 0 そろってない 1 そろってる
  
************/
int checkLineEnough( char *buf , int max )
{
faskjdhfksjahf	
}
