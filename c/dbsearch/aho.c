/*
  名前: UID というデータベースが1000万人分あるとする。
  10M人なので
  hogehoge 10084778
  という情報だと約20バイトでファイルにすると 200Mである。
  その中に、だいたい平均して  40人の同じ名前のユーザがいるとする。

  その検索をどうするのか？
  単語をRDB風データベース化するのは十分高速にできることが判明しているが、
  全データから検索するのはむつかしい。 test/rdb のコードを改良
  する。

  実際には1000万人(40MByte分)のアクティブUIDを管理する必要がある。

  uid
  nickへのindex
  emailへのindex
  ...へのindex
  というような複数の情報をもつ構造体を定義して、最低でもこの配列は
  オンメモリに持っておく必要がある。で、全部検索する場合はこの
  配列を全部検索する必要がある。ように思える。
  問題をnickだけにしぼってかんがえる。

  理想的には、同じnickを持つ人がリンクでつながっていて、
  hash -> そのリンクのトップ -> ループ で検索できたら50人いても50ループ
  で済む。 ここまでいきたい。

  Nickとは単語なので、単語エントリがUIDリンクへのトップになっていると
  かなりよい。
  First-name も last-name も、単語であり、最終的には UID を検索したいので、
  UIDリンクを定義すれば、そのリンクを検索していくだけでUIDを発見する
  ことが可能っぽい。
  unsigned int UID
  int type
  unsigned int next
  という12バイトのリンクをつくればよい。1000万人で120MBとなる。
  
 */
/*
  単語の回数数えるのを今度は、
  ハッシュ -> リスト でやる
  
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>


typedef enum{
    NICK = 1,
    EMAIL=2,
    FNAME=3,
    LNAME=4,
} DBTYPE;

typedef struct uent_tag
{
    unsigned int uid;
    struct uent_tag *next;
} uent;

#define MAXUENT ( 1024 * 1024 )     /* メモリがすくないのででかくためせない */
uent ubuf[MAXUENT];


typedef struct apent_tag
{
    char *p;        /* dbbuf のある位置へのポインタ */
    DBTYPE type;
    struct apent_tag *next;
    uent *utop;
} apent;


#define HASHCOUNT ( 500000  )      /* ハッシュの値の種類。大きくすると早く
                                 なるだろう。5万でよいことが判明 */
apent *aptop[HASHCOUNT];


#define DBSIZE ( 1024 * 1024 )

/*単語の本体を入れておくバッファ。だいたい1単語あたり8バイトか。 */
char dbbuf[DBSIZE * 8 ];
apent apbuf[DBSIZE];        /* 単語の個数だけ */


int call_count = 0;
time_t start_tm;

/*  次のバッファをもらう。長さだけ。文字列を保存するので
    strlen + 1 だけ引数に与えること。
 */
static char *nexp = dbbuf;
static int lastapbufind = 0;
apent *getnextapent( int len )
{
    apent *a = & apbuf[lastapbufind++];

    a->p = nexp;
    nexp += len;

    return a;
}

unsigned int hashpjw( char* s )
{
    char *p;
    unsigned int h= 0 ,g;
    for( p = s ; *p ; p ++ ){
        h = ( h<< 4 ) + (*p);
        if( (g = h & 0xf0000000) != 0){
            h = h ^ (g>>24);
            h = h ^ g;
        }
    }
    return h;
}

int last_uent_index = 0;
uent *getnewuent( void )
{
//    fprintf( stderr ,"getnewuent:%d\n",last_uent_index );
    return & ubuf[ last_uent_index ++ ];
}


/*
  単語を登録する 
 */
