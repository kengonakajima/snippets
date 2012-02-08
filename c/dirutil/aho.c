
#include <strings.h>
#include <sys/types.h>
#include <time.h>
#include <sys/dirent.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

/******************************
  ディレクトリに含まれるすべてのファイルを再帰的に探しだしてリスト
  を作る。

  char *dir ... ディレクトリのパス(フルパス)
  char *flist ... ファイルリストを入れていく文字列
  int flistlen ... flist のサイズ
  int c ... トークン切りの文字
  
  この関数は、flistの最大値を越えない範囲で、flistに名前へのポインタを加えていく。

  flistは、
******************************/
int getAllFilesUnderDirectory( char *dir , char *flist ,int flistlen , int c)
{
	DIR *d;
	char fullpath[1024];

	d = opendir( dir );
	if( d == NULL) return -1;
	
	while(1){
		struct dirent *dent;
		struct stat statbuf;
		DIR *dtmp;
		
		dent = readdir( d );
		if( dent == NULL ) break;

		/* . と .. はぬかす */
		if( strcmp( dent->d_name , ".") == 0 || strcmp( dent->d_name , "..") == 0  ){
			continue;
		}
		
		sprintf( fullpath , "%s/%s" , dir , dent->d_name );

		/* flistlenを越えるのなら、それ以上探しても仕方ない */
		if( ( strlen( flist ) + strlen( fullpath) + 1 ) >= flistlen ) break ;
			
		if( (dtmp = opendir( fullpath )) != NULL){
			/* ディレクトリだった */
			closedir( dtmp );
			getAllFilesUnderDirectory( fullpath , flist , flistlen , c);
		} else {
			/* ディレクトリではなかった */
			char addchar[2];
			addchar[0] = c;
			addchar[1] = '\0';
			strcat( flist , fullpath );
			strcat( flist , addchar );
		}
	}
	return 0;
}



int main( int argc , char **argv)
{
	char flist[10000];

	strcpy( flist , "" );
	getAllFilesUnderDirectory( argv[1] , flist , sizeof( flist) ,'\n');

	fprintf( stderr , "%s\n" , flist );

	
}



