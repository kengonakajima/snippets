#include <stdio.h>





#define ON 1
#define OFF 0


#define EMOVE_INTERVAL 20	/* この回数の関数呼びだしごとに敵を移動させる */

/*
   クライアントプログラムでこの構造体を使って敵を管理するようにすれば、
   いちいちenemyMoveに与える引数を作らなくてよくなるぞ。

   
*/

struct EE
{
#define EE_MAX 100
	int enumber;			/* いくつまで有効なデータか */
	int etype[ EE_MAX ];	/* タイプは、1のみ */ 
	int ex[ EE_MAX ];
	int ey[ EE_MAX ];
	int enext_move[ EE_MAX ];	/* これが0になったら移動させる */
	int ebeforeact[ EE_MAX ];	/* この関数を呼ぶ前にこの敵がされたこと */

#define EBEFORE_ATTACK	1
#define EBEFORE_MAGIC	2
	
	/* 以下は返り値用 */
	int retx[ EE_MAX ];		/* 敵の行動対象の位置。移動なら目的地、攻撃ならその位置。 */
	int rety[ EE_MAX ];
	int retact[ EE_MAX ];		/* 敵の次の行動。 1なら移動。2なら攻撃 */

#define EREACT_MOVE		1
#define EREACT_ATTACK	2
	
#define MAPSIZ 16
	int map[MAPSIZ*MAPSIZ];		/* マップは、0が通れる、1が通れない */
								   
#define EE_P_MAX 8

	int pnumber;			/* プレイヤーの数 */
	int px[ EE_P_MAX ];
	int py[ EE_P_MAX ];

};




#define RAND() ( random() % 32768 )

void traceRoute( int x,  int y , int* map , int* retmap , int xsiz , int ysiz);
int getNearestIndex( int mx , int my , int *x , int *y , int count );

/*****************************
  この関数を呼びだすのは、目的地を指定する、または攻撃などのアクションを
  させるときだけである。
  
*****************************/
void enemyMove( struct EE *ee )
{
	int i;
	int retmap[MAPSIZ*MAPSIZ];
	int ii;


	for(i=0;i<ee->enumber;i++){
		
		if( ( -- ee->enext_move[i] ) <= 0 ){
			/* 移動タイミングだったぞ */
			
			ee->enext_move[i] = EMOVE_INTERVAL;
			/* EMOVE_INTERVAL - ee->elv[i]というようにしてレベルがあがると頻繁に動くとかできる*/
			
			switch( ee->etype[i] ){
				case 1:
				{
					/* くっついていたら攻撃 */
					if( 0 /* くっついてる */ ) {
						
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
						/* くっつこうとして目的地を指定 */
						int nearest_player_index =
						getNearestIndex( ee->ex[i] , ee->ey[i],ee->px, ee->py , ee->pnumber );
						
					} else {
						/* 2歩以内からランダム */
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
	/* 世界のマップ */
	
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

