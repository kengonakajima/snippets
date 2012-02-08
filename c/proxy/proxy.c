


#include "common.h" /* 一般化された関数のプロトタイプとかが入る */

#include "proxy.h"


/*
 * ぷろくしをすたーとするなり
 * この関数のなかでつかわれるかんすうは全部 common.h にはいってるはず
 */

#define HTTP_DEFAULTPORT 9999        /* でふぉるとぽーと。 */
#define FTP_DEFAULTPORT 9990


char linebuf[65536*2];            /* ヘッダ1行の最大ながさ */
char linebuf2[sizeof(linebuf)];            /* ヘッダ1行の最大ながさ */

int http_port = HTTP_DEFAULTPORT;
int ftp_port = FTP_DEFAULTPORT;

int mainsockfd;

#include <stdio.h>

int startProxy( int argc , char **argv )
{
#if 0
    /* 以下のコードでサーバから情報をとってこれまする。 */
    int s;
    s = p_connectToHTTPServer( "pingu" , 80 );
    while(1){
        char hoge[100];
        char aho[11111];
        
        /* かきこんだあり */

        strcpy( hoge , "GET / HTTP/1.0\n\n");
        write( s , hoge , strlen( hoge ));

        sleep(1);
        {
            int r;
            r = read( s ,  aho, sizeof(aho));
            aho[r]=0;

            fprintf( stderr, "Data:[%s]\n", aho );
            fprintf( stderr, "Len:%d\n", strlen(aho) );
        }
    }
    
    return 0;
#endif
    
    /* サーバーソケットを初期化 */
    mainsockfd = p_initServerSocket( http_port , ftp_port );
    if( mainsockfd < 0 ) return 1;
    
    /* えいきゅーるーぷマワしちゃいましょう */
    while(1){
        int i;
        
        /* ポーリングする準備しましょう
           fd_set とかするのね
         */
        p_preparePolling(mainsockfd);

        /* ポーリングしちゃいましょう。
           ここでタイムアウトの指定とかできるので、
           あえて prepareと わけた .
           じっさいには doPollingすると、もうネットワーク
           から read してしまうとする。そしてバッファにため
           ちゃってるとする。
           */
        p_doPolling(mainsockfd);


        for(i=p_getMinPollingCount();
            i<p_getMaxPollingCount();i++){
            char buffer[5120];
            char req[1000]; /* てきとうすぎるからちゃんとすろ*/
            int r;


            
            if( p_isHTTP( i ) ){    /* HTTPで */
                if( p_isHTTPHeaderState(i)){    /* ヘッダをまだ処理してなくて*/
                    r = p_socketReadHTTPHeader(i,buffer,sizeof(buffer));
                    if( r <= 0 ) continue;

                    /* ヘッダがぜんぶきた。 */
                    if( p_isClientSocket(i)){   /* クライアント接続. */

                        int s;

                        p_startHTTPBody(i); /* 状態をすすめる */
                        
                        getHTTPRequest( buffer , req ,sizeof(req));
                        p_print( req );
                        
                        s = p_connectToHTTPServer( "pingu" , 80 );
                        if(s>0){
                            p_socketWrite( s,"GET / HTTP/1.0\n\n" ,
                                           strlen( "GET / HTTP/1.0\n\n" ));
                            /* ふたつのソケットを関連づける。
                               i はクライアントとの接続で、 s は
                               サーバとの接続。 */
                            p_associateSocket( i , s );
                        }
                    } else {        /* サーバー接続 */
                        p_startHTTPBody(i);
                        getHTTPRequest(buffer,req,sizeof(req));
                        p_print(req);
                    }
                } else {        /* へっだハ全部きた後だ。 */
                    r = p_socketRead( i , buffer ,sizeof(buffer));

                    if( r <= 0 ) continue;
                    
                    if( p_isClientSocket(i)){   /* クライアント接続だ*/
                        p_socketWrite( p_getAssociatedSocket(i),
                                       buffer , r );
                    } else {    /* サーバー接続だ */
                        int sa;
                        sa = p_getAssociatedSocket(i);
                        if( sa < 0 ){
                            /* クライアントの方がさきに切れている。
                               これでは仕事をしても意味がないなあ。
                               とじちゃえ */
                            fprintf( stderr, "Client closed first\n");
                        } else {
                            p_socketWrite( sa , buffer , r );
                        }
                    }
                }
            }
        }

        /* ポーリング終了。 */
        p_finishPolling( mainsockfd );
        
        
    }
    
    /* おわるのでとじちゃいましょう。 */
    p_closeServerSocket( mainsockfd );
    return 0;
}

/* ヘッダからリクエスト行をとりだす。
   先頭から改行の前の文字まで。
 */   
void getHTTPRequest( char *src , char *out , int maxlen )
{
    getStringFromIndexWithDelim( src , "\n" , 1, out , maxlen );        
}
/* リクエスト行の中のURL */
void getHTTPRequestURL( char *src , char *out , int maxlen )
{
    getHTTPRequest( src,linebuf, sizeof( linebuf));
    getStringFromIndexWithDelim( linebuf , " " , 2, out , maxlen );    
}
/* リクエスト行の中のMethod */
void getHTTPRequestMethod( char *src , char *out , int maxlen )
{
    getHTTPRequest( src,linebuf, sizeof( linebuf));
    getStringFromIndexWithDelim( linebuf, " " , 1 , out , maxlen );        
}
/* http:// とか ftp:// をとりだす。 */
void getHTTPRequestScheme(  char *src , char *out , int maxlen )
{
}
/* http://HOSTNAME/....  の HOSTNAMEをとりだす */
void getHTTPRequestHostname( char *src , char *out , int maxlen )
{
}
/* http://HOSTNAME/FILENAME の /FILENAMEをとりだす */
void getHTTPRequestFilename( char *src , char *out , int maxlen )
{
}


