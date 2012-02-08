#include <stdio.h>

#define ON 1
#define OFF 0

/***********************************

  元のファイルのタグから、その内容に応じた文字列を書きこむ。

  char **params .... 引数列。最後の要素はNULLポインタでなければならな
  い。
  int sockfd .... 書きこむソケットバッファ

  すべて、パラメータの終端にヌル文字の入っていない文字列を使うので、
  比較にはstrncmpを使う必要があるので注意。
  
***********************************/
int contentsInserter( char **params , int sockfd )
{
	char *p;
	char *name=NULL, *param=NULL;


	/* 引数列には、少なくてもnameは必要である。 */
	while( p = *params++){
		if( strncmp( p,"name=" ,strlen("name=") )==0 ){
			name = p+strlen("name=");
		}
		if( strncmp( p , "param=" , strlen("param=") )== 0 ){
			param = p + strlen("param=");
		}
	}

	/* nameの種類ごとに、処理をふりわける。 */
	if( name!= NULL ){
		if( param != NULL ){
			/* paramが必要なもの */
		}

		/* paramが不要なもの、またはそれによって機能が変化するもの  */
		if( strncmp( name , "population" , strlen( "population" ) )== 0 ){
			if( param != NULL ){
				printf( "POPULATION TAG: %s\n" , param );
			} else {
				printf( "POPULATION TAG: asdfkuhasldkfj");
			}
		}
	}

	
}
/***********************************

  lifestormタグを理解して、そこに指定した文字列を挟みこむ。

  <lifestorm>タグは、ネストできない。' 'を含むことはできない。


  <lifestorm  name=population> -> 1046
  <lifestorm  name=playerimagetable col=5> -> <table...........
  
  テキストの中にタグがないかどうかを探すには、多少高速な方法を使わなければならない。

  char *buf ... 元の文字列
  char *tag ... 反応するタグ
  int sockfd ... ソケットバッファ


***********************************/
#define TAG_MAXPARAM 32
void parseHTMLTags( char *buf , char *tag , int sockfd )
{
	char *p;
	int tag_mode = OFF;		/* タグの中をパースしているか？ */
	int arg_mode = OFF;		/* 引数列の中をパースしているか？ */
	char *params[TAG_MAXPARAM];		/* 32引数まで */
	int pno = 0;
	int l = strlen( tag );
	p = buf;

	while( *p){
		register int c;
		c = *p;

		if( c == '<' && tag_mode == OFF && strncmp(p+1 , tag ,l)==0){
			/* lifestormタグがみつかったので、フラグをたてる。 */
			tag_mode = ON;
			arg_mode = OFF;
			pno = 0;
			params[pno] = NULL;
			p+=l;
			continue;
		}
		if( c == '>' && tag_mode == ON){
			p++;
			tag_mode = OFF;
			arg_mode = OFF;
			params[pno] = NULL;
			contentsInserter( params  , sockfd );
			continue;
		}
		if( tag_mode == ON && arg_mode == ON && c == ' ' ){
			arg_mode = OFF;
			p++;
			continue;
		}
		if( tag_mode == ON && arg_mode == OFF && isalpha(c)){
			arg_mode = ON;
			params[pno++] = p;
			if( pno == TAG_MAXPARAM ){
				/* 引数は32個までである。それ以降は無視する。 */
				pno--;
			}
			params[pno] = NULL;
			p++;
			continue;
		}
		if( tag_mode == ON ){
			p++;
			continue;
		}
		if( tag_mode == OFF ){
			printf("%c" , c );
			p++;
			continue;
		}

		/* not reached */
	}
	


}



main(int argc , char **argv)
{
	FILE *fp;
	char buf[10000];

	fp = fopen( argv[1] , "r");

	fread( buf , 1,10000,fp );

	parseHTMLTags(buf , "lifestorm " , 0 );
	
	fclose(fp);
	
	
}
