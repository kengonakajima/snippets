
#include <strings.h>
#include <sys/types.h>
#include <time.h>
#include <sys/dirent.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

/******************************
  $B%G%#%l%/%H%j$K4^$^$l$k$9$Y$F$N%U%!%$%k$r:F5"E*$KC5$7$@$7$F%j%9%H(B
  $B$r:n$k!#(B

  char *dir ... $B%G%#%l%/%H%j$N%Q%9(B($B%U%k%Q%9(B)
  char *flist ... $B%U%!%$%k%j%9%H$rF~$l$F$$$/J8;zNs(B
  int flistlen ... flist $B$N%5%$%:(B
  int c ... $B%H!<%/%s@Z$j$NJ8;z(B
  
  $B$3$N4X?t$O!"(Bflist$B$N:GBgCM$r1[$($J$$HO0O$G!"(Bflist$B$KL>A0$X$N%]%$%s%?$r2C$($F$$$/!#(B

  flist$B$O!"(B
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

		/* . $B$H(B .. $B$O$L$+$9(B */
		if( strcmp( dent->d_name , ".") == 0 || strcmp( dent->d_name , "..") == 0  ){
			continue;
		}
		
		sprintf( fullpath , "%s/%s" , dir , dent->d_name );

		/* flistlen$B$r1[$($k$N$J$i!"$=$l0J>eC5$7$F$b;EJ}$J$$(B */
		if( ( strlen( flist ) + strlen( fullpath) + 1 ) >= flistlen ) break ;
			
		if( (dtmp = opendir( fullpath )) != NULL){
			/* $B%G%#%l%/%H%j$@$C$?(B */
			closedir( dtmp );
			getAllFilesUnderDirectory( fullpath , flist , flistlen , c);
		} else {
			/* $B%G%#%l%/%H%j$G$O$J$+$C$?(B */
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



