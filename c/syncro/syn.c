#define _SYN_C_

#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>


#include "syn.h"

#define IOCTLFUNC ioctl        /* win32ではこれを ioctlsocket に変更 */

/*
 *
 *  ゲームのコードは基本的には
 *
 * 1 initSyncroする
 * 2 addSyncroAddrをつかって自分以外の全員のアドレスを登録
 * 3 negotiateSyncroHostsをループさせてホストを全部揃える
 * 4 ゲーム開始。でdoSyncro()する
 * 5 ゲーム終了でfinishSyncro()する
 * 
 * 1から5をくりかえす。
 *
 * というようになるのだ。
 * ホスト同士は2本づつ、read用、write用のソケットをもつ。
 * nホストだったら1ホストあたり、2(n-1)本のコネクションを持つことになる。
 * そのコネクションが全部そろって、はじめてOKとなる。
 * negotiateSyncroHostsは、相手ホストとの2本がそろうとnowが1増える。
 */


SOCKET sockfd;            /* メインのソケット */
unsigned short port;      /* 自分のポート */
int current_session_id;                   /* カレント セッションID */
int current_my_id;                        /* 世界にひとつしかない自分のId. */
BOOL initialized = FALSE;        /* 初期化されているかどうか */

struct sockaddr_in sockaddr;     /* 自分のアドレス */

#define MAXCON 128

typedef struct {
    int use;
    int sockfd;
    int init;                    /* sockfd が意味のある値なら1 */

    int type;               
#define CON_TYPE_READ  101   /* acceptして、readだけするソケット */
#define CON_TYPE_WRITE 102   /* connectして、writeだけするソケット */
    
    char asciiaddr[128];         /* アスキー文字列のアドレス。  */
    unsigned short port;         /* ポート。ホストバイトオーダ。connectするときつかう */
    
    struct sockaddr_in peer;     /* ピアアドレス。acceptするときつかう */

    int my_id;                   /* 相手のmy_id. TYPE_READでつかう。この値がおなじ
                                    接続が2個以上きたら2個目以降は完全無視 
                                  */
    
    int negotiation_ok;          /* negotiationがうまくいったかどうか */
    
    
} CONNECTION;

CONNECTION con[ MAXCON ];

extern int errno;


static int findBlankConnection( void );
static void prepareAllConnections(void);
static void releaseConnection( int index );

/*
 * syncro をしょきかする。
 * 
 * unsigned short port : まちうけポート. ホストバイトオーダで
 * int session_id : セッションID
 * int my_id : 自分のID. 世界でひとつしかない。マッチングさーばカラモラウ。
 *
 * return value:
 * -1 : すでに初期化されているので、先にfinishSyncroしてほしい。
 */
int initSyncro( unsigned short port , int session_id , int my_id )
{
    int ret;
    
	if( initialized ) return -1;

    /* ID 類を記憶 */
    current_session_id = session_id;
    current_my_id = my_id;
    

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons( port );
    sockaddr.sin_addr.s_addr = INADDR_ANY;

    ret = bind( sockfd ,(struct sockaddr *)&sockaddr,sizeof( struct sockaddr_in ));
	if( ret < 0){
		perror((char *)strerror(errno));
		exit(1);
	}

    ret = listen( sockfd , 5 );
	if(ret < 0){
		perror( (char *) strerror(errno) );
        exit(1);
	}
    
    initialized = TRUE;

    return 0;
}

/*
 * リモートホストのアドレスを追加。自分以外のだけ。自分を追加しても無視される(追加されない)
 *
 * int my_id : 相手のmy_id。
 */
int addSyncroAddr( char *ipaddr, unsigned short port )
{
    int index;
    
    if( !initialized )return -1;

    index = findBlankConnection();
    if( index < 0 )return -1;


    /* 構造体に情報をコピーする */
    con[index].use = 1;
    strcpy( con[index].asciiaddr , ipaddr );
    con[index].port= port;
    con[index].type = CON_TYPE_WRITE;
    con[index].negotiation_ok = 0;
    con[index].init = 0;                /* まだsocket()してない */
    con[index].my_id = current_my_id;

    fprintf( stderr , "added addr:%s port:%d\n" , ipaddr , port );
    return 0;
}

/*
 * リモートホストが揃うのをまつ。
 *
 * この関数はノンブロッキングで、ループをまわして待つ。
 *
 * int *total : 合計でいくつのホストがあるのか。addSyncroAddrした回数。
 * int *now : 現在いくつのホストがnegotiation完了しているか
 * 
 * return value:
 * 0 正常終了。
 * 負 エラー。これ以上処理をつづけることができないので、finishSyncroしたほうがよい。
 * total , now が等しくなるまで、自分で時間をはかって待つべし。
 * それで、ここ以降ではacceptしないので、もうプレイヤーが増えることはない。
 *
 * やることは accept, connectをノンブロッキングでやるだけだ。
 * 
 */
