#include <sys/time.h>
#include <stdio.h>

/* decoder フリーウェアから持ってきた
   バイナリ用なので、最後に0をいれたりしないことに注目。
   int *で長さを返す。



   ほとんどがエスケープされているとき
   142byte : 2万回/sec      : 3.8MB/sec
   14byte : 10万回/sec      : 1.4MB/sec

   ぜんぶASCIIのとき
   140byte : 4万回/sec      : 7.6MB/sec
   14byte : 25万回/sec      : 3.5MB/sec


   これを速くする
   
 */
void
simple_de_escape( char *src, char *out, int outlen , int *actualout )
{
    int i=0 , srci=0;
    for(;;){
        int c;
        if( src[srci] == 0 )break;
        if( src[srci] == '%' ){
            char up ,low;
            if( (!src[srci+1])  || (!src[srci+2]) )break;
            up = src[srci+1];
            low = src[srci+2];
#define ISDIG(x) ( (x) >= '0' && (x) <= 9 )
            
            if( ISDIG(up) ) up -= '0'; else up = up - 'a' + 10;
            if( ISDIG(low) ) low -= '0'; else low = low - 'a' + 10 ;
            c = up * 16 + low;
            srci+=2;
        } else {
            c = src[srci];
        }
        out[i++] = c;
        srci++;
    }
    if( actualout ) *actualout = i;
}
    

char *
simple_escape( char *src, int srclen, char *out, int outlen )
{
    char enc_outs[16] = { '0','1','2','3','4','5','6','7',
                          '8','9','a','b','c','d','e','f' };
    int srci=0,i=0;
    
    if(outlen < 1 )return out;
    if( srclen == 0 ) return out;
#define ENCO(x)     ( (x)<=9 ? '0'+(x) : 'a'+(x)-10 )
    while(1){
        int c = src[srci++];
        if( ( c <= 0x1f ) ||
            ( c == 0x20 ) ||  /* space char */
            ( c == 0x25 ) ||
            ( c == 0x3a ) ||
            ( c >= 0x7f )  ){
            if( i >= (outlen-3) ){
                *out=0;
                break;
            } 
            *(out++) = '%';
//            *(out++) = enc_outs[ c / 16 ];
//           *(out++) = enc_outs[ c % 16 ];
            *(out++) = ENCO( c>>4);   // すこし改善
            *(out++) = ENCO( c&15);
            i+=3;
        } else {
            if( i >= (outlen-1) ){
                out[i] = 0;
                break;
            }
            out[i++] = c;
        }
        if( srci == srclen ){
            if( i <= ( outlen-1)){
                out[i]=0;
                break;
            }  else {
                out[outlen-1]=0;
                break;
            }
        }
    }
    return out;
}



#define ISDELIM( c )   ( (c) == ' ' || (c) == '\t' )
#define ESCAPER '\\'


/* FIXME: spaghetti or ramen, isn't it? (ringo) */
int
find_tk_start( char *src, int count )
{
    int i , tk_start = -1, c = 0;
    int in_esc = 0;    

    for(i=0;;i++){
        if( src[i]==0 )break;
        if( src[i] == ESCAPER ){
            if( (i>0) && ISDELIM(src[i-1] )  ){
                c++;
                if( c == count ){
                    tk_start = i;
                    break;
                }
            }
            in_esc = 1;
            continue;
        }
        if( in_esc
            && ( src[i] == ' ' || src[i] == 't'
                 || src[i] == ESCAPER || src[i] == 'n' ) ){
            in_esc = 0;
            continue;
        }
        if( !ISDELIM(src[i]) ){
            if( i == 0 ){
                c++;
            } else if( ISDELIM(src[i-1]) ){
                if( (i-2)>=0 && src[i-2] != ESCAPER ){
                    c++;
                }
            }
            if( c == count ){
                tk_start = i;
                break;
            }
        }
    }

    return tk_start;
}

void
get_simple_token( char *src, int count, char*out, int len )
{
    int i, tk_start, c;

    tk_start = find_tk_start( src, count );
    
    if( tk_start < 0 ){
        snprintf( out, len, "" );
        return;
    }

    c = 0; 
    for(i=tk_start;;i++){
        if( src[i] == 0 ){
            out[c]=0;
            break;
        }
        if( src[i] == ESCAPER
            && ( src[i+1] == ' ' || src[i+1] == 't'
                 || src[i+1] == ESCAPER || src[i+1] == 'n' ) ){
            if( src[i+1] == ' '){ out[c++] = ' '; }
            else if( src[i+1] == 't' ){ out[c++] = '\t'; }
            else if( src[i+1] == ESCAPER ){ out[c++] = '\\'; }
            else if( src[i+1] == 'n' ){ out[c++] = '\n'; }            
            i++;
            if( c == (len-1) ){ out[c]=0; break; }
            continue;
        }
        if( ISDELIM(src[i])){
            out[c]=0;
            break;
        }
        out[c++] = src[i];
        if( c == (len-1) ){ out[c]=0; break; }        
    }
}



int
main( int argc, char ** argv )
{
    int i , aho , cpl=1;

    char fuck[1000] , outy[1000];
    struct timeval t,et;
    snprintf(fuck,sizeof(fuck), "asd asd d as99 asdfasudfasdfasd fas df asd fas df asd f asd f asdf asd f asd f as df as df as df as dfasdfasdf asd fasdfawieasdfasdf asd fas dfas df as df asd f asdf s df asd f asd fas df as df asd fas df as df asd fa sdf as df s dfa sd fasdf asdf asdfasdfas df asd fas df as df asd fas df asd fa sdf asd f as df asd fasdfa sdf asdof asdkhflaksd hflkasd fkljash dlkfa sdkfj askldjfh askljdhf laksjdh flaksjdh flkasjhd flkjash dlkfjha sldkjfh alskdjhf laksjdh flkajshd flkajshdlfkj alsdkjfh laksdjhf laksjdhf lajksh dflkjahs dlkfjah sldkjfh aldkj flakdjh flka sjdhlfajkhdlfkhja dlkfjah sldkfjha lsdkjfalskdjhf laksjdhflaksjdhflkasjhd flkahjsdlfkjah sdlkfj ahlskd fhlaskdjhf lakjsdhf lkasdjh flaksjdh flkasjd flkajsh dlkfahj sldkfjha lsdkj fhlaksdjhflaksdj fasdkf ahfsdlkj lsdkjh klas lkadhj lkasjh klasdh klas dklas klas lkas ldk sal jhalsdk jhlas djflaksdjh flkasjd flakjsd hflaksjdh flajksdh lfjkahd lfkjahdlfkjashdlkfjahsldkjfalsdkj laksdj ldjlakj lkjlakjs lad asl d" );

    printf( "strlen:%d\n", strlen( fuck ) );

#define L 400000

    for(cpl=1; cpl<1000 ; cpl++){
        if( cpl > 256 ) cpl += 3;

        gettimeofday ( & t, NULL );        
        for(i=0;i<L;i++){
            int a;
            simple_escape( fuck, cpl, outy, sizeof( outy ));
            simple_de_escape( outy, fuck, sizeof( fuck ), &a );
        }
        gettimeofday( & et, NULL );
        {
            int q = ( et.tv_sec - t.tv_sec )* 1000000 +
                (et.tv_usec - t.tv_usec ) ;

            fprintf( stderr, "%d %f\n",
                     cpl, (double)L/(double)((double)q/1000000) );
        }
    }
}
