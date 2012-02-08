#include <stdio.h>


char data[] = " \t 100  200     \t     400";

void easyGetTokenFromString( char *src , int count , char*output , int len );
void strcpysafe( char* dest ,size_t n ,const char* src );
char* strncpy2( char* dest, const char* src, size_t n );

main()
{
    char out[100];
    int a,b,c;
    

    easyGetTokenFromString( data ,1, out , sizeof(out) );
    a = atoi( out );
    printf("hoge:'%s'\n" , out );
    easyGetTokenFromString( data , 2, out , sizeof(out) );
    b = atoi(out);
    printf("hoge:'%s'\n" , out );
    easyGetTokenFromString( data , 3, out , sizeof(out) );
    c=atoi( out);
    printf("hoge:'%s'\n" , out );
    printf( "%d %d %d\n" , a ,b,c );
}

/*
 * 設定ファイル読み用の、split.
 * エスケープには対応していない。
 * タブとかスペースがいくつあってもそれがデリミタとなる。
 * 設定ファイルはよく、きれいにならべるためにスペースとかタブを入れまくるが、
 * それでもちゃんと読みこめるようにするためのもの。
 * char *src : 元の文字列
 * int count : 最初のトークンなら1 それ以降2、3、4..
 * char *output : 出力
 * int len : outputにコピー可能な長さ
 */
void easyGetTokenFromString( char *src , int count , char*output , int len )
{
    int i;
    int counter = 0;
    
    if( len <= 0 )return;

    
#define ISSPACETAB( c )   ( (c) == ' ' || (c) == '\t' )
    
    for(i=0;;i++){
        if( src[i]=='\0'){
            output[0] = '\0';
            return;
        }
        if( i > 0 && ! ISSPACETAB( src[i-1] ) &&
            ! ISSPACETAB(  src[i] ) ){
            continue;
        }
        
        if( ! ISSPACETAB( src[i]) ){
            counter++;
            if( counter == count){
                /* copy it */
                int j;
                for(j=0;j<len-1;j++){
                    if( src[i+j] == '\0' ||
                        ISSPACETAB( src[i+j] ) ){
                        break;
                    }
                    output[j]=src[i+j];
                }
                output[j]='\0';
                return;
                
            }
        }

    }
    
}


void strcpysafe( char* dest ,size_t n ,const char* src )
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


/*------------------------------------------------------------
 * EUCの2バイト目だけをコピーしないstrncpy
 * 引数
 *  dest        char*   コピー先
 *  src         char*   ソース
 *  n           size_t  長さ
 * 返り値
 *  dest
 ------------------------------------------------------------*/
char* strncpy2( char* dest, const char* src, size_t n )
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

