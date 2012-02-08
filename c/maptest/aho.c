#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/signal.h>

/*

   $BCO7A$,Bg$-$9$.$k;~$NBP1~$H$7$F$O!"<+J,$G%^%C%W$N%9%o%C%T%s%0$r(B
   $B:n$k$h$j$b!"(BOS$B$N@vN}$5$l$?%9%o%C%T%s%0%"%k%4%j%:%`$r$&$^$/@8$+(B
   $B$;$k$h$&$J%G!<%?9=B$$K$7$F!"%W%m%0%i%`E*$K%9%o%C%W%7%9%F%`$r:n$i$J$$(B
   $B$[$&$,$$$$$H$$$&7kO@$K$J$C$?!#(B
     $B0\?"@-$rJ]$C$?$^$^%9%o%C%W$9$k$K$O!"IaDL$N%U%!%$%k%7%9%F%`$r;H$&(B
   $B$3$H$K$J$k$,!"$=$&$9$k$H!"%U%!%$%k$K%"%/%;%9$9$k$4$H$K(Bi$B%N!<%I$N8!:w(B
   $B$,H/@8$7!"%Q%U%)!<%^%s%9$,Mn$A$k!#$=$l$KBP$7$F!"(BOS$B$N%9%o%C%W$O!"%G%#(B
   $B%9%/$KD>@\%"%/%;%9$7!"$7$P$7$PFC<l$JE>Aw%b!<%I$r;H$C$?$j$b$9$k$?$a!"(B
   $B9bB.$G$"$k$+$i$G$"$k!#7k2LE*$K!"(BOS$B$N%9%o%C%W$rMxMQ$7$?$[$&$,!"@-G=$O(B
   $B$h$/$J$k$O$:$G$"$k!#(B

   OS$B$N%9%o%C%W5!G=$rE,@Z$KMxMQ$9$k$K$O!"(BOS$B$N%a%b%j%Z!<%8$N%5%$%:$d!"(B
   $B%G%#%9%/$N%V%m%C%/$N%5%$%:$r9M$($k$3$H$,I,MW$K$J$C$F$/$k!#(B
   $BDL>o!"%G%#%9%/%V%m%C%/$N%5%$%:$O!"(B512$B%P%$%H$G$"$k!#$=$7$F!"%a%b%j(B
   $B$N%Z!<%8%5%$%:$O!"(B4096$B%P%$%H$G$"$k!#(B

   RPG$B$N%^%C%W$O(B2$B<!85G[Ns$G9=@.$5$l$F$$$F!"9-$$%^%C%W$N>l9g$O!"0l9T$G(B
   4KB$B$r1[$($k$N$,IaDL$G$"$k!#$7$?$,$C$F!"0lJb2<$KJb$/$@$1$G!"JL$N(B
   $B%a%b%j%Z!<%8$K%"%/%;%9$9$k$3$H$K$J$k$N$G!"Hs>o$KB?$/$N%a%b%j%Z!<%8(B
   $B$rMW5a$9$k$3$H$K$J$k!#%a%b%j$r8zN($h$/;H$&$?$a$K$O!"(B4096$B%P%$%H$K(B
   $B$*$5$^$k%0%j%C%I$K%^%C%W$rJ,$1!"$=$l$r(B2$B<!85E*$KG[Ns$9$l$P:GE,$G$"$k(B
   $B$H$$$&$3$H$K$J$k!#M}O@E*$K8@$($P!#(B


   $B$3$N%W%m%0%i%`$O$=$N<B83$G$"$k!#(B
   
   $B$^$:!"J#?t$NE@$,9-$$9-$$%^%C%W$N>e$r%i%s%@%`%&%)!<%/$9$k!#$=$l$O(B
   $B<B:]$N%2!<%`$G$O!"%a%b%j%"%/%;%9%]%$%s%H$O!"40A4$J%i%s%@%`%"%/%;%9(B
   $B$G$O$J$/!"6a$$E@$rO"B3E*$K%"%/%;%9$9$k$3$H$,$[$H$s$I$@$+$i$G$"$k!#(B
   $B%0%j%C%I$KJ,$1$l$P!"<B:]$K;HMQ$7$F$$$k%a%b%j%V%m%C%/$N?t$r8:$i$9(B
   $B$3$H$,$G$-$k$O$:$G$"$k!#$3$l$O(Btop$B%3%^%s%I$r;H$C$FD4$Y$k$3$H$,$G$-$k(B
   $B$O$:$@!#40A4$J%i%s%@%`%"%/%;%9$G$O!"%a%b%jNN0h$,A4%^%C%WNN0h$r(B
   $B1[$($k>l9g!"CWL?E*$J@-G=$N>c32$,H/@8$9$k$3$H$,$o$+$C$?!#(B
   
   
   $B$I$NDxEY@-G=$K:9$,=P$k$N$+$r%F%9%H$9$k!#(B
   
   $B9=B$BN$NBg$-$5$r(B64$B%P%$%H$K$7$F!"(B8*8$B%^%9$K$9$l$P!"(B1$B%0%j%C%I$"$?$j(B
   4096$B%P%$%H$K$J$k!#(BgetFloorElement$B$NCf?H$r$9$3$7JQ$($k$@$1$G$"$k!#(B

   a.out mode charno

   mode ... r $B%-%c%i$,%i%s%@%`%&%)!<%/$9$k!#(B
   			s $BA40w$,F1$8COE@$+$i;O$^$k%i%s%@%`%&%)!<%/(B
            R $B40A4%i%s%@%`%"%/%;%9(B($B%-%c%i$,%o!<%W$7$^$/$j(B )


  $B:#2s$o$+$C$?%a%b%j$N@-<A$H$7$F(B($B$3$l$O(BUNIX$B$K6&DL$@$H;W$($k(B)

  * malloc$B$9$k$H!"$H$j$"$($:(Bswap$BNN0h$,3NJ]$5$l$k!#<B:]$K%a%b%j>e$K$O(B
  $B3NJ]$5$l$J$$!#(B
  * $B%a%b%j$K:\$C$F$$$k$,%"%/%;%9$5$l$F$$$J$$%a%b%j%V%m%C%/$O!"J|$C$F$*(B
  $B$$$F$bK:$l$F$$$/$3$H$O$J$$!#B>$N%a%b%j$rI,MW$H$9$k%W%m%;%9$,?7$?$J(B
  $B<B%a%b%j$rI,MW$H$7$?$H$-$K%G%#%9%/$K%9%o%C%W%"%&%H$5$l$k!#(B

  $B%^%C%W$,9-$$>l9g$O!"(B
  
 
  
  
*/   

