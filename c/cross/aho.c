/* (xa,ya) �� (x1,y1)-(x2,y2) ��ޤ�ľ���α����������ɤä��ˤ��뤫Ĵ�٤�

    +
     \
      \
       \
  A .   \     . B
         \
          \
           +

�� A �Ϻ�(1)���� B �ϱ�(-1)�����ξ����0

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
/* ��ʬ(xa,ya)-(xb,yb) �� (x1,y1)-(x2,y2) ��ޤ�ľ���ȸ򺹤��Ƥ��뤫Ĵ�٤롣
   2����ľ�����Ф���Ʊ��¦�ˤ���Ф�����


                +(xa,ya)
                 \
                  \
                   \
  ------(x1,y1)-----------------------(x2,y2)-- ľ��
                     \  
                      +(xb,yb)
�򺹤��Ƥ���ʤ�1�����Ƥ��ʤ��ʤ�0
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
/* ��ʬ (xa,ya)-(xb,yb) �� ��ʬ(xc,yc)-(xd,yd) ���򺹤��Ƥ��뤫Ĵ�٤롣

                +(xa,ya)
                 \
                  \
                   \
    (xc,yc)+-------------------+(xd,yd)
                     \  
                      +(xb,yb)
 
�򺹤��Ƥ�����1�����Ƥ��ʤ��ʤ�0
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

/* 2��ʬ�θ�������롣������ (xx,yy) �Ǥ��롣
   ���Υ롼����ư��뤿��ˤϡ���ʬƱ�Τ��򺹤��Ƥ��뤳�Ȥ�ɬ��

   rate : 1�ΤȤ������礦�ɸ������֤���
   0�ΤȤ���2���ܤ���ʬ�λ���(xc,yc)���֤��� 1����礭���Ȥɤ�ɤ󿭤ӤƤ�����
   �դ�0��꾮������ȿ��¦�˹Ԥ���

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
		/* �¹Ԥ������顼�򤫤��� */
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
