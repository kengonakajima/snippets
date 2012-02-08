


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>



/*
     ぷろくし用  linux.c by ringo


     linux のアーキテクチャに依存する部分をかく。といってもそんなところ
     はない。普通にUnixなことをするだけ。まっくとかうぃんに移植しようと
     おもってるからわざわざこんなコードがある。

     なんせ int main(...... もないかもしれへんしのう。

     mac.c
     win.c もあるんかな。 macとwinはアイコンをDblClickするんでしょな。

     「ぷろくし」のコードの構造
     
     ラッパープログラムが アーキテクチャごとにある。
     * mac.c , win.c , linux.c
     こいつらのうちひとつをえらんで、さらに
     proxy??.c を全部コンパイルするとできあがりとする。

     ファイルを分割する必要がでてきたら、 mac??.c とかにしていこう。
     ラッパープログラムで用意する関数群は、みんな同じ名前の関数群
     を用意する約束にしておかないと、proxy??.c を作れなくなる。
     ここにそのリストをかいとこう。

まず型。 ソケットをあらわすハンドルは、 Unix の fd みたいに int の
番号(ファイルの表へのindex)にする。それをシミュレートするのは、
ラッパーの仕事。 linux.c ではとりあえず何もしなくていいので、
何もしない。 linux.c でコンパイルした場合は、その値はたぶん3か4くらいから
はじまるけど、 macとかだと 0からはじまるのかもしれん。それはそれでよい。
ハンドルの値自体に意味はない。ソケットごとに異なればよい。

返り値のやくそく
これもUnixのシステムコールぽく、負なら失敗、0で成功、バイト数とかは
int でかえしましょう。

int fd = setupServerSocket( int port ) : サーバーソケットを生成する。
win.c だとwinsock を初期化する処理がはいるのでしょう。
mac.c だとわかりません。いったいどうやってやんのかな。



1   まずぷろくしが起動するときに、かならず win,max,linuxのどれでも、
   startProxy( int argc , char *argv ) という関数がよばれる。
   これが main関数のラッパーである。argc,argv の仕様は、unixにあわせる
   こと。マックとかウィンだったら、これは起動時にダイアログを出して
   求めたりとかショートカットに登録したりとかするんだろう。


 */

#include "common.h"     /* win,mac,linuxがみんな用意する一般化関数。 */
#include "proxy.h"      /* proxy の機能についての */
#include "linux.h"      /* linux だけ。 */
                           

#define DEFAULTLOG  "proxy.log"


/*
 *
 * 接続を管理するための構造体
 * index に fd をそのままつかう。
 */
typedef enum
{
    CONTYPE_CLI = 1,        /* クライアントからのコネクション */
    CONTYPE_SV,             /* サーバーとのコネクション */
    CONTYPE_MAX
} CONTYPE;

typedef enum
{
    CONSTATE_HTTP_HEADER = 100, /* HTTP のヘッダをよみかきしている状態。*/
    CONSTATE_HTTP_BODY,         /* HTTP のヘッダのあとの空行のあと
                                   をよみかきしている状態。 */
    CONSTATE_FTP,               /* FTP のとき。 */
    CONSTATE_MAX
} CONSTATE;

#define NETBUFSIZE 65536
typedef struct
{
    int use;                /* 使用中かどうかのフラグ */
    char rb[NETBUFSIZE];    /* 読みこみバッファー */
    int rbuse;                       
    char wb[NETBUFSIZE];    /* 書きこみバッファー */
    int wbuse;
    int id;                 /* socket id */
    CONTYPE type;           /* こねくしょんの種類 */
    CONSTATE state;         /* こねくしょんの状態 */
    int close_flag;         /* このフラグがたっていると、データ送信が
                               終了した時点で閉じる */
    int asoc_id;            /* 関連づけられているサーバ用接続。-1だったら
                             まだ初期化されていない。*/

} Con;

#define MAXCON 128
Con con[MAXCON];            /* 最大の fd */

/*
 * 適当なメインルーチン。
 *
 *
 *
 *
 */
int main(int argc , char **argv )
{

    /* Linux では、これだけ。 まっくとか大変そうね */
    return startProxy( argc, argv );

}

/*
 * コネクションに ID をふる。
 *
 *
 */
static int p_createNewSocketID( void )
{
    static int s_id = 0;

    s_id ++;

    return s_id;
}