#define MAXCHARNO 100000

int mx[MAXCHARNO], my[MAXCHARNO];


int mode;

struct floorelement {

	/* $B%0%i%U%#%C%/HV9f$rF~$l$F$*$/$b$N(B */
	int tile;
	int obj;
	int thinwall[4];	/* 0: $B>e(B 1: $B1&(B 2: $B2<(B 3: $B:8(B */
	int itemobj;

	/* $B%-%c%i$N(Bindex -> $B%-%c%i%G!<%?I=(B */
	int objindex;

	/* index -> itemtable */
	int itemind;

	int damage;

	int offsets[16-10];

};

#define FLOORSIZE_X 2048
#define FLOORSIZE_Y	2048
int gridaccess[FLOORSIZE_Y][FLOORSIZE_X];

int sleepi=0;

struct floorelement *floor;

int faccess( int x , int y )
{
#if 0
	int ind  = x + y*FLOORSIZE_X;
#else
	
	/* $B%a%b%j%Z!<%8$H9g$&$h$&$K!"(B8*8$B$K$9$l$P(B4096$B%P%$%H$K$J$k$h$&$K(B
   $BD4@0$7$?!#(B*/

	int ind;		/* $B$3$3$G$N(Bind$B$O!"<B:]$K(Bfloor$BG[Ns$K%"%/%;%9$9$k$?(B */
					/* $B$a$N%$%s%G%/%9$G$"$k!#(B */

	int grid_x , grid_y ,grid_ind;
	int ingrid_x , ingrid_y , ingrid_ind;
	
	grid_x = x / 8;
	grid_y = y / 8;

	gridaccess[grid_y][grid_x]++;
	
	grid_ind = grid_x + ( grid_y * (FLOORSIZE_X /8) ) ;

	ingrid_x = x % 8;
	ingrid_y = y % 8;
	ingrid_ind = ingrid_x + (ingrid_y * 8);
	

	ind = grid_ind * 64 + ingrid_ind;

/*	printf( "Gx:%d Gy:%d Grid ind: %d iGx:%d iGy:%d ingrid ind:%d ind: %d\n" ,
		   grid_x , grid_y , grid_ind , ingrid_x , ingrid_y , ingrid_ind ,ind);
*/
#endif
	
	
	floor[ ind ].damage ++;
	return floor[ ind ].tile;
}


