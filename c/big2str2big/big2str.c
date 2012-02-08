#include <stdio.h>

#define MAXDIGITS 10000

typedef struct _big
{
    char sign;
    long len;
    unsigned short digits[MAXDIGITS];
} big;


/*
  正の数だけ
 */

#define CHAR_BIT 8
#define BITSPERDIG (sizeof(short)*CHAR_BIT)
#define BIGRAD ( 1L << BITSPERDIG)
#define BIGLO(x)  ((unsigned short)((x) & (BIGRAD-1)))
#define RSHIFT(x,y)     ((x)>>(int)(y))
#define BIGDN(x)  RSHIFT(x,BITSPERDIG)
#define BIGUP(x) ((unsigned long)(x) << BITSPERDIG )

static big *
bignorm( big *x )
{
    long len = x->len;
    unsigned short *ds = x->digits;

    while(len-- && !ds[len] );
    x->len = ++len;
}

static void
str2big( char *str, int base , big *z )
{
    char c;
    int i,blen=1 , num;
    unsigned short *zds;

    memset( z->digits, 0 , sizeof( z->digits )) ;

    zds = z->digits;
    z->len = strlen(str)*4;
        
    while( c = *str++ ){
        switch(c){
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            c = c - '0';
            break;
          case 'a': case 'b': case 'c':
          case 'd': case 'e': case 'f':
            c = c - 'a' + 10;
            break;
          case 'A': case 'B': case 'C':
          case 'D': case 'E': case 'F':
            c = c - 'A' + 10;
            break;
          default:
            c = base;
        }
        if( c>= base )break;
        i=0;
        num=c;
        for(;;){
            while( i < blen ){
                num += zds[i]*base;
                zds[i++]=BIGLO(num);
                num = BIGDN(num);
            }
            if( num ){
                blen++;
                continue;
            }
            break;
        }
    }
    bignorm(z);
}

/*
  正の16進数専用文字列変換ルーチン
*/
static char hexmap[] = "0123456789abcdef";

void
big2hexstr( char *str, int len , big *x )
{
    big t;
    int i = x->len;
    int j = (sizeof(unsigned short)/sizeof(char)*CHAR_BIT*i)+2;
    unsigned long hbase = 0x10000;
    unsigned short *ds;
    char c;
    int slen;


    memcpy( &t, x, sizeof( big ));
    ds = t.digits;
    slen = j;

    while( i && j ) {
        long k = i;
        unsigned long num = 0;
        while (k--) {
            num = BIGUP(num) + ds[k];
            ds[k] = (unsigned short)( num / hbase );
            num %= hbase;
        }
        if (ds[i-1] == 0) i--;
        k=4;
        while(k--){
            c = (char)( num % 16 );
            str[--j] = hexmap[(int)c];
            num /= 16;
            if( i == 0 && num == 0 )break;
        }
    }
    while( str[j] == '0' ) j++;
    memmove( str, str+j, slen-j);
    
}

int
main()
{
    big z;
    char fuck[19011];
    
    str2big( "1234567899829487239487239487293487293874923887697364971239471639478162394781937849178236491762349761239478619378469172364917862349716239478613476123784691723649127364987123694817234891638961238461238469123846912378491278346912736491782639481726349871634897138619237846178236491783649178236491786234917234978123489716238612384691238649127836498126734" , 10 , &z);

    big2hexstr( fuck, sizeof(fuck), &z );

    fprintf( stderr, "FUCK:%s\n", fuck );
    
    
}