/*
  一般化用関数。このプロトタイプは common.h にあり

  初期化されたサーバソケットをかえす

  しっぱいしたら負ね.成功したら正の値

  とりあえず ftp についてはほったらかし

  http_port からはじめるが bind エラーがでたら1づつふやしてためす。
  100かいやってだめだったらあきらめる。
 */
int p_initServerSocket( int http_port , int ftp_port )
{
    int i;
    int ret;
    int s;
    
    /* まず HTTP をやる */
    for( i = 0 ;i < 100;i++){
        
        struct sockaddr_in sin;
        
        s = socket( AF_INET , SOCK_STREAM , 0 );
        if( s < 0 ) return s;

        bzero( &sin , sizeof( sin ));
        sin.sin_port = htons( http_port + i );
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;

        ret = bind( s , (struct sockaddr*)&sin,
                    sizeof(struct sockaddr_in ));
        if( ret < 0 ){
            close( s );
            continue;
        } else {
            break;
        }
    }
    if( i == 100 ) return -100;
    if( i > 0 ){
        char msg[100];
        sprintf( msg , "Bound on port: %d\n" , http_port + i );
        p_print( msg);
    }
    
    ret = listen( s , 5 );
    if( ret < 0 ){
        p_printError( "listen error!\n");
    }

    p_print( "server socket ok\n" );
    return s;
}
/*
 * サーバーソケットをとじましょうの関数
 */
void p_closeServerSocket( int s )
{
    close( s );
}
/*
  一般化用かんすう

  エラーメッセージをだす。 まっくとかういんなら、
  ダイアログボックすかな。ファイルにログする場合はどこにだすのか
  なあ。
  
 */
void p_printError( char *s )
{
    /* linux ではそのまま出す。ファイルへのログは、
       何も考えていない仕様だが       */
    fprintf( stderr , s );

    {
        FILE *fp = fopen( DEFAULTLOG , "a" );
        if( !fp ) return;
        fprintf( fp , s );
        fclose(fp);
    }
}
/* エラーじゃないプリント */
void p_print( char *s )
{
    p_printError( s );
}

/*
 * ポーリングの準備
 * 
 * int sockfd : メインの accept用のやつ
 */
fd_set rfd , wfd , efd;   /* ポーリング用データ構造 */
struct timeval timeout;     /* タイムアウトの時間 */
void p_preparePolling( int sockfd )
{
    int i;
    
    FD_ZERO( &rfd );
    FD_ZERO( &wfd );
    FD_ZERO( &efd );

    timeout.tv_sec = 0;
    timeout.tv_usec = 1000*100;

    /* メインのsockfd は毎回初期化。 */
    FD_SET( sockfd , &rfd );
    FD_SET( sockfd , &efd );

    /* 子供ソケット */
    for(i=p_getMinPollingCount();i<p_getMaxPollingCount();i++){
        if( !con[i].use ) continue;

        FD_SET( i , &rfd );
        FD_SET( i , &efd );
        if( con[i].wbuse > 0 ) FD_SET( i , &wfd );
    }

}

/*
 * Write buffer を しふとする
 *
 */
static int p_shiftWriteBuffer( int s , int len )
{
    int cp;
    if( con[s].wbuse <= len ){
        cp = con[s].wbuse ;
    } else {
        cp = len;
    }
    
    memmove( con[s].wb , con[s].wb + cp , con[s].wbuse - cp );
    
    con[s].wbuse -= cp;
    return cp;
}


/*
 * ねっとわーくのよみこみ/かきこばっふぁーに追加する
 */
static int p_appendReadBuffer( int s , char *buf , int len )
{
    int cp;
    
    if( !con[s].use )return -1;
    if( ( con[s].rbuse + len ) >= NETBUFSIZE ){
        cp = NETBUFSIZE - con[s].rbuse;
    } else {
        cp = len;
    }
    memcpy( con[s].rb + con[s].rbuse , buf , cp );
    con[s].rbuse += cp;
    
    return cp;
}
static int p_appendWriteBuffer( int s, char *buf , int len )
{
    int cp;
    if( !con[s].use )return -1;
    if( ( con[s].wbuse + len ) >= NETBUFSIZE ){
        cp = NETBUFSIZE - con[s].wbuse;
    } else {
        cp = len;
    }
    memcpy( con[s].wb + con[s].wbuse , buf , cp );
    con[s].wbuse += cp;
    return cp;
}


