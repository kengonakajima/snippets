#include <stdio.h>





#define ON 1
#define OFF 0


#define EMOVE_INTERVAL 20	/* $B$3$N2s?t$N4X?t8F$S$@$7$4$H$KE($r0\F0$5$;$k(B */

/*
   $B%/%i%$%"%s%H%W%m%0%i%`$G$3$N9=B$BN$r;H$C$FE($r4IM}$9$k$h$&$K$9$l$P!"(B
   $B$$$A$$$A(BenemyMove$B$KM?$($k0z?t$r:n$i$J$/$F$h$/$J$k$>!#(B

   
*/

struct EE
{
#define EE_MAX 100
	int enumber;			/* $B$$$/$D$^$GM-8z$J%G!<%?$+(B */
	int etype[ EE_MAX ];	/* $B%?%$%W$O!"(B1$B$N$_(B */ 
	int ex[ EE_MAX ];
	int ey[ EE_MAX ];
	int enext_move[ EE_MAX ];	/* $B$3$l$,(B0$B$K$J$C$?$i0\F0$5$;$k(B */
	int ebeforeact[ EE_MAX ];	/* $B$3$N4X?t$r8F$VA0$K$3$NE($,$5$l$?$3$H(B */

#define EBEFORE_ATTACK	1
#define EBEFORE_MAGIC	2
	
	/* $B0J2<$OJV$jCMMQ(B */
	int retx[ EE_MAX ];		/* $BE($N9TF0BP>]$N0LCV!#0\F0$J$iL\E*CO!"967b$J$i$=$N0LCV!#(B */
	int rety[ EE_MAX ];
	int retact[ EE_MAX ];		/* $BE($N<!$N9TF0!#(B 1$B$J$i0\F0!#(B2$B$J$i967b(B */

#define EREACT_MOVE		1
#define EREACT_ATTACK	2
	
#define MAPSIZ 16
	int map[MAPSIZ*MAPSIZ];		/* $B%^%C%W$O!"(B0$B$,DL$l$k!"(B1$B$,DL$l$J$$(B */
								   
#define EE_P_MAX 8

	int pnumber;			/* $B%W%l%$%d!<$N?t(B */
	int px[ EE_P_MAX ];
	int py[ EE_P_MAX ];

};




#define RAND() ( random() % 32768 )

void traceRoute( int x,  int y , int* map , int* retmap , int xsiz , int ysiz);
int getNearestIndex( int mx , int my , int *x , int *y , int count );

/*****************************
  $B$3$N4X?t$r8F$S$@$9$N$O!"L\E*CO$r;XDj$9$k!"$^$?$O967b$J$I$N%"%/%7%g%s$r(B
  $B$5$;$k$H$-$@$1$G$"$k!#(B
  
*****************************/
void enemyMove( struct EE *ee )
{
	int i;
	int retmap[MAPSIZ*MAPSIZ];
	int ii;


	for(i=0;i<ee->enumber;i++){
		
		if( ( -- ee->enext_move[i] ) <= 0 ){
			/* $B0\F0%?%$%_%s%0$@$C$?$>(B */
			
			ee->enext_move[i] = EMOVE_INTERVAL;
			/* EMOVE_INTERVAL - ee->elv[i]$B$H$$$&$h$&$K$7$F%l%Y%k$,$"$,$k$HIQHK$KF0$/$H$+$G$-$k(B*/
			
			switch( ee->etype[i] ){
				case 1:
				{
					/* $B$/$C$D$$$F$$$?$i967b(B */
					if( 0 /* $B$/$C$D$$$F$k(B */ ) {
						
						int npi=getNearestIndex( ee->ex[i],ee->ey[i],ee->px, ee->py , ee->pnumber );
						if( npi >= 0 ){
							ee->retx[i] = ee->px[npi];
							ee->rety[i] = ee->py[npi];
						} else {
							continue;
						}
						ee->retact[i] = EREACT_ATTACK;	
					} else {
						ee->retact[i] = EREACT_MOVE;
					}
					
					if( RAND() > 20000 ){
						/* $B$/$C$D$3$&$H$7$FL\E*CO$r;XDj(B */
						int nearest_player_index =
						getNearestIndex( ee->ex[i] , ee->ey[i],ee->px, ee->py , ee->pnumber );
						
					} else {
						/* 2$BJb0JFb$+$i%i%s%@%`(B */
						ee->retx[i]= ee->ex[i] - 2 + RAND() % 5;
						ee->rety[i]= ee->ey[i] - 2 + RAND() % 5;
						
					}
				}
				break;
			}
		}
		
	}
}


