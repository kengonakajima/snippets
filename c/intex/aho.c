#include <stdio.h>

#define ON 1
#define OFF 0

/***********************************

  $B85$N%U%!%$%k$N%?%0$+$i!"$=$NFbMF$K1~$8$?J8;zNs$r=q$-$3$`!#(B

  char **params .... $B0z?tNs!#:G8e$NMWAG$O(BNULL$B%]%$%s%?$G$J$1$l$P$J$i$J(B
  $B$$!#(B
  int sockfd .... $B=q$-$3$`%=%1%C%H%P%C%U%!(B

  $B$9$Y$F!"%Q%i%a!<%?$N=*C<$K%L%kJ8;z$NF~$C$F$$$J$$J8;zNs$r;H$&$N$G!"(B
  $BHf3S$K$O(Bstrncmp$B$r;H$&I,MW$,$"$k$N$GCm0U!#(B
  
***********************************/
int contentsInserter( char **params , int sockfd )
{
	char *p;
	char *name=NULL, *param=NULL;


	/* $B0z?tNs$K$O!">/$J$/$F$b(Bname$B$OI,MW$G$"$k!#(B */
	while( p = *params++){
		if( strncmp( p,"name=" ,strlen("name=") )==0 ){
			name = p+strlen("name=");
		}
		if( strncmp( p , "param=" , strlen("param=") )== 0 ){
			param = p + strlen("param=");
		}
	}

	/* name$B$N<oN`$4$H$K!"=hM}$r$U$j$o$1$k!#(B */
	if( name!= NULL ){
		if( param != NULL ){
			/* param$B$,I,MW$J$b$N(B */
		}

		/* param$B$,ITMW$J$b$N!"$^$?$O$=$l$K$h$C$F5!G=$,JQ2=$9$k$b$N(B  */
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

  lifestorm$B%?%0$rM}2r$7$F!"$=$3$K;XDj$7$?J8;zNs$r64$_$3$`!#(B

  <lifestorm>$B%?%0$O!"%M%9%H$G$-$J$$!#(B' '$B$r4^$`$3$H$O$G$-$J$$!#(B


  <lifestorm  name=population> -> 1046
  <lifestorm  name=playerimagetable col=5> -> <table...........
  
  $B%F%-%9%H$NCf$K%?%0$,$J$$$+$I$&$+$rC5$9$K$O!"B?>/9bB.$JJ}K!$r;H$o$J$1$l$P$J$i$J$$!#(B

  char *buf ... $B85$NJ8;zNs(B
  char *tag ... $BH?1~$9$k%?%0(B
  int sockfd ... $B%=%1%C%H%P%C%U%!(B


***********************************/
#define TAG_MAXPARAM 32
void parseHTMLTags( char *buf , char *tag , int sockfd )
{
	char *p;
	int tag_mode = OFF;		/* $B%?%0$NCf$r%Q!<%9$7$F$$$k$+!)(B */
	int arg_mode = OFF;		/* $B0z?tNs$NCf$r%Q!<%9$7$F$$$k$+!)(B */
	char *params[TAG_MAXPARAM];		/* 32$B0z?t$^$G(B */
	int pno = 0;
	int l = strlen( tag );
	p = buf;

	while( *p){
		register int c;
		c = *p;

		if( c == '<' && tag_mode == OFF && strncmp(p+1 , tag ,l)==0){
			/* lifestorm$B%?%0$,$_$D$+$C$?$N$G!"%U%i%0$r$?$F$k!#(B */
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
				/* $B0z?t$O(B32$B8D$^$G$G$"$k!#$=$l0J9_$OL5;k$9$k!#(B */
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