int charno;
double t,tt,total_t;
int uloop;





double getUTimeDouble(void)
{

	struct timeval tm;

	gettimeofday( &tm , NULL );


    return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}


void leave( int s )
{

	int i , j;

	for(i=0;i<FLOORSIZE_Y;i++){
		for(j=0;j<FLOORSIZE_X ;j++){
			printf( "%d " , gridaccess[j][i] );
		}
		printf("\n");
	}
	exit(0);
}


int main( int argc , char **argv )
{
	int i;
	int loops=0;
	
	if( argc < 4 ){
		fprintf( stderr,"Usage: a.out [rsR] charno sleep\n"
				"s: start in the same position and random walk\n"
				"r: start from position and random walk\n"
				"S: Completely random walk\n sleep: >0\n"
				"d: don't walk\n"
				);
		exit(0);
	}

	//signal(SIGINT , leave );

	sleepi = atoi( argv[3] );
	floor = (struct floorelement*)malloc(
			sizeof( struct floorelement)*FLOORSIZE_X*FLOORSIZE_Y );

	if( floor == NULL ){
		fprintf( stderr, "cannot allocate memory for floor.\n");
		return;

	}

	charno = atoi( argv[2] );
	if( charno >= MAXCHARNO ){
		fprintf( stderr,"charno: too big.\n");
		return;
	}

	mode = argv[1][0];

	if( mode == 'd' ) printf(" don't move\n");
	else if( mode == 'r' ) printf( "random walk\n");
	else if( mode == 's' ) printf( "start form same position\n");
	else if( mode == 'd' ) printf( "don't walk\n");
	
	/* $B:G=i$K!"0LCV$r=i4|2=$9$k(B */
	for(i=0;i<MAXCHARNO;i++){
		if( mode == 's' ){
			/* $B$_$s$J!"??$sCf$+$i;O$^$k(B */
			mx[i] = FLOORSIZE_X/2;
			my[i] = FLOORSIZE_Y/2;
		} else {
			mx[i] = random() % FLOORSIZE_X;
			my[i] = random() % FLOORSIZE_Y;
		}
	}
		
	for(;;){
		loops++;
		uloop++;
		t = getUTimeDouble();
		if( mode == 's' || mode == 'r' ){
			for(i=0;i<charno;i++){
				mx[i] += ( -1 + (random() % 3) );
				my[i] += ( -1 + (random() % 3) );
				if( mx[i] < 0 ) mx[i] = FLOORSIZE_X -1;
				if( mx[i] >= FLOORSIZE_X ) mx[i] = 0;
				if( my[i] < 0 ) my[i] = FLOORSIZE_Y -1;
				if( my[i] >= FLOORSIZE_Y ) my[i] = 0;
				faccess( mx[i] , my[i] );
			}
		} else if( mode == 'R' ){
			for(i=0;i<charno;i++ ){
				mx[i] = random() % FLOORSIZE_X;
				my[i] = random() % FLOORSIZE_Y;
				faccess( mx[i] , my[i] );
			}
		} else {
			/* don't move */
			for(i=0;i<charno;i++){
				faccess( mx[i] , my[i] );
			}
		}
		tt = getUTimeDouble();
		total_t += ( tt-t );
		if( (loops % 1000)==0){
			printf( "loop:%d average usec per access(last 1000 loops): %f\n" ,
				   loops , total_t / uloop / charno);
			uloop = 0;
			total_t = 0;

		}
		if( sleepi == 0 ); else usleep(sleepi);
	}
	
}

   

   
   

   