int negotiateSyncroHosts( int *total, int *now )
{
    fd_set rfds , wfds ,efds;
    struct timeval tm;
    int i;
    
    if( !initialized ) return -1;


    /* 準備できていないソケットがあったら準備する*/
    prepareAllConnections();
        
    tm.tv_usec = 0;
    tm.tv_sec = 0;


    FD_ZERO( &rfds );
    FD_ZERO( &wfds );
    FD_ZERO( &efds );


    FD_SET( sockfd , &rfds );
    FD_SET( sockfd , &wfds );
    FD_SET( sockfd , &efds );

    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){
            FD_SET( con[i].sockfd , &rfds );
            FD_SET( con[i].sockfd , &wfds );
            FD_SET( con[i].sockfd , &efds );
        }
    }
    
    select( MAXCON + 5 , &rfds , &wfds , &efds , &tm );

    /* メインソケットがよみこみ可能だったら、アクセプトできそうなことを意味する */
    if( FD_ISSET( sockfd ,&rfds )){
        struct sockaddr_in peer;
        int peeraddrlen = sizeof( peer );
        
        int newsockfd = accept( sockfd, (struct sockaddr*)&peer , &peeraddrlen );
        if( newsockfd < 0 ){
            perror( (char*) strerror( errno ));
        } else {
            int index = findBlankConnection();
            if( index < 0 ){
                /* もうこれ以上acceptできない(異常事態 )*/
            } else {
                con[index].use = 1;
                con[index].init = 1;
                bcopy( (char*)&peer , & con[i].peer ,sizeof(struct sockaddr_in));
                con[index].type = CON_TYPE_READ;
                con[index].sockfd = newsockfd;
                con[index].negotiation_ok = 0;
                con[index].my_id = 0;            /* これからうけつけるのだ */

#if 1
                {
                    fprintf( stderr,"accepted index:%d fd:%d\n",index,con[index].sockfd);
                }
#endif
            }
        }
    }

    /* コネクションをチェック */
    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){
            char buffer[1024];
            
            if( FD_ISSET( con[i].sockfd , &rfds )){
                if( con[i].type == CON_TYPE_READ ){
                    int ret =  read( con[i].sockfd , buffer , sizeof( buffer ));
                    if( ret <= 0 ){
                        /* そのソケットはもうだめだこりゃ */
                        releaseConnection( i );
                        continue;
                    } else {
                        /* 何か読みこめたぞ。ネゴシエーションの情報かどうかしらべる */
#if 1
                        buffer[ret] = 0;
                        fprintf( stderr, "Read data:[%s]\n", buffer );
#endif
                    }
                }
            }
            if( FD_ISSET( con[i].sockfd , &wfds )){
                if( con[i].type == CON_TYPE_WRITE ){
                    
#if 0
                    /* ついに connectできたかのチェックがはいるはずだがwindowsとは
                     やりかたがことなるようだ。なのでPENDING */
                    fprintf( stderr, "connect OK? on index:%d fd:%d\n", i,con[i].sockfd);
/*                    strcpy( buffer , "fucking!" );
                    write( con[i].sockfd ,buffer , strlen( buffer ));*/
#endif
                }
                       
            }
            if( FD_ISSET( con[i].sockfd , &efds )){
                if( con[i].type == CON_TYPE_READ ){
                    fprintf( stderr , "Fucking Exception on index:%d fd:%d\n" ,
                             i , con[i].sockfd );
                }
            }
        }
            
    }

    /* 現在の状況を出力 */
    *total = *now = 0;
    
    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){
            (*total)++;
            if( con[i].negotiation_ok ){
                (*now)++;
            }
        }
    }
    
    return 0;
}

/*
 * もうつかわない、つかえない、ソケットを解放する
 *
 * int index : どのコネクションを解放するのか
 */
static void releaseConnection( int index )
{
    con[index].use = 0;
    close( con[index].sockfd );
    bzero( & con[index] , sizeof( con[0] )) ;

#if 1
    fprintf( stderr , "Released connection index:%d\n" , index );
        
#endif
}

/*
 * すべてのコネクションをsocket()する. sockfdが意味のある値になるようにする。
 * もちろん、connectするタイプのやつだけ。
 *
 *
 */
static void prepareAllConnections(void)
{
    int i;

    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].type == CON_TYPE_WRITE &&
            con[i].init == 0 ){
            int flg;
            int ret;
            SOCKET s;
            struct sockaddr_in sin;


            
            s = socket( AF_INET , SOCK_STREAM , 0 );

            flg = 1;

#if 0
            /* ノンブロッキングにする */
            IOCTLFUNC( s , FIONBIO , &flg );
#endif
            
            sin.sin_family = AF_INET;
            sin.sin_port = htons( con[i].port );
            sin.sin_addr.s_addr = inet_addr( con[i].asciiaddr );

            ret = connect( s ,(struct sockaddr*)&sin , sizeof(sin ));

            if( ret >= 0 ){
                con[i].init = 1;
                con[i].sockfd = s;
                fprintf( stderr , "Connect OK on index:%d" , i );
            } else {
                close( s );
                fprintf( stderr , "Connect fail on index:%d" , i );
                perror( (char*)strerror(errno));
            }
            
            /*
             * PENDING
             * 
             * ここのコードは、errnoをみてEINPROGRESSかどうかをしらべる必要があるが、
             * windowsでも動くコードにするにはどうするか悩むので置いてある
             *
             * また、UNIXでconnectをノンブロッキングにする方法がわからないので、
             * ブロックするままにしてある。
             */
            

        }
    }
    
}


