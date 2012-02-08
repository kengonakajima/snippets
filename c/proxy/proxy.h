#ifndef _PROXY_H_
#define _PROXY_H_



/*
 * マシンに一切依存しない(32ビットネイティブであることは、前提。)のコード
 * 群。
 *
 */


/* プロクシを起動する。この関数は終了するとき以外かえってこないー
   正常終了したら0、そじゃないばあいは1をかえしましょう。
   この関数はかならず定義しなければならないのだ。
 */
int startProxy( int argc , char **argv );

/* HTTP 関連の、マシン依存しない処理 */
void getHTTPRequest( char *src , char *out , int maxlen );
void getHTTPRequestURL( char *src , char *out , int maxlen );
void getHTTPRequestMethod( char *src , char *out , int maxlen );
void getHTTPRequestHostname( char *src , char *out , int maxlen );
void getHTTPRequestScheme(  char *src , char *out , int maxlen );
void getHTTPRequestFilename( char *src , char *out , int maxlen );
void getHTTPHost( char *src , char *out, int maxlen );

/* のぶおかのルーチン。マシン依存しない処理。 */
int getStringFromIndexWithDelim( char* src ,char* delim ,int index,
                                 char* buf , int buflen);




#endif
