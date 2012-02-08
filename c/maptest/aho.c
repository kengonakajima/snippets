#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/signal.h>

/*

   地形が大きすぎる時の対応としては、自分でマップのスワッピングを
   作るよりも、OSの洗練されたスワッピングアルゴリズムをうまく生か
   せるようなデータ構造にして、プログラム的にスワップシステムを作らない
   ほうがいいという結論になった。
     移植性を保ったままスワップするには、普通のファイルシステムを使う
   ことになるが、そうすると、ファイルにアクセスするごとにiノードの検索
   が発生し、パフォーマンスが落ちる。それに対して、OSのスワップは、ディ
   スクに直接アクセスし、しばしば特殊な転送モードを使ったりもするため、
   高速であるからである。結果的に、OSのスワップを利用したほうが、性能は
   よくなるはずである。

   OSのスワップ機能を適切に利用するには、OSのメモリページのサイズや、
   ディスクのブロックのサイズを考えることが必要になってくる。
   通常、ディスクブロックのサイズは、512バイトである。そして、メモリ
   のページサイズは、4096バイトである。

   RPGのマップは2次元配列で構成されていて、広いマップの場合は、一行で
   4KBを越えるのが普通である。したがって、一歩下に歩くだけで、別の
   メモリページにアクセスすることになるので、非常に多くのメモリページ
   を要求することになる。メモリを効率よく使うためには、4096バイトに
   おさまるグリッドにマップを分け、それを2次元的に配列すれば最適である
   ということになる。理論的に言えば。


   このプログラムはその実験である。
   
   まず、複数の点が広い広いマップの上をランダムウォークする。それは
   実際のゲームでは、メモリアクセスポイントは、完全なランダムアクセス
   ではなく、近い点を連続的にアクセスすることがほとんどだからである。
   グリッドに分ければ、実際に使用しているメモリブロックの数を減らす
   ことができるはずである。これはtopコマンドを使って調べることができる
   はずだ。完全なランダムアクセスでは、メモリ領域が全マップ領域を
   越える場合、致命的な性能の障害が発生することがわかった。
   
   
   どの程度性能に差が出るのかをテストする。
   
   構造体の大きさを64バイトにして、8*8マスにすれば、1グリッドあたり
   4096バイトになる。getFloorElementの中身をすこし変えるだけである。

   a.out mode charno

   mode ... r キャラがランダムウォークする。
   			s 全員が同じ地点から始まるランダムウォーク
            R 完全ランダムアクセス(キャラがワープしまくり )


  今回わかったメモリの性質として(これはUNIXに共通だと思える)

  * mallocすると、とりあえずswap領域が確保される。実際にメモリ上には
  確保されない。
  * メモリに載っているがアクセスされていないメモリブロックは、放ってお
  いても忘れていくことはない。他のメモリを必要とするプロセスが新たな
  実メモリを必要としたときにディスクにスワップアウトされる。

  マップが広い場合は、
  
 
  
  
*/   

#define MAXCHARNO 100000

int mx[MAXCHARNO], my[MAXCHARNO];


int mode;

struct floorelement {

	/* グラフィック番号を入れておくもの */
	int tile;
	int obj;
	int thinwall[4];	/* 0: 上 1: 右 2: 下 3: 左 */
	int itemobj;

	/* キャラのindex -> キャラデータ表 */
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
	
	/* メモリページと合うように、8*8にすれば4096バイトになるように
   調整した。*/

	int ind;		/* ここでのindは、実際にfloor配列にアクセスするた */
					/* めのインデクスである。 */

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
	
	/* 最初に、位置を初期化する */
	for(i=0;i<MAXCHARNO;i++){
		if( mode == 's' ){
			/* みんな、真ん中から始まる */
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

   

   
   

   