/* close it */
static void p_closeSocket( int s )
{
    close(s);
    con[s].use = con[s].rbuse = con[s].wbuse = 0;
    con[s].asoc_id = con[s].id = -1;
    fprintf( stderr,"closed socket\n");
}

/*
 * 実際にポーリングする
 *
 * int sockfd : メインの socket fd
 */
#define RWUNIT   1024

void p_doPolling( int sockfd )
{
    int r,i;

    r = select( p_getMaxPollingCount() + 1, &rfd , &wfd , &efd, &timeout );

    /* accept もしてまう。 */

    if( FD_ISSET( sockfd , &rfd )){
        int newsockfd;
        int addrlen = sizeof( struct sockaddr_in );
        struct sockaddr_in sin;
        
        newsockfd = accept( sockfd ,  (struct sockaddr*)&sin,
                            &addrlen );
        if( newsockfd < 0 ){
            p_printError( "accept error\n");
        } else if( con[newsockfd].use ){
            p_printError( "con array inconsistency\n");
        } else {
            /* 正常ね。 */
            con[newsockfd].use = 1;
            con[newsockfd].rbuse = con[newsockfd].wbuse = 0;
            /* acceptしてできたやつは、クライアントとの接続用なのだ。 */
            con[newsockfd].type = CONTYPE_CLI;
            con[newsockfd].id = p_createNewSocketID();
            con[newsockfd].state = CONSTATE_HTTP_HEADER;
            con[newsockfd].close_flag = 0;
            con[newsockfd].asoc_id = -1;
        }
    }


    for(i=p_getMinPollingCount();i<p_getMaxPollingCount();i++){
        if( !con[i].use )continue;
        
        /* まず read */
        if( FD_ISSET( i , &rfd )){
            int r;
            char buf[RWUNIT];
            

            r = read( i , buf , sizeof( buf ) );
            if( r <= 0 ){

                /* ここで使用中フラグがおちるので、そのあとリードしよう
                   としても、エラーになるので、そこでのエラー処理は必要ない*/
                fprintf( stderr , "read error\n");
                if( !p_isClientSocket(i)){
                    fprintf( stderr , "close asociated client\n");
                    p_setCloseFlag( p_getAssociatedSocket(i));
                }
                p_closeSocket( i );

                continue;
            }
            p_appendReadBuffer( i , buf , r );
        }
        if( FD_ISSET( i , &wfd )){
            int w,tobe;
            
            if( con[i].wbuse >= RWUNIT ) tobe = RWUNIT;
            else tobe = con[i].wbuse;
            
            w = write( i , con[i].wb , tobe );
            if( w <= 0 ){
                p_closeSocket(i);
                continue;
            }
            p_shiftWriteBuffer( i , w );

            /* Close flag がたっていたらとじる */
            if( con[i].wbuse == 0 && con[i].close_flag ){
                p_closeSocket(i );
                continue;
            }
        }
        if( FD_ISSET( i , &efd )){
            /* なにもしない、とりあえず */
        }
    }
}

/*
 * ポーリングの終了処理。なにもしないぞ
 */
void p_finishPolling( int s  )
{
    ;
}


/*
 * 行が完成してるかどうかしらべる。
 * 最初から検索して'\n' があったら 1をかえす
 *
 * かえりち： -1 なら完成してない。 0以上なら、その位置に改行がある。
 */
static int p_completeLine( int fd )
{
    int i;
    char *s = con[fd].rb;
    int len = con[fd].rbuse;

    for(i=0;i<len;i++){
        if( s[i] == '\0' ) return -1;
        if( s[i] == '\n' ) return i;
    }
    return 0;
}
/* ヘッダが完成してたら1をかえす。完成してない場合は0 */
static int p_completeHeader( int fd )
{
    int i;
    char *s = con[fd].rb;
    int len = con[fd].rbuse;

    if( len <= 1 ){
        return 0;
    }
    for(i=0;i<len-1;i++){
        if( s[i] == '\n' &&
            s[i+1] == '\n' ) return i+1;
    }
    return 0;
}

/*
 * 一般化されたソケットから読みこむ
 *
 */
int p_socketRead( int s , char *buf , int maxlen )
{
    int cp;


    if( !con[s].use ) return -1;
    
    if( con[s].rbuse == 0 ) return 0;
    
    if( con[s].rbuse <= maxlen ){
        cp = con[s].rbuse ;
    } else {
        cp = maxlen;
    }
    
    memcpy( buf , con[s].rb , cp );
    memmove( con[s].rb, con[s].rb + cp , con[s].rbuse - cp );
    
    con[s].rbuse -= cp;

    return cp;
}

