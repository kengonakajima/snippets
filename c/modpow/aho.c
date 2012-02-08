/*
  GNU MP にたよらない modpow プログラム

  Diffie-Hellman アルゴリズムで用いるのに必要な機能だけでよい。
  それはかけ算と、わり算の余りと、代入だけでよい。

  y = y ^ e mod n

  step-1 e を2進数配列で表現 (e_k-1,e_k-2, ... e_1, e_0 )
  step-2 y=1 とする。 y は解を格納するための変数である。
  step-3 i=i-1 から 0まで、 y = y × y mod n を繰りかえす
  step-4 e_i = 1 になったら、 y = x × y mod n
  である。

  MP_MAXDIGITを越えたときのエラー処理はちゃんとやること。
  これで VCEが落ちたら話にならんので。

  10進数で保存するのだよ。
  
 */
#include <stdio.h>

#define MP_MAXDIGIT 4096
typedef struct
{
    char d[MP_MAXDIGIT];     /* 16進数。0からfの値をいれよう。
                              小文字でも大文字でもいい。
                              d[0]が1の桁で、d[1]が10の桁。
                              ASCIIではないので注意 */
    int l;
} mp_t;



/* さかさまmemcpy */
static void
mp_rmemcpy( char *dest, char *src, int len )
{
    int i;
    for(i=0; i < len; i++ ){
        dest[ len - i - 1 ] = src[ i ];
    }
}



static int
mp_hatoi( int c )
{
    switch(c){
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': return 10;
    case 'b': return 11;
    case 'c': return 12;
    case 'd': return 13;
    case 'e': return 14;
    case 'f': return 15;
    case 'A': return 10;
    case 'B': return 11;
    case 'C': return 12;
    case 'D': return 13;
    case 'E': return 14;
    case 'F': return 15;        
    }
    return -1;
}
static int
mp_hitoa( int c )
{
    switch(c){
      case 0: return '0';
      case 1: return '1';
      case 2: return '2';
      case 3: return '3';
      case 4: return '4';
      case 5: return '5';
      case 6: return '6';
      case 7: return '7';
      case 8: return '8';
      case 9: return '9';
      case 10: return 'A';
      case 11: return 'B';
      case 12: return 'C';
      case 13: return 'D';
      case 14: return 'E';
      case 15: return 'F';
      default: return -1;
    }
}


/*
  16進数の文字列をもとに、数値をつくったり戻したり
  エラーテストしない(staticなので)
 */
static int
mp_init_set_hexstr( mp_t *m , char *hexstr )
{
    int i;
    int l = strlen(hexstr );

    if( l > sizeof( m->d ) ) return -1;
    
    memset( m->d , 0 , sizeof( m->d ));

    
    for(i=l-1 ; i >=0; i-- ){
        fprintf( stderr, "HOGE:%d\n", hexstr[i] );
        m->d[i] = mp_hatoi( hexstr[i] );
        fprintf( stderr, "HOGED:%d\n", m->d[i] );
    }
    m->d[l]=0;
    m->l = l;
    return 0;
}


static int
mp_get_hexstr( mp_t *m , char *hexstrout , int len )
{
    int i;
    if( (m->l+1) >= len ) return -1;
    if( len < 3 )return -1;
    
    for(i=m->l-1;i>=0;i--){
        fprintf(stderr ,"AHO: %d\n", m->d[i] );
        hexstrout[i] = mp_hitoa( m->d[i] );
    }
    hexstrout[m->l]=0;
    return 0;
}

/*
   mp_t を2進数展開する(0/1)
   入力は16進数なので、1桁あたり4ビットの情報量をもつ。
   文字が不正な場合とバッファが足りない場合は負を返す。成功したら0。
   
   char *dest :出力用バッファ
   int *len : 値結果引数。バッファの長さを受けとり、結果の長さをかえす。
   len は、 strlen(v->d) * 4 の長さが必要である。

   
 */

static int
mp_deploy_binary( char *dest, int *len, mp_t *v )
{
    char aho[16][4] = {
            { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,1 },
            { 0,1,0,0 }, { 0,1,0,1 }, { 0,1,1,0 }, { 0,1,1,1 },
            { 1,0,0,0 }, { 1,0,0,1 }, { 1,0,1,0 }, { 1,0,1,1 },
            { 1,1,0,0 }, { 1,1,0,1 }, { 1,1,1,0 }, { 1,1,1,1 } };

    int i;
    int buflen = *len;
    int vl = strlen( v->d );

    if( buflen < ( vl*4 ) ){
        return -1;
    }

    for( i = 0; i < vl ; i++ ){
        int intval = mp_hatoi( v->d[i] );
        /* 入力が不正である。 */
        if( intval < 0 ){
            *len = 0;
            return -1;
        }
        dest[ i*4    ] = aho[intval][0];
        dest[ i*4 +1 ] = aho[intval][1];
        dest[ i*4 +2 ] = aho[intval][2];
        dest[ i*4 +3 ] = aho[intval][3];
    }
    *len = vl*4;
    return 0;
}

/*
  指定した数値の累乗

 */
static void
mp_pow( mp_t *dest, mp_t *p , mp_t *n )
{
    mp_t w;

    /* w を1とする */
    mp_init_set_hexstr( &w, "1" );

    /* nを2進数展開する */
}

/*
  代入をする。単なる文字列コピーをする。
 */
static void
mp_subst( mp_t *dest , mp_t *src )
{
    memcpy( dest->d , src->d , src->l );
}

/*
  足し算をする
 */
static void
mp_add( mp_t *dest, mp_t *src , mp_t *add )
{
    int i;
    int srcl = strlen( src->d );
    int addl = strlen( add->d );

    /* d[0]が1の桁であることに注目ね。
     srcも addも両方が\0になるまでやる*/
    for( i = 0; i< (MP_MAXDIGIT-2) ; i++ ){
        int k;
        if( src->d[i] == '\0' && add->d[i] == '\0' ){
            return;
        }
    }
}

/*
  引き算 をする 。負になる場合は途中でやめて -1を返すのでよい。
 */
static int
mp_sub( mp_t *dest, mp_t *src, mp_t *sub )
{
}


    
/*
  かけ算をする
 */
static int
mp_mul( mp_t *dest, mp_t *number, mp_t *multiplier )
{
}

/*
  余りをもとめる。
*/
static int
mp_mod( mp_t *dest, mp_t *number, mp_t *divider )
{
}

/*
  累乗剰余を求める(唯一のexported関数)
 */
int
vce_mp_powm( mp_t *dest, mp_t *p , mp_t *n, mp_t *m )
{
}


int
main( int argc, char **argv )
{
    char a[] = "1234";
    char b[1110];
    char n[] = "";

    mp_t t;
    
    mp_init_set_hexstr( &t, "123876879aacc9fffaaaac9f8af98cc9afa98fc9a89f8c9a898cf98ca9f8ac989fc8a9f8af98a9fc89af8c9f8af9c899af8a9acc8f9a8a" );

    mp_get_hexstr( &t, b , sizeof( b ));

    printf( "Retrieved:%s\n", b );
}