void getHTTPHost( char *src , char *out, int maxlen )
{
}


/*****************************************************************
  のぶおかのるーちん。アーキテクチャ依存なし。
  
*****************************************************************/

#define min( x,y ) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __x : __y; })
#define max( x,y ) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __y : __x; })
#define swap( x,y )({typeof(x) __x=(y);(y)=(x);(x)=__x;})
#define FALSE 0
#define TRUE 0
#undef NULL 0
#define NULL 0


/* strstr は linux のカーネルコードからもってきたった。 */
char * strstr(const char * s1,const char * s2)
{
	int l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *) s1;
	l1 = strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!memcmp(s1,s2,l2))
			return (char *) s1;
		s1++;
	}
	return NULL;
}


char* strncpy2( char* dest, const char* src, int n )
{
    if( n > 0 ){
        char*   d = dest;
        const char*   s = src;
        int i;
        for( i=0; i<n ; i++ ){
            if( *(s+i) == 0 ){
                /*  コピーしおわったら NULL 文字を入れる   */
                *(d+i) = '\0';
                return dest;
            }
            if( *(s+i) & 0x80 ){
                *(d+i)  = *(s+i);
                i++;
                if( i>=n ){
                    *(d+i-1)='\0';
                    break;
                }
                *(d+i)  = *(s+i);
            }else
                *(d+i) = *(s+i);
        }
    }
    return dest;
}

/*----------------------------------------
 * src から dest にコピーする。 コピー先のサイズを調べる。
 * 引数
 *      dest        コピー先
 *      n           サイズ
 *      src         元の文字列
 ----------------------------------------*/
void strcpysafe( char* dest ,int n ,const char* src )
{
    /*
     * src から dest へコピーする.
     * strcpy, strncpy では dest より コピーする量が
     * 大きい時に呼ぶと,メモリ外アクセスが起る.
     * これを防ぐ為に, strncpy があるが strlen( src ) が n より
     * 大きい時には, dest の最後が NULL 文字とはならない.
     *
     * したがって dest の大きさより src のほうが長い時には
     * n-1 で strncpy をする. 小さい時はそのままコピーする
     *
     * n が負の時はおかしくなるので負の時は 何もしない。
     *
     */

    if( n <= 0 )        /* 何もしない   */
        return;

    /*  この時点で、 n >= 1 以上が決定  */
    /*  NULL文字を考慮して比較する  */
    else if( n < strlen( src ) + 1 ){
        /*
         * バッファが足りないので n - 1(NULL文字)
         * で strncpy を呼ぶ
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';
    }else
        strcpy( dest , src );

}

/*----------------------------------------
 * src から length 映す。   コピー先の大きさを調べる。
 * 引数
 *          dest        コピー先
 *          n           コピー先のサイズ
 *          src         コピー元
 *          length      何文字コピーするか
 ----------------------------------------*/
void strncpysafe( char* dest , const int n ,
                  const char* src ,const int length )
{
    /*
     * src から dest に length コピーする
     * strcpy, strncpy では dest より コピーする量が
     * 大きい時に呼ぶと,メモリ外アクセスが起る.
     * この関数では、strlen( src ) と length の小さいほう
     * (実際にコピーする量) と dest のサイズを比べて、
     * strcpysafe と同じ事をする。
     */

    int Short;
    Short = min( strlen( src ) , length );

    /* NULL文字 を考慮した比較 */
    if( n < Short + 1 ){
        /*
         * バッファが足りないので n - 1(NULL文字)
         * で strncpy を呼ぶ
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';

    }else if( n <= 0 ){
        return;
    }else{
        /*
         * バッファは十分にあるので Short でstrncpyを呼ぶ
         * なお src には Short の長さ中に NULL がないので、
         * dest には 追加しておく。
         */

        strncpy2( dest , src , Short );
        dest[Short]= '\0';

    }
}

/*------------------------------------------------------------
 * strcatを安全に実現する。遅いので、特別な場合を除いて使わない事。
 * 引数
 *  src     char*       元になる文字列
 *  size    int         src のサイズ( 後ろにどれだけ残っているかでは
 *                          なくてバッファ全体 )
 *  ap      char*       付け加える物
 * 返り値
 *  src     (入れ子用)
 ------------------------------------------------------------*/
char* strcatsafe( char* src, int size, char* ap )
{
    strcpysafe( src + strlen( src ) , size - strlen(src), ap );
    return src;
}


int getStringFromIndexWithDelim( char* src ,char* delim ,int index,
                                 char* buf , int buflen)
{
    int i;          /* ループ変数 */
    int length =0;  /* 取り出した文字列の長さ */
    int addlen=0;   /* 足される長さ */

    for( i =  0 ; i < index ; i ++ ){
        char* last;
        src += addlen;          /* みつかった長さを足す */
        last  = strstr( src , delim );  /* 見付ける */

        if( last == NULL ){
            /*
             * みつからなかったのですべてコピーして return。
             */
            strcpysafe( buf , buflen, src );
            if( i == index - 1 )
                /*ちょうどみつかった*/
                return TRUE;

            /*みつからなかった*/
            return FALSE;
        }

        /*
         * みつかった所と最初の位置の差を求める
         * つまり区切られている文字列の長さ
         */
        length = last - src;

        /*
         * 次のループの為にみつかった長さと delim の長さを足しておく
         */
        addlen= length + strlen( delim );
    }

    strncpysafe( buf, buflen , src,length );
    return TRUE;
}
