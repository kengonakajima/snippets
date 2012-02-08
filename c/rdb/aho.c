/*
  単語の回数数えるのを今度は、
  ハッシュ -> リスト でやる
  
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>



typedef struct apent_tag
{
    char *p;        /* dbbuf のある位置へのポインタ */
    int count;
    int hash;
    struct apent_tag *next;
} apent;


#define HASHCOUNT ( 500000  )      /* ハッシュの値の種類。大きくすると早く
                                 なるだろう
                                 1000万：スワップしまくってだめ
                                 400万： 26秒。つかいきらない
                                 200万： 25秒 つかいきらない
                                 100万： 23秒。つかいきらない
                                 10万： 22秒。つかいきらない(55297)
                                 1万： 27秒つかいきらない(9650)
                                 5000:  29sec    つかいきる
                                 2500:  31秒  つかいきる
                                 1000:  47秒  つかいきった

                                 drinkだとちょうど2倍になる。 11秒。
                                 100万： 10sec hashなし空ループ 6秒
                                 200万： 10sec
                                 メモリがおおいので多くしてもあまり遅く
                                 ならん。

                                 
                                 */
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

unsigned int hashpjw ( char* s )
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
/*
  単語を登録する 
 */
int htop_usage = 0;
int add_word( char *w )
{
    unsigned int h = hashpjw( w ) % HASHCOUNT;
    apent *now_top_ap;

    call_count ++;
    if( ( call_count % 5000 ) == 0 ){
        fprintf( stderr ,"C:%d Usage:%d %d\n",call_count , htop_usage ,
                 time(NULL) - start_tm );
    }

    if( aptop[h] == NULL ){
        /* まだこのハッシュ値をもつ単語は登録されてないので
           登録する */
        int l = strlen( w );
        apent *ape = getnextapent( l + 1);
        aptop[h] = ape;
        ape->count = 1;
        ape->next = NULL;
        ape->hash = h;
        memcpy( ape->p , w , l + 1 );
//        fprintf( stderr , "%d new [%s]\n", call_count , w );
        htop_usage ++;
        return;
    }

    /* 1個以上登録されてたのでリンクをたどる */

    now_top_ap = aptop[h];
    for(;;){
        if( now_top_ap->hash == h && strcmp( now_top_ap->p , w ) == 0 ){
            now_top_ap->count ++;
//     fprintf( stderr, "add count to %d: [%s]\n", now_top_ap->count,w );
            return;
        } else {
            if( now_top_ap->next == NULL ){
                /* リンクの最後まできたのでこの単語は登録されて
                   いなかったのだ。 */
                int l = strlen( w );                
                apent *ape = getnextapent( l + 1 );
                now_top_ap->next = ape;
                ape->next = NULL;
                memcpy( ape->p , w , l + 1 );
                ape->count = 1;
                ape->hash = h;
            } else {
                now_top_ap = now_top_ap->next;
            }
        }
    }

    
}






    


void rout( char*nm )
{
    int i;
    FILE *fp=fopen( nm , "w");
    for(i=0;i<DBSIZE;i++){
        if( apbuf[i].p ){
            fprintf( fp, "%s %d\n", apbuf[i].p , apbuf[i].count );
        }
    }
    fclose(fp);
    fprintf( stderr , "CALL:%d\n", call_count);
}

void r( char *aho)
{
    char fuck[10000];
    FILE*fp=fopen(aho,"r");
    while(fgets(fuck,sizeof(fuck),fp)){
        fuck[strlen(fuck)-1]=0;
        add_word( fuck );
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

}


