#include <stdio.h>
#include <time.h>
#include <sys/time.h>


struct slot
{
	int use;
	
	unsigned long ip_net;
	unsigned short port_net;
	int dslot;					/* 相手のスロット */
	
	unsigned int send_i;		/* 書きこみ番号 最後にregしたパケットの*/
	unsigned int recv_i;		/* 読みこみ番号 次に読みこむパケットの */

	int open_ack;				/* openのackがきたら1 */
	int accept_wait;			/* acceptされるのをまっている */

	int close_wait;				/* closeされるのをまっている状態なら1。
								   CLOSEコマンドがおくられてきたら1にし、
								   ぜんぶのデータをよみこむまでは実際には
								   クローズしない。このフラグが1の状態で
								   読みこめるデータも書きこめるデータも
								   なくなったら、useを0にする。
								   closeできるかもしれないタイミングは、
								   CLOSEコマンドがきたとき、
								   CLOSEACKコマンドがきたとき、
								   rp_recvしたとき(recvqを消化)、
								   DATAACKをうけとってsendqを消化した
								   ときである。
								   それらのときにチェックしてclose可能
								   であれば、closeする。

								   */
	
	double last_in;				/* 最後にパケットがきたのはいつか */


#define QSIZE	16
	char sb[QSIZE][256];
	char rb[QSIZE][128];

	int open_pbi , close_pbi;	/* open , closeのパケットを送信したら
								   どのパケットバッファをつかったかこ
								   こに記憶 */
	
};

#define MAXSLOT 8000

struct slot* slot;





double getUTimeDouble(void)
{
	struct timeval tm;

	gettimeofday( &tm , NULL );

	return (double)( tm.tv_usec ) + (double)( tm.tv_sec) *
	1000000.0 ;
}




int main()
{
	int i;
	double starttime;
	unsigned long ipn;
	unsigned short portn;
	char buf[128];
	

	printf( "structsize: %d tital:%d\n", sizeof( struct slot ),
		   sizeof( struct slot ) * MAXSLOT );
	
	slot = (struct slot*) malloc( sizeof( struct slot ) * MAXSLOT );
	if( slot == NULL ) return -1;


	starttime = getUTimeDouble();
	for(i=0;i<100000;i++){

		int ind = rand() % MAXSLOT;
		
/*		printf( "%d\n" , ind );*/
		bcopy( buf ,slot[ind].rb[i%16] , sizeof(buf ));
	}
	
	fprintf( stderr , "TIME: %f msec\n" ,
			( getUTimeDouble() - starttime ) / 1000.0);

	
}

