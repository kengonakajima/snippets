#define _DATASERV_H_

#ifdef _DATASERV_C_
#define EXT
#else
#define EXT extern
#endif

/****************************************************************
	$B%G!<%?%Y!<%9%5!<%P$N$?$a$N%X%C%@!#(B   


****************************************************************/
EXT struct gamedata {
	int cron;	
} game;
/****************************************************************
$BCO7A$N%G!<%?(B
floor $B$H$$$&9=B$BN$K8G$a$k!#$9$Y$F$3$N9=B$BN$NCf$KCO7A$K4X$9$k$3$H$O$"$k!#(B
****************************************************************/

struct point {
	
	/* $BFC0[E@$N>pJs$=$N%U%m%"$N2?HV$NFC0[E@$K$D$J$,$C$F$$$k$+(B */

	int x , y , kind , destfloor , linkno;

};


struct floor {

	int using;
	char name[20];		/* $B$=$N3,$NL>A0(B */
	int width , height;
	int *(data[FLOORDATADEPTH]);	/* data[DATADEPTH][width*height]$B$H$J$k$h$&$K%a%b%j3NJ](B
			 	data , obj , flg ,...*/
	int pointno;
	struct point *point;
};


/****************************************************************
$B%"%$%F%`$N%G!<%?(B
item$B$H$$$&9=B$BN$K$^$H$a$k!#(B
****************************************************************/
struct item{
	char name[20];
		

};

/****************************************************************
	$B%-%c%i$N%G!<%?(B
	$B$3$N%G!<%?$K$O!"$=$l$>$l$N8D?M>pJs$K%j%s%/$9$k%G!<%?(B
	$B$b4^$^$l$k!#8\5R(BID$B$,$=$l$G$"$k!#(B
	$B8\5R%G!<%?%Y!<%9$O!"$^$?JL$K$"$k!#(B
	
****************************************************************/
struct chardata{
	int hp;
	int birthtime;
	int x , y , floor;
	int exp;	/* $B7P83CM!#$3$l$OFbItE*$K7W;;$9$k$@$1!#8+$;$J$$(B */
	int logintime;	/* login$B$7$F$$$k;~4V(B */
	int dir;
	int level;	/* $BCO7A$K$*$1$k9b$5!#(BQuarter View$B$@$+$i!#(B*/
	char name[20];		/* $BL>A0(B($B1QJ8;z(B) */
	char jname[20]; 	/* $BL>A0(B($BF|K\8l(B) */
	char plan[1024];	/* $B<+8J>R2p(B */
	char jplan[1024];	/* $B<+8J>R2p(B($BF|K\8l(B) */
	char iconspeak[16][128];	/* $B%"%$%3%s$G2qOC$G$-$k$,!"$=$N$?$a$N%;%j%U$r<+J,$G(B
					   $B7h$a$H$/!#(B */
	int command[256];	/* $B$I$N%3%^%s%I$r2?2s<B9T$7$?$+(B */
	int walklen;			/* $B:#$^$G$K$I$l$@$1Jb$$$?$+(B */
#define MAXITEM 32
	int itemkind[MAXITEM] , itemno[MAXITEM] , itemflg[MAXITEM];	/* $B%"%$%F%`(B */
	char listenbuf[1024];	/* $B$=$N%-%c%i$KJ9$3$($k@<$J$I$,0l;~F~$k(B */
	char custom_id[256];	/* $B8\5R%j%9%H$K%j%s%/$5$;$k$?$a$N%G!<%?(B */
	char passwd[20];	/* $B%Q%9%o!<%I(B */	
	
};
#define MAXFLOOR 100
#define MAXCHAR 100

EXT struct floor floor[MAXFLOOR];
EXT struct chardata chardata[MAXCHAR];