int getNearestIndex( int mx , int my , int *x , int *y , int count )
{
	int i;
	int return_ind=-1;
	
	double neardist=8080808080808.0F, curdist=0.0F;
	
	for(i=0;i<count;i++){
		curdist = (double)(x[i]-mx ) * (double)( x[i]-mx ) + (double)(y[i]-my)*(double)( y[i] -my );
		if( curdist < neardist ){
			neardist = curdist;
			return_ind = i;
		}
	}

	return return_ind;
		
}


int main(int argc , char **argv )
{
	/* $B@$3&$N%^%C%W(B */
	
	int map[MAPSIZ*MAPSIZ]={
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,

		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,

		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,

		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	};
	int retmap[MAPSIZ*MAPSIZ];
	
	struct EE e;
	int i , j;
	
	bzero( &e , sizeof( e ) );



	traceRoute( 8 , 9 , map , retmap , MAPSIZ , MAPSIZ );

	for(i=0;i<MAPSIZ;i++){
		for(j=0;j<MAPSIZ;j++){
			fprintf( stderr , "%d " , retmap[j + i*MAPSIZ] );
		}
		fprintf( stderr , "\n" );
	}

	
	
}


							

/*
   void traceRoute(int x , y , int* map , int* retmap , int xsiz , int ysiz)

   $B0z?t(B
   1    $B%9%?!<%HCOE@$N(Bx$B:BI8(B
   2    $B%9%?!<%HCOE@$N(By$B:BI8(B
   3    $B%^%C%W%G!<%?(B(int)$B$N@hF,%"%I%l%9!#DL$l$J$$=j$O(B-1$B$rBeF~$9$k;v!#(B
            $BDL$l$k=j$O2?$G$b$+$^$o$J$$!#$^$?$3$N%G!<%?$NB?$5$O(B XSIZ*YSIZ $B$G$"$k!#(B
   4    $B%^%C%W%G!<%?(B(int)$B$N@hF,%"%I%l%9!#$3$l$KCM$,F~$C$FJV$k!#DL$l$l$k=j$K$O$=$3$^$G$N(B
            $BJb?t$,F~$j!"DL$l$J$$=j$K$O(B -1 $B$,$O$$$k!#$^$?$3$N%G!<%?$NB?$5$O(B XSIZ*YSIZ $B$G$"$k!#(B
   5    $B%^%C%W%G!<%?$N(B X $B$N%5%$%:(B
   6    $B%^%C%W%G!<%?$N(B Y $B$N%5%$%:(B


   $BJV$jCM(B
   $B$J$7(B
   */

void traceRoute( int x,  int y , int* map , int* retmap , int xsiz , int ysiz)
{
    int i,j , k;
    int hosuu;

    int walkflg;

    //$B>e(B
    int dx[]={0 , 1,1,1,0,-1,-1,-1};
    int dy[]={-1,-1,0,1,1, 1, 0,-1};
        

    for( i = 0 ; i < ysiz ; i++ )
        for( j = 0 ; j < xsiz; j ++)
            retmap[xsiz*i + j] = 1000;

    //$B<+J,$N;O$^$j$N0LCV$O(B0
    retmap[y*xsiz + x] = 0;

    //$BJb?t$G%k!<%W$r2s$9(B
    //$B$`$2$s$k!<$W(B
    for(hosuu = 0 ;  ;  hosuu++){

        walkflg = 0;
        for(i = 0 ; i < xsiz ; i++){
            for(j = 0 ; j < ysiz ;j ++){

                //$B$2$s$6$$$NJb?t$N$b$N$r$_$D$1$?$i$=$l$r$7$g$j$9$k(B
                //$B8=:_$NJb?t$N$b$N$r$_$D$1$k(B            
                if( retmap[j*xsiz+i]  == hosuu ){

                    //$B$3$3$G(B8$BJ}8~$r$_$F:#$^$G9T$C$?;v$,$J$/(B
                    //$B$+$D9T$1$k$H$3$m$J$i9T$/(B
                    //$B;~7W$^$o$j$K$5$,$9(B
                    for( k = 0 ; k < 8 ; k ++ ){

                        if( 0 <= j + dy[k]  && j + dy[k] < ysiz
                           && 0 <= i + dx[k]  && i + dx[k] < xsiz  )
                           
                            if( retmap[ (j+dy[k])*xsiz+i+dx[k] ] == 1000 &&
                               map[ (j+dy[k])*xsiz+i+dx[k] ] != -1 ){
                                retmap[ (j+dy[k])*xsiz+i+dx[k] ] = hosuu + 1;
                                walkflg = 1;
                            }
                    }

                }

            }
        }
        if( walkflg == 0 )
            break;
    }
}

