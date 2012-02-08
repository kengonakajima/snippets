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
	
}a ;

main()
{
	printf("%d\n", sizeof(struct chardata ));
}