/*
 * コネクションを閉じる。
 * char *ipaddr : 対象リモートホストのIPアドレス
 * unsigned short port : ポート。
 * 
 * return value:
 * -1 : 初期化されてないから何もしないぞ。
 * 
 * 接続を閉じるが、そのときにアドレスとポートを検索して、すでにひらいている
 * もののうち両方が一致するものだけ閉じる。
 */
int closeSyncroAddr( char *ipaddr , unsigned short port )
{
    int i;
    
	if( !initialized )return -1;

    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){

        }
    }
    

    return 0;
}

/*
 * ダブルバッファをdiffして、送信するべきものをさがしだし、送信する。
 *
 */
void doSyncro( void )
{
	if( !initialized )return;

}

/*
 *  ライブラリの動作を終了する。ふたたびはじめるには、initSyncroする必要があるのだ。
 *
 */
void finishSyncro( void )
{


	initialized = FALSE;

}

/*
 * データをついかする。
 * char *name : 変数名。文字列なら何でもいいが、
 * Cでつかえる名前そのままか、それをみじかくしたものが推奨される。
 * void *pointer : 変数の実体を指すポインタ。
 * TYPE type : 変数の型。 "int" , "short" , "char" , "string" の4種類のみ。
 * "string"の場合はcountの値が1以外だったらエラー。文字列の配列はサポートしない。
 * int diff : 差分送信タイプの場合1、そうでない場合0
 * int local : 1ならネットワークからの要求では値を変更しない。0ならする。
 *
 * それぞれのデータは、誰が最後にアクセスしたか、を記憶する。チート対策用 
 */
int addSyncroData(
 char *name , void *pointer , int count , char *type, int diff , int local )
{
	if( !initialized ) return -1;


    return 0;
}

/*
 * ログを出力する。
 *
 */
void printSyncroLog( char *out , int maxlen )
{
	if( !initialized ){
		strncpy( out , "not initialized" , maxlen );
		out[maxlen]='\0';
		return;
	}


}
/*
 * rhostのあきをさがす
 *
 * return value:
 * >=0 値
 * <0 Error
 */
static int findBlankConnection( void )
{
    int i;
    for(i=0;i<MAXCON;i++){
        if( con[i].use == 0 ){
            return i;
        }
    }
    return -1;
}


/*
 * 変数のデータをcharの配列にする。
 *
 * short i : 値
 * char *out : 出力
 * char *name : 変数名
 * return value
 * できたものの長さ(byte)
 * 
 */
int makeShortData( short i , char *out ,char *name )
{
    int nlen = strlen( name );
    
    strcpy( out , name );
    out[nlen]=0;
    out[nlen+1]='s';
    i = htons(i);
    bcopy( (char*)&i, &out[nlen+2] , 2 );

    return nlen + 4;
    
}
int makeShortDataDiff( short before , short after , char *out , char *name )
{
    int nlen = strlen( name );
    short i;
    
    strcpy( out , name );
    out[nlen]=0;
    out[nlen+1]='y';

    i = htons( ( after - before ) );
    bcopy( (char*)&i , &out[nlen+2] , 2 );

    return nlen + 4;
}

int makeShortArrayData( short *array , int sind , int eind , char*out , char *name )
{
    int i;
    char tmp[128];
    int silen , eilen;
    int nlen = strlen( name );

    strcpy( out , name );
    out[nlen]=0;
    sprintf( tmp , "%d" , sind );
    silen = strlen( tmp );
    strcpy( out + nlen + 1, tmp );
    sprintf( tmp , "%d" , eind );
    eilen = strlen(tmp );
    strcpy( out + nlen + 1 + silen + 1 , tmp );
    out[nlen+1+silen+1+eilen] = 'S';
    
    for(i=sind;i<=eind;i++){
        short s;
        s = htons( array[i] );
        bcopy( (char*)&s , out+nlen+1+silen+1+eilen+1+((i-sind)*2) , 2 );
    }
    return nlen+1+silen+1+eilen+1+((eind-sind+1)*2);
}


int makeTextData( char *text , char *out , char *name )
{
    char tmp[128];
    
    int nlen = strlen( name );
    int slen = strlen( text );
    int dlen;
    
    strcpy( out , name );
    out[nlen]=0;

    sprintf( tmp , "%d" , slen );
    dlen = strlen( tmp );
    
    strcpy( &out[nlen+1] , tmp );
    out[nlen+1+dlen]='T';
    strcpy( &out[nlen+1+dlen+1] , text );
    return nlen + 1 + dlen + 1 + slen;
}