/*
 * \rの問題をかいけつする。 \rをぜんぶ削除してシフトする。
 * readline以外のときにやったらバイナリがこわれるからだめだよ。
 */
static void p_compressLine( int fd )
{

    int src_i = 0 , dest_i = 0;
    int modcount = 0;
    char *s = con[fd].rb;
    int len = con[fd].rbuse;
    
    while(1){
        s[dest_i] = s[src_i];
        
        if( s[src_i] == '\0' )break;
        if( s[src_i] == '\r' ){ dest_i += 0; modcount++;}else dest_i ++;
        src_i ++;
        if( src_i == len )break;
    }
    
    s[dest_i]='\0';
    con[fd].rbuse -= modcount;
}

/*
 * 一般化されたソケットから1行よみこむ。行がないけどバッファにはたまってる
 * 場合は、なにもよみこまない。HTTPヘッダを読むのにつかう。
 * 返り値は、よみこんだバイト数。
 *
 * '\n\r' も '\n' も '\r\n' も、ぜんぶ '\n' に変換してかえす。
 * maxlen が1のときでも、  上記の変換をするように、1文字余分な
 * バッファを自前で用意する。
 *
 *かえりち： -1: エラー
             -2: バッファがみじかすぎて、1行よみこめなかったのであきらめた。
 */
int p_socketReadLine( int s , char *buf , int maxlen )
{

    int newline_index;
    int r;
    
    if( !con[s].use )return -1;
    if( con[s].rbuse == 0 )return 0;
    
    /* まず、1行が完成してるかどうかしらべる */
    if( (newline_index = p_completeLine( s )) <= 0 )
        return 0;

    /* \r の問題をここでかいけつ */
    p_compressLine( s );
    

    /* どこに改行記号があるのかを利用して。 */
    if( newline_index >= maxlen ) return -2;

    r = p_socketRead( s , buf , newline_index );

    if( r > 0 ){
        buf[r]='\0';       /* ぬるもじ */
    }
    
    return r;
}

/*
 * HTTP のヘッダを読みこむ。完成してるときだけ。
 * 完成とは、 compressLineしたあとに、 "\n\n" がバッファーにふくまれている
 * とき。出力にはこの空行をふくめて返す。
 *
 * 返り値： 実際にヘッダをよみこんだばあいはそのバイト数。
 * まだ完成してないときとかは0、 エラーだったら負
 *
 * -1 : エラー
 * -3 : ヘッダがバッファにはいりきらない。
 * -5 : HTTP ではない。
 */
int p_socketReadHTTPHeader( int s , char *buf , int maxlen )
{
    int last_newline_index;
    int r;
    
    if( !con[s].use ) return -1;
    if( con[s].rbuse == 0 ) return 0;
    if( !p_isHTTP(s)) return -5;
    
    /* \r の問題をここでかいけつ */
    p_compressLine( s );
    
    /* ヘッダが完成してるかどうかしらべる。 */
    if( ( last_newline_index = p_completeHeader(s)) <= 0 ){
        return 0;
    }

    if( last_newline_index >= maxlen ) return -3;

    r = p_socketRead( s , buf , last_newline_index );
    if( r >0 ){
        buf[r] = '\0';
    }
    return r;
}

/*
 * 一般化されたソケットにかっきこむ。
 *
 * 返り値： じっさいに書きこんだバイト数
 */
int p_socketWrite( int s , char *buf , int len )
{
    if( !con[s].use ) return -1;
    return p_appendWriteBuffer( s , buf , len );
}

/*
 * 接続の配列をつかう最低のインデクスをかえす
 *
 */
int p_getMinPollingCount( void )
{
    return 3;       /* デバッガをつかうときは変化するが無視 */
}
/* 最大の index+1 ( さいず ) をかえす */
int p_getMaxPollingCount( void )
{
    return sizeof( con ) / sizeof( con[0] ) ;
}

/*
 * ホスト名からアドレスをとってきて connectしたソケットをかえす
 * 
 * Windowsでは、ノンブロッキングのconnectをつくる方法がわからなかった。
 * ので、ふつうにブロックするバージョんをまずつくってみる。
 *
 * かえりち：
 * -1 : gethostbyname error ( siteがみつからんとかいう )
 * -2 : socket error
 * -3 : connect error
 * -4 : 回復できない内部エラー(ばぐってます。)
 */
