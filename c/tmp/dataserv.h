#define _DATASERV_H_

#ifdef _DATASERV_C_
#define EXT
#else
#define EXT extern
#endif

/****************************************************************
	データベースサーバのためのヘッダ。   


****************************************************************/
EXT struct gamedata {
	int cron;	
} game;
/****************************************************************
地形のデータ
floor という構造体に固める。すべてこの構造体の中に地形に関することはある。
****************************************************************/

struct point {
	
	/* 特異点の情報そのフロアの何番の特異点につながっているか */

	int x , y , kind , destfloor , linkno;

};


struct floor {

	int using;
	char name[20];		/* その階の名前 */
	int width , height;
	int *(data[FLOORDATADEPTH]);	/* data[DATADEPTH][width*height]となるようにメモリ確保
			 	data , obj , flg ,...*/
	int pointno;
	struct point *point;
};


/****************************************************************
アイテムのデータ
itemという構造体にまとめる。
****************************************************************/
struct item{
	char name[20];
		

};

/****************************************************************
	キャラのデータ
	このデータには、それぞれの個人情報にリンクするデータ
	も含まれる。顧客IDがそれである。
	顧客データベースは、また別にある。
	
****************************************************************/
struct chardata{
	int hp;
	int birthtime;
	int x , y , floor;
	int exp;	/* 経験値。これは内部的に計算するだけ。見せない */
	int logintime;	/* loginしている時間 */
	int dir;
	int level;	/* 地形における高さ。Quarter Viewだから。*/
	char name[20];		/* 名前(英文字) */
	char jname[20]; 	/* 名前(日本語) */
	char plan[1024];	/* 自己紹介 */
	char jplan[1024];	/* 自己紹介(日本語) */
	char iconspeak[16][128];	/* アイコンで会話できるが、そのためのセリフを自分で
					   決めとく。 */
	int command[256];	/* どのコマンドを何回実行したか */
	int walklen;			/* 今までにどれだけ歩いたか */
#define MAXITEM 32
	int itemkind[MAXITEM] , itemno[MAXITEM] , itemflg[MAXITEM];	/* アイテム */
	char listenbuf[1024];	/* そのキャラに聞こえる声などが一時入る */
	char custom_id[256];	/* 顧客リストにリンクさせるためのデータ */
	char passwd[20];	/* パスワード */	
	
};
#define MAXFLOOR 100
#define MAXCHAR 100

EXT struct floor floor[MAXFLOOR];
EXT struct chardata chardata[MAXCHAR];






