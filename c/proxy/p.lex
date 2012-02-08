%{  

#include "proxy.h"  /* このコードの中で定義する関数は、proxy.hにかく。 */
    
    /* p 用の flex コード。
       なんでも処理しまっせー。

     */

#undef YY_INPUT
#define YY_INPUT( b , r , ms ) ( r = yyinput(b,ms))

typedef enum
{
    R_HTTP_REQUEST,         /* HTTP の最初の GET / HTTP/1.0 の行 */
    R_HTTP_METHOD,          /* HTTPのGET/PUT/POSTにまっちしてほしい。 */
} REGEXP;

REGEXP scantype;        /* なにをスキャンしましょう？ */
int scancount;          /* いくつめでしょう？ */
int current_scancount;   
%}    

HTTPMETHOD "GET"|"POST"|"PUT"
HTTPVERSION "HTTP/1."[01]

%%

[\t ]+         { ;}

^{HTTPMETHOD}       {  printf( "AAAAA\n");answer(R_HTTP_METHOD);  }

{HTTPMETHOD}" "(.+)" "{HTTPVERSION}\n  { answer(R_HTTP_REQUEST); }


. { printf( "CHAR:[%s]\n" , yytext ); }

%%

char *inputbuf;     /* 大元のバッファ */
char *inputbuf_p;   /* 大元のバッファのどこかをさすpointer。 */
int inputbufmax;     /* おおもとのバッファの使用量 */
char *answerstring;       /* 答えをいれるポインタ */
int answerstring_max;     /* こたえの最大長さ */


#define SETANS( out , len ) answerstring = (out); answerstring_max = (len);\
    answerstring[0] = '\0';   

#define SETSCAN( type , c ) scantype = (type); scancount = (c);\
    current_scancount = 0;
    
#define SETINPUT( src ) inputbuf=inputbuf_p=(src);inputbufmax=strlen(src);

#define LEX()       yylex();


/*
 * ヘッダ全体から、HTTPのリクエスト行をとりだす。
 */
void getHTTPRequest( char *src , char *out , int maxlen )
{
    SETANS( out, maxlen );
    SETSCAN(R_HTTP_REQUEST , 1 );
    SETINPUT( src );
    LEX();

    
    
}

/*
 * yytext に抽出された文字列から、答えをだしたい文字列
 * p_scan_answer にコピーする。
 * あるパターンにマッチしたらこれをよぶだけ。
 *
 * 何回もよびだされたときの対処を考えましょう。
 *
 */
void answer( int rtype )
{
    if( scantype != rtype ) return;
    
    current_scancount++;
    if( current_scancount < scancount )return;
    
    if( answerstring){
        int i;
        for(i=0;i<answerstring_max-1;i++){
            answerstring[i] = yytext[i];
            if( yytext[i] == '\0' )break;
        }
        answerstring[i] = '\0';     /* おわらせないとね。 */
    }
}

/*
 * 何ばいとコピーしたかをかえすのだ。
 *
 */
int yyinput( char *buf , int max_size )
{
    
    int n;
    
    /* 処理されたら勝手に縮むのでもいかな */
    int last = inputbufmax - ( inputbuf_p - inputbuf );
    if( last < max_size ){
        memcpy( buf , inputbuf_p , last );
        inputbuf_p += last;
        return last;
    } else if( last == 0 ){
        return 0;
    } else {
        memcpy( buf , inputbuf_p , max_size );
        inputbuf_p += max_size;
        return max_size;
    }
}