int p_connectToHTTPServer( char *h , int port )
{
    struct hostent* he;
    struct sockaddr_in sin;
    int sockfd;
    int r;
    
    sin.sin_family = AF_INET;
    sin.sin_port = htons( port );
    sin.sin_addr.s_addr = inet_addr( h );
    if( sin.sin_addr.s_addr == -1 ){
        he = gethostbyname( h );
        if( he == NULL ){
            p_print( "gethostbyname err." );
            return -1;
        }
        memcpy((void*)&sin.sin_addr.s_addr,
               he->h_addr, sizeof( struct in_addr));
    }

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );
    if( sockfd <0 ){
        p_printError( "socket error\n" );
        return -2;
    }

    r = connect( sockfd , ( struct sockaddr*) &sin,
                 sizeof( struct sockaddr_in ));
    if( r < 0){
        p_print( "cannot connect\n");
        return -3;
    }


    
    /* ソケットが初期化できたので、こんどは、Conに登録する。 */
    if( con[sockfd].use ){
        p_printError( "in p_connectToServer: Con array inconsistency\n");
        return -4;
    }
    
    fprintf( stderr , "created client socket fd%d\n",sockfd);
    
    con[sockfd].use = 1;
    con[sockfd].rbuse = con[sockfd].wbuse = 0;
    con[sockfd].type = CONTYPE_SV;
    con[sockfd].id = p_createNewSocketID();
    con[sockfd].state = CONSTATE_HTTP_HEADER;
    con[sockfd].close_flag = 0;
    con[sockfd].asoc_id = -1;
    return sockfd;
}

/*
 * プロトコルの種類をしらべる
 *
 * 0だったらそのプロトコルではない。1だったらそのぷろとこるである。
 */
int p_isHTTP( int fd )
{
    if( !con[fd].use )return 0;
    if( con[fd].state == CONSTATE_HTTP_HEADER ||
        con[fd].state == CONSTATE_HTTP_BODY ){
        return 1;
    } else {
        return 0;
    }
}
int p_isFTP( int fd )
{
    if( !con[fd].use )return 0;
    if( con[fd].state == CONSTATE_FTP ){
        return 1;
    } else {
        return 0;
    }
}



/*
 * Close flag をつける
 *
 */
void p_setCloseFlag( int fd )
{
    
    if( fd < 0 || fd >= p_getMaxPollingCount() || !con[fd].use ) return;
    con[fd].close_flag = 1;
}



/*
 * ヘッダーのよみこみがおわって本体にいっている。
 *
 */
void p_startHTTPBody( int s )
{
    con[s].state = CONSTATE_HTTP_BODY;
}

int p_isHTTPHeaderState( int s )
{
    return ( con[s].state == CONSTATE_HTTP_HEADER );
}

int p_isClientSocket( int s )
{
    return ( con[s].type == CONTYPE_CLI );
}

/*
 * 接続が切れてるかどうかしらべる
 *
 */
int p_isClosed( int s )
{
    int sa;
    /* 普通はこれで OK。でも、おなじfdを再利用する
       から、次の条件がひつようになるのだ。 */
    if( s < 0 || s >= p_getMaxPollingCount() ) return 0;
    if( con[s].use == 0 )return 1;

    /* この検索のところをみればわかるが、 使用中でidが一致するものだけ
       を検索するので、目的は達成されるのだ。 */
    sa = p_getAssociatedSocket(s);
    if( sa < 0 ){
        return 1;
    }

    return 0;
}
/*
 * 2本の接続を関連づける、それを得る
 *
 * int c : client connection
 * int s : server connection
 */
void p_associateSocket( int c , int s )
{
    con[c].asoc_id = con[s].id;
    con[s].asoc_id = con[c].id;
    fprintf(stderr,"Associated fd%d(%d) <->  fd%d(%d)\n",
            c,con[c].id , s,con[s].id);
}
int p_getAssociatedSocket( int so )
{
    int i;
    for(i=p_getMinPollingCount();i<p_getMaxPollingCount();i++){
        if( con[so].asoc_id == con[i].id &&
            con[i].use && con[so].asoc_id >=0 ){
            return i;
        }
    }
    return -1;
}
