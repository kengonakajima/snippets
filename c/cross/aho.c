/* (xa,ya) が (x1,y1)-(x2,y2) を含む直線の右か左か、どっちにあるか調べる

    +
     \
      \
       \
  A .   \     . B
         \
          \
           +

点 A は左(1)、点 B は右(-1)、線の上だと0

*/
static int
check_point_lr( int x1, int y1, int x2, int y2, int xa, int ya )
{
	struct _vector_t { int x,y; };
	struct _vector_t v1,v2;
	int d;


	v1.x = x2 - x1;
	v1.y = y2 - y1;
	v2.x = xa - x1;
	v2.y = ya - y1;

	d = v1.x*v2.y - v1.y*v2.x;
	if( d > 0 ){
		return 1;
	} else if( d < 0 ){
		return -1;
	} else {
		return 0;
	}
}
/* 線分(xa,ya)-(xb,yb) が (x1,y1)-(x2,y2) を含む直線と交差しているか調べる。
   2点が直線に対して同じ側にあればいい。


                +(xa,ya)
                 \
                  \
                   \
  ------(x1,y1)-----------------------(x2,y2)-- 直線
                     \  
                      +(xb,yb)
交差しているなら1、していないなら0
*/
static int
check_segment_cross_line( int x1, int y1, int x2, int y2,
						  int xa, int ya, int xb, int yb )
{
	if( check_point_lr( x1,y1,x2,y2, xa,ya )
		!= check_point_lr( x1,y1,x2,y2, xb,yb ) ) {
		return 1;
	} else {
		return 0;
	}
}
/* 線分 (xa,ya)-(xb,yb) と 線分(xc,yc)-(xd,yd) が交差しているか調べる。

                +(xa,ya)
                 \
                  \
                   \
    (xc,yc)+-------------------+(xd,yd)
                     \  
                      +(xb,yb)
 
交差していたら1、していないなら0
*/
static int
check_segment_cross_segment( int xa, int ya, int xb, int yb,
							 int xc, int yc, int xd, int yd )
{
	if( check_point_lr( xa,ya,xb,yb, xc,yc )
		!= check_point_lr( xa,ya,xb,yb, xd,yd ) ) {
		if( check_point_lr( xc,yc,xd,yd, xa,ya )
			!= check_point_lr( xc,yc,xd,yd, xb,yb ) ){
			return 1;
		}
	}
	return 0;
}

/* 2線分の交点を求める。交点は (xx,yy) である。
   このルーチンが動作するためには、線分同士が交差していることが必要

   rate : 1のとき、ちょうど交点を返す。
   0のとき、2個目の線分の始点(xc,yc)を返す。 1より大きいとどんどん伸びていく。
   逆に0より小さいと反対側に行く。

                +(xa,ya) PA
                 \
                  \
                   \
PB  (xc,yc)+--------*----------+(xd,yd) PBe
                     \  
                      +(xb,yb) PAe
   
*/
static int
calc_crossing_point( int xa, int ya, int xb, int yb,
					 int xc, int yc, int xd, int yd,
					 int *xx, int *yy, double rate )
{
	int PAx = xa;
	int PAy = ya;
	int PBx = xc;
	int PBy = yc;
	int Ax = ( xb - xa );
	int Ay = ( yb - ya );
	int Bx = ( xd - xc );
	int By = ( yd - yc );
	double t,div;

	div = Bx * Ay - By * Ax;

	if( div == 0 ){
		/* 並行だ。エラーをかえす */
		return -1;
	}
	t = (double)( Ax * ( PBy - PAy ) - Ay * (PBx - PAx ) ) / div;

	*xx = PBx + (t * Bx) * rate;
	*yy = PBy + (t * By) * rate;
	return 0;
}

int
main()
{
	int i,x=0,y=0;

	i = calc_crossing_point( 100,100, 200,200, 100,150, 200,190,
							 &x, &y, 1.0 );
	printf( "%d %d %d\n", i,x,y );
	
}
