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
	
}a ;

main()
{
	printf("%d\n", sizeof(struct chardata ));
}


