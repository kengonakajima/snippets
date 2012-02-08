#include <stdio.h>





#define ON 1
#define OFF 0



struct EE
{
	int x;
	int y;
	
	int before_event;		/* $B$3$NE($KBP$7$F$N9T$J$$(B */
	int reaction;			/* $B$3$NE($NH?1~(B(return) */

	int tox,toy;			/* $BL\E*CO$^$?$O967bBP>]$N0LCV(B(return) */
	
	int pnum;		/* $BA4%W%l%$%d!<$N0LCV(B */
	int *px;
	int *py;
	int *pusing;
	
};



int main()
{
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