int htop_usage = 0;
int add_u( unsigned int uid , char *nk, char *fn , char *ln , char *em  )
{
    uent *ue;

    /* Nickname は絶対必要 */
    if( nk[0] == 0 ) return -1;


    
    ue = getnewuent();
    ue->uid = uid;
    ue->next = NULL;

    add_word( nk , NICK , ue );
    
    if( fn[0] ){
        ue = getnewuent();
        ue->uid = uid;
        ue->next = NULL;
        add_word( fn , FNAME , ue);
    }

    if( ln[0] ){
        ue = getnewuent();
        ue->uid = uid;
        ue->next = NULL;
        add_word( ln , LNAME , ue );
    }

    if( em[0] ){
        ue = getnewuent();
        ue->uid = uid;
        ue->next = NULL;
        add_word( em ,EMAIL , ue);
    }
}
int ii_max = 1000;
int add_word( char *w , DBTYPE type , uent *up )
{
    unsigned int h = hashpjw( w ) % HASHCOUNT;
    apent *now_top_ap;

    
    call_count ++;
    if( ( call_count % 5000 ) == 0 ){

//    {    
        fprintf( stderr ,"C:%d Usage:%d %d\n",call_count , htop_usage ,
                 time(NULL) - start_tm );
    }

    if( aptop[h] == NULL ){
        /* まだこのハッシュ値をもつ単語は登録されてないので
           登録する */
        int l = strlen( w );
        apent *ape = getnextapent( l + 1);
        aptop[h] = ape;
        ape->next = NULL;
        ape->type = type;
        memcpy( ape->p , w , l + 1 );
        htop_usage ++;

        /* はじめての初期化なので  utop を初期化 */
        ape->utop = up;
        return;
    }

    /* 1個以上登録されてたのでリンクをたどる */

    now_top_ap = aptop[h];
    for(;;){
        /* タイプと文字列が完全に一致する場合だけ */
        if( type == now_top_ap->type &&
            strcmp( now_top_ap->p , w ) == 0 ){
            /* UIDエントリリストの最後に */
            uent *ut;
            int i=0;
            ut = now_top_ap->utop;            
            for(;;){
                i++;
                if(ut->next == NULL){
                    ut->next = up;
//                    fprintf( stderr , "appended\n");
                    break;
                }
                ut = ut->next;
            }

            if( i > ii_max ){
                ii_max = i;
                fprintf( stderr , "uent appnd i:%d H:%d %s\n",i,(int)h,w);
            }
            return;
        } else {
            if( now_top_ap->next == NULL ){
                /* リンクの最後まできたのでこの単語は登録されて
                   いなかったのだ。 */
                int l = strlen( w );                
                apent *ape = getnextapent( l + 1 );
                
                now_top_ap->next = ape;
                ape->next = NULL;
                ape->type = type;
                memcpy( ape->p , w , l + 1 );
                ape->utop = up;
                return;
            } else {
                now_top_ap = now_top_ap->next;
            }
        }
    }
    
}

/*
  文字列とタイプから検索する
 */  
void search( DBTYPE type , char *s )
{
    unsigned int h = hashpjw( s ) % HASHCOUNT;
    apent *now_top;

    if( aptop[h] == NULL ){
        fprintf( stderr, "no user found\n");
        return;
    }

    /* 検索。 */
    now_top = aptop[h];
    for(;;){
        if( now_top == NULL ) break;
        if( type == now_top->type &&
            strcmp( s , now_top->p ) == 0 ){
            /* found */
            uent *utop = now_top->utop;
            for(;;){
                if( utop == NULL ){
                    break;
                } else {
                    fprintf( stderr , "%u\n", utop->uid );
                    utop = utop->next;
                }
            }
        }
        now_top = now_top->next;
    }
}

                


void rout( char*nm )
{
    int i;
    FILE *fp=fopen( nm , "w");
    for(i=0;i<DBSIZE;i++){
        if( apbuf[i].p ){
            uent *u;
            fprintf( fp, "%s TYPE:%d U:",
                     apbuf[i].p , apbuf[i].type );
            u = apbuf[i].utop;
            for(;;){
                if(u==NULL)break;
                fprintf( fp , " %u" , u->uid );
                u = u->next;
            }
            fprintf( fp , "\n");
        }
    }
    fclose(fp);
//    fprintf( stderr , "CALL:%d\n", call_count);
}

unsigned int ahouid = 1000000;

void r( char *aho)
{
    char fuck[10000];
    char fuck2[10000];
    char fuck3[10000];
    char fuck4[10000];
    
    FILE*fp=fopen(aho,"r");
        
    while(1){
        fuck[0] = fuck2[0] = fuck3[0] = fuck4[0] = 0;

        if(fgets(fuck,sizeof(fuck),fp)==0)break;
/*        if(fgets(fuck2,sizeof(fuck2),fp)==0)break;
        if(fgets(fuck3,sizeof(fuck3),fp)==0)break;
        if(fgets(fuck4,sizeof(fuck4),fp)==0)break;*/
        fuck[strlen(fuck)-1]=0;
/*        fuck2[strlen(fuck2)-1]=0;
        fuck3[strlen(fuck3)-1]=0;
        fuck4[strlen(fuck4)-1]=0;*/

//        fprintf( stderr, "%s\n",fuck);
//        add_u( ahouid++ , fuck ,fuck2,fuck3,fuck4);
        add_u( ahouid++ , fuck ,"","","");
    }
    fclose(fp);


        
}

void siginth(int a)
{
    exit(0);
}
int main( int argc, char **argv )
{
    
    if( argc == 1 ){
        fprintf(stderr ,"usage: infile outfile]\n" );
        exit(0);
    }
    
    time ( &start_tm );
    signal( SIGINT,siginth );

    r(argv[1]);

    rout(argv[2]);

    
//    search( EMAIL , "EEnm" );
}
