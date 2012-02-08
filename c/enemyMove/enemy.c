#include <stdio.h>





#define ON 1
#define OFF 0



struct EE
{
	int x;
	int y;
	
	int before_event;		/* この敵に対しての行ない */
	int reaction;			/* この敵の反応(return) */

	int tox,toy;			/* 目的地または攻撃対象の位置(return) */
	
	int pnum;		/* 全プレイヤーの位置 */
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

   引数
   1    スタート地点のx座標
   2    スタート地点のy座標
   3    マップデータ(int)の先頭アドレス。通れない所は-1を代入する事。
            通れる所は何でもかまわない。またこのデータの多さは XSIZ*YSIZ である。
   4    マップデータ(int)の先頭アドレス。これに値が入って返る。通れれる所にはそこまでの
            歩数が入り、通れない所には -1 がはいる。またこのデータの多さは XSIZ*YSIZ である。
   5    マップデータの X のサイズ
   6    マップデータの Y のサイズ


   返り値
   なし
   */

void traceRoute( int x,  int y , int* map , int* retmap , int xsiz , int ysiz)
{
    int i,j , k;
    int hosuu;

    int walkflg;

    //上
    int dx[]={0 , 1,1,1,0,-1,-1,-1};
    int dy[]={-1,-1,0,1,1, 1, 0,-1};
        

    for( i = 0 ; i < ysiz ; i++ )
        for( j = 0 ; j < xsiz; j ++)
            retmap[xsiz*i + j] = 1000;

    //自分の始まりの位置は0
    retmap[y*xsiz + x] = 0;

    //歩数でループを回す
    //むげんるーぷ
    for(hosuu = 0 ;  ;  hosuu++){

        walkflg = 0;
        for(i = 0 ; i < xsiz ; i++){
            for(j = 0 ; j < ysiz ;j ++){

                //げんざいの歩数のものをみつけたらそれをしょりする
                //現在の歩数のものをみつける            
                if( retmap[j*xsiz+i]  == hosuu ){

                    //ここで8方向をみて今まで行った事がなく
                    //かつ行けるところなら行く
                    //時計まわりにさがす
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
