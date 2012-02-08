/*---------------------------------------------------------------------------*
 * unpack.cpp
 * 98-11-04 by eikeda@titan.co.jp
 *
 * TODO
 * * サイズが増える場合に無圧縮変換(エンコードはあり)
 * * ==> 判定してデコード
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #define DEBUG */
#define LOCAL static
#define EXPORTED

#define INCLUDE_SIZE_INTO_DATA /* サイズ情報をデータの先頭に入れる */
#define ID_STRING          "DaMe"
#define ID_STRING_LEN      4
#define ID_COMPRESSED      '8'
#define ID_NOT_COMPRESSED  '9'
#define SIZE_INFO_OFFSET   5 /* ID_STRINGの長さ、圧縮フラグ1 */
#define HEADER_SIZE        9 /* ID_STRINGの長さ、圧縮フラグ1、サイズ情報4 */

/*-------------------------------------------------------------------------*
 * srcbufからsrclenバイトのデータをエンコードしてdestbufから始まる領域に
 * 格納し、そのバイト数をdestlenに格納します。keyString と keyLength は
 * 暗号化キーとその長さ(バイト数)です。キーの長さに制限はありませんが、
 * 5～10バイト程度を想定しています。
 * destbuf から後には必要なだけ格納用の領域を確保しておく必要があります。
 * destlen は初期化してない状態でも構いません。
 * JPEG画像などデータによっては元よりもデータサイズが増える場合もあるので、
 * destbufに確保しておく領域のバイト数は srclen の倍程度がおすすめです。
 * (確保しているサイズを越えたかどうかのチェックなどはいっさいしません)
 *-------------------------------------------------------------------------*/
EXPORTED int EncodeData(char* destbuf, char* srcbuf, long* destlen, long srclen,
	       char* keyString, int keyLength);

/*--------------------------------------------------------------------------*
 * エンコードされたデータ列の先頭を指すポインタpからそのデコード後のデータ
 * サイズを算出して返します。
 *--------------------------------------------------------------------------*/
EXPORTED long GetOriginalDataSize(char* srcbuf);

/*-------------------------------------------------------------------------*
 * srcbufからsrclenバイトのデータをデコードしてdestbufから始まる領域に
 * 格納し、そのバイト数をdestlenに格納します。keyString と keyLength は
 * 暗号化キーとその長さ(バイト数)です。キーの長さに制限はありませんが、
 * 5～10バイト程度を想定しています。
 * destbuf から後には必要なだけ格納用の領域を確保しておく必要があります。
 * destbufとdestlenを適切に初期化しておく
 * 方法は2つあり、1つは先に destlen = GetOriginalDataSize()関数を使って
 * 元データのサイズを取得しておき、その大きさぴったりの領域を
 * destbuf = malloc(destlen) で確保し、そのままのdestbufとdestlenを
 * 使うことです。2つめは適当に大きなサイズの領域を destbuf に確保しておき、
 * destlen には **正でない** 適当な値を入れて使うことです。後者の場合は
 * 正常終了ならデコードされた元データのサイズが destlen に返って来ますが、
 * 元データのサイズが驚異的に大きかった場合には、データが入り切らずに異常終了
 * する危険があります。
 * (確保しているサイズを越えたかどうかのチェックなどはいっさいしません)
 *-------------------------------------------------------------------------*/
EXPORTED int DecodeData(char* destbuf, char* srcbuf, long* destlen, long srclen,
	       char* keyString, int keyLength);


/* もろもろの宣言 */
#define BUF_SIZE       8192 /* buf_size = bufferの参照部+ bufferの符号化部 */
#define HASH_SIZE     32728 /* insert()参照。エンコード時のみ使用 */
/* 現在は必ず2のべき乗にすること */
#if 0
#define BUF_SIZE_EXP     13 /* log_2 8192 */
#endif

/*--------------------------------------------------------------------------*
 * ムッチャ注意! Windowsではコンパイラ(VC++)がへんな最適化をしてしまう
 * ようなので、絶対に2のべき乗にしてはいけません。
 *--------------------------------------------------------------------------*/
#if 1
#define REG_PART_POS   2190 /* buffer の参照部の最大領域 */
#define MAX_MATCH_LENG 2190 /* 最長一致系列長の上限 */
#else
#define REG_PART_POS   4096 /* buffer の参照部の最大領域 */
#define MAX_MATCH_LENG 4096 /* 最長一致系列長の上限 */
#endif



/*****************************************************************************
 *---------------------------------------------------------------------------*
 * 最上位関数はこれです。
 * modeは零がエンコード(圧縮)、１がデコード(展開)です。
 *---------------------------------------------------------------------------*
 *****************************************************************************/
EXPORTED int unpack(char* bytedata, long* bytelength, int mode){
    char  key[10] = "KeyWord";
    int   keylen = 7;
    char* p_in;
    char* p_out;
    long srclen;
    long destlen;

    srclen = * bytelength;
    p_in   = bytedata;
    /* 出力データサイズを決定します。 */
    if(mode == 1){
        destlen = GetOriginalDataSize(p_in);
    }
    else{
        destlen = srclen * 2;
    }
    /* 出力データ用領域を確保 */
    if((p_out = (char* )malloc(destlen)) == NULL){
        fprintf(stderr, "Error: out of memory.\n");
        free(p_in);
        return 0;
    }
    /* 圧縮エンコード＝デコード実行 */
    if(mode == 1){
        DecodeData(p_out, p_in, & destlen, srclen, key, keylen);
    }
    else{
        EncodeData(p_out, p_in, & destlen, srclen, key, keylen);
    }
    /* 元の領域に書き戻します。 */
    * bytelength = destlen;
    memcpy(bytedata, p_out, destlen);
    free(p_out);
    return 1;
}





/*****************************************************************************
 *---------------------------------------------------------------------------*
 * ビットI/Oとか簡単な暗号化とか
 * 暗号化は基本中の基本、排他論理和を使った文字対文字、対照のメチャ単純なものです。
 * もっと複雑で負荷がかからない例はないかなあ
 *---------------------------------------------------------------------------*
 *****************************************************************************/
typedef struct {
  int infilt;
  int inbyte;
  int outcnt;
  int outbyte;
  int hoeindex;
  int hoej;
  unsigned char* KeyString;
  int KeyLength;
} GSTATIC;

static GSTATIC gg;

LOCAL void BitioInit(unsigned char* keyString, int keyLength);
LOCAL unsigned char* bit_in(unsigned char* p, int* v, long* lenp);
LOCAL unsigned char* bit_out(unsigned char* p, int bit, int* tame, long* lenp);
LOCAL void flush(unsigned char* p, long* lenp);

/*---------------------------------------------------------------------------*
 * 静的で内部的な変数を初期化します。
 * 同時に２スレッド以上で圧縮展開することを考えていません。
 * EncodeData()かDecodeData()で処理開始前に必ず呼ばないと２回目以降はエラーになるぞう
 *---------------------------------------------------------------------------*/
LOCAL void BitioInit(unsigned char* keyString, int keyLength){
  gg.KeyString = (unsigned char* ) keyString;
  gg.KeyLength = keyLength;
  gg.infilt = 1;
  gg.inbyte = 0;
  gg.outcnt = 0;
  gg.outbyte = 0;
  gg.hoeindex = 0;
  gg.hoej     = 0;
}

/*---------------------------------------------------------------------------*
 * 非常に簡単な暗号化
 * もっと工夫予定です。別にエンコードとデコードで違う関数にしてもいいのだし、
 * これではあまり意味がありません。
 * bit_in(), bit_out() に呼ばれます。
 *---------------------------------------------------------------------------*/
#define SEEDLOOP 126
LOCAL int hoencrypt(int c, unsigned char* keyString, int keyLength){
  c = c ^ keyString[gg.hoeindex];
  c = c ^ ((254-(gg.hoej & 254)) | (127 ^ gg.hoej));
  if(++ gg.hoeindex >= keyLength){
    gg.hoeindex = 0;
  }
  if(++ gg.hoej > SEEDLOOP){
    gg.hoej = 0;
  }
  return c;
}


/*---------------------------------------------------------------------------*
 * p から1ビット(実際には1バイト単位で)読み進めます。
 * で、ポインタを返し、読んだビットは v に格納。読んだ総バイト数は lenp
 * に蓄積します。
 *---------------------------------------------------------------------------*/
LOCAL unsigned char* bit_in(unsigned char* p, int* v, long* lenp){
/*  static int filt = 1, byte = 0; */
  unsigned char* q;

  if (gg.infilt == 1) {
#if 0
    gg.inbyte = * p;
#else
    gg.inbyte = hoencrypt(* p, gg.KeyString, gg.KeyLength);
#endif
    q = p + 1;
    (* lenp)++;
    gg.infilt = 0x80;
  }
  else{
    q = p;
    /* * lenp そのまま */
    gg.infilt = gg.infilt >> 1;
  }
  * v = ((gg.inbyte & gg.infilt) == 0) ? 0 : 1;
  return q;
}


/*---------------------------------------------------------------------------*
 * pから１bitずつ書きすすめます。ビット単位で呼ばれますが実際は１byteになった時点で
 * バッファに書き出し、ポインタを進めます。 
 * 溜まっているbit数をtameに返します。lenpは現在までに書き込んだバイト数を管理する
 * 変数へのポインタです。
 *---------------------------------------------------------------------------*/
LOCAL unsigned char* bit_out(unsigned char* p, int bit, int* tame, long* lenp){
/*  static int cnt = 0, byte = 0; */
  unsigned char* q;

  gg.outbyte = bit + (gg.outbyte << 1);
  if (gg.outcnt == 7) {
#if 0
    * p = gg.outbyte;
#else
    * p = hoencrypt(gg.outbyte, gg.KeyString, gg.KeyLength);
#endif
    gg.outcnt = 0;
    gg.outbyte = 0;
    (* lenp)++;
    q = p + 1;
  }
  else{
    gg.outcnt++;
    /* * lenp そのまま  */
    q = p;
  }
  * tame = gg.outcnt;
  return q;
}


/*---------------------------------------------------------------------------*
 * 読み込み終了時に残っているビットをはきだします。
 * EncodeData()に呼ばれます。
 *---------------------------------------------------------------------------*/
LOCAL void flush(unsigned char* p, long* lenp){
  int tame;
  unsigned char* q;
  while(1){
    q = bit_out(p, 0, & tame, lenp);
    if(tame == 0) break;
    p = q;
  }
}


/*****************************************************************************
 *---------------------------------------------------------------------------*
 * ASCIIエンコード(BASE64みたいなの)を行ないます。
 *---------------------------------------------------------------------------*
 *****************************************************************************/
typedef struct {
  int infilt;
  int inbyte;
  int outcnt;
  int outbyte;
} hoeGSTATIC;

static hoeGSTATIC hgg;

static unsigned char* en(int b){
  static unsigned char hoe[10];
  hoe[0] = (b & 128) ? '1' : '0';
  hoe[1] = (b & 64) ? '1' : '0';
  hoe[2] = (b & 32) ? '1' : '0';
  hoe[3] = (b & 16) ? '1' : '0';
  hoe[4] = (b & 8) ? '1' : '0';
  hoe[5] = (b & 4) ? '1' : '0';
  hoe[6] = (b & 2) ? '1' : '0';
  hoe[7] = (b & 1) ? '1' : '0';
  hoe[8] = '\0';
  return hoe;
}

/*---------------------------------------------------------------------------*
 * 内部の静的な変数を初期化します。処理前に必ず呼ばないといけません。
 *---------------------------------------------------------------------------*/
LOCAL void Uuen_Init(void){
  hgg.infilt = 1;
  hgg.inbyte = 0;
  hgg.outcnt = 0;
  hgg.outbyte = 0;
}

/*---------------------------------------------------------------------------*
 * ASCIIエンコード１ビット読み込み
 *---------------------------------------------------------------------------*/
LOCAL unsigned char* uubit_in(unsigned char* p, int* v, long* lenp){
  unsigned char* q;

  if(p == NULL){
    /* static変数を初期化する */
    fprintf(stderr, "debug** filt=%d, byte=%d\n", hgg.infilt, hgg.inbyte);
    hgg.infilt = 1; hgg.inbyte = 0;
    return NULL;
  }

  if (hgg.infilt == 1) {
#if 1
    hgg.inbyte = * p;
#ifdef DEBUG
    fprintf(stderr, "{%d}", hgg.inbyte & 0xFF);
    fprintf(stderr, "{%s}", en(hgg.inbyte));
#endif
#else
    byte = hoencrypt(* p, KeyString, KeyLength);
#endif
    q = p + 1;
    (* lenp)++;
    hgg.infilt = 0x80;
  }
  else{
    q = p;
    /* * lenp そのまま */
    hgg.infilt = hgg.infilt >> 1;
  }
  * v = ((hgg.inbyte & hgg.infilt) == 0) ? 0 : 1;
  return q;
}


/*---------------------------------------------------------------------------*
 * ASCIIエンコード１ビット書き込み(実際には1バイト単位)
 * たまっているビット数をtameに返します。
 *---------------------------------------------------------------------------*/
LOCAL unsigned char* uubit_out(unsigned char* p, int bit, int* tame, long* lenp){
/*  static int cnt = 0, byte = 0; */
  unsigned char* q;

  gg.outbyte = bit + (gg.outbyte << 1);
  if (gg.outcnt == 7) {
#if 1
    * p = hgg.outbyte;
#ifdef DEBUG
    fprintf(stderr, "{%d}", hgg.outbyte & 0xFF);
    fprintf(stderr, "{%s}", en(hgg.outbyte));
#endif
#else
    * p = hoencrypt(byte, KeyString, KeyLength);
#endif
    hgg.outcnt = 0;
    hgg.outbyte = 0;
    (* lenp)++;
    q = p + 1;
  }
  else{
    hgg.outcnt++;
    /* * lenp そのまま  */
    q = p;
  }
  * tame = hgg.outcnt;
  return q;
}

#if 0
/* 読込み終了時に残っているbitを掃き出す */
LOCAL void flush(unsigned char* p, long* lenp){
  int tame;
  unsigned char* q;
  while(1){
    q = uubit_out(p, 0, & tame, lenp);
    if(tame == 0) break;
    p = q;
  }
}
#endif


/****************************************************************************
 *--------------------------------------------------------------------------*
 * メインの圧縮はここから
 *--------------------------------------------------------------------------*
 ****************************************************************************/
static int malloc_count = 0, free_count = 0; /* デバッグ用だった */
struct node {
    unsigned long key1;	/* 先頭3文字のコード */
    unsigned char key2;	/* 残り1文字のコード */
    unsigned short key1_0ff0; /* key1 & 0x0ff0 の値。insert_slave参照 */
    struct node *left;
    struct node *right;
    int buf_start_pos;
    int buf_end_pos;	/* もっとも最近加えられた文字の絶対バッファ位置 */
};

typedef struct node Tree;

typedef struct {
  /*int pos_code_filt;*/  /* pos_code()を参照(初期値1) */
  int pos_code2_filt; /* pos_code2()を参照(1) */
  int flush_need;     /* ファイル出力時のbit掃き捨ての有無(0) */
  int get_c_chr;      /* get_c()参照(0) */
  int buf_point;      /* 配列buf[]の絶対位置(0) */
  int buf_leng;       /* 配列buf[]の入力記号数(0) */
  int part_pos;       /* 仕切りの位置(move_part_e()参照)(0) */
  int now_match_leng; /* search()参照(0) */
  /*int set_tree_ps_back;*/ /* set_tree()参照(0) */
} enGSTATIC;

static enGSTATIC egg;

static unsigned char* Currentp;
static unsigned char* Currenti;
static long InLen, OutLen;
static long InTotalBytes;
static Tree *T;

LOCAL int I_log(int num, int *filt);
LOCAL void fit_code(int code, int filt);
/* LOCAL void pos_code(int code, int p_pos); */
LOCAL void pos_code2(int code, int p_pos);
LOCAL void leng_code(int code);

LOCAL struct node *talloc(void);
LOCAL void insert(unsigned long x1, unsigned char x2, struct node **p, int *b_ps, int b_pe, int *ret, int *leng);
LOCAL void deleteo(unsigned long x1, unsigned char x2, struct node **p, int b_p);

LOCAL int get_c(void);
LOCAL void put_code(int h, int pos, int lng, int part);
/* LOCAL int get_buf_e(int pos); */
LOCAL int put_buf_e(int c);
/* LOCAL void fill_buf(int c); */
/* LOCAL int get_real_pos(int pos); */
/* LOCAL int get_ring_pos(int pos); */
/* LOCAL int get_buf_e_leng(void); */
LOCAL int move_part_e(int num);
/* LOCAL int get_search_code_e(int pos); */
/* LOCAL int get_part_e(void); */
LOCAL void pre_set_buf(void);
LOCAL int set_buf(void);
LOCAL void init_buf(void);
LOCAL void mod_buf(int num);
LOCAL int do_cont(void);
LOCAL int search(int pos0, int pos1, int *lng);
LOCAL int set_tree(int pos, int *match_pos, int *match_leng);
LOCAL void encode(int ps, int pe);
LOCAL void del_tree(int shift);

/*--------------------------------------------------------------------------*
 * 広域データを初期化します。EncodeData()に呼ばれます。
 *--------------------------------------------------------------------------*/
LOCAL void initEncode(void){
  /* egg.pos_code_filt = 1; */
  egg.pos_code2_filt = 1;
  egg.flush_need = 0;
  egg.get_c_chr = 0;
  egg.buf_point = 0;
  egg.buf_leng = 0;
  egg.part_pos = 0;
  egg.now_match_leng = 0;
  /* egg.set_tree_ps_back = 0; */
}



/*--------------------------------------------------------------------------*
 * もとbin_code.cだったルーチン
 * これは簡単な整数ベースの対数の計算です。
 *--------------------------------------------------------------------------*/
LOCAL int I_log(int num, int *filt){
  /* 整数用対数 log2 num を返す filt=2^(log2 num) */
  int i = 0;
  (*filt) = 1;
  for (i = 0; (*filt) <= num; i++)
    *filt = (*filt) << 1;
  return (i);
}


/*--------------------------------------------------------------------------*
 * ビット幅filtで文字codeを符号化します。
 *--------------------------------------------------------------------------*/
LOCAL void fit_code(int code, int filt){
  int bit;

  filt = filt >> 1;
  while (filt > 0) {
    bit = ((code & filt) == 0) ? 0 : 1;
#if 0
    egg.flush_need = bit_out(fp_out, bit);
#else
    Currentp = bit_out(Currentp, bit, & egg.flush_need, & OutLen);
#endif
    filt = filt >> 1;
  }
}

#if 0
LOCAL void pos_code(int code, int p_pos){
  /* 一致位置の符号化 */
  /* static int filt = 1; */
  int leng = p_pos;

  while (egg.pos_code_filt < leng){
    egg.pos_code_filt = egg.pos_code_filt << 1;
  }
  fit_code(code, egg.pos_code_filt);
}
#endif /* deprecated */

/*--------------------------------------------------------------------------*
 * 一致位置を符号化します(その２、真打)
 *--------------------------------------------------------------------------*/
LOCAL void pos_code2(int code, int p_pos){
  /* static int filt = 1; */
  int leng;

  leng = p_pos;
  while (egg.pos_code2_filt < leng){
    egg.pos_code2_filt = egg.pos_code2_filt << 1;
  }
  if (code < egg.pos_code2_filt - leng){
    fit_code(code, egg.pos_code2_filt / 2);
  }
  else{
    fit_code(code + egg.pos_code2_filt - leng, egg.pos_code2_filt);
  }
}

/*--------------------------------------------------------------------------*
 * 一致記号数を符号化します。
 *--------------------------------------------------------------------------*/
LOCAL void leng_code(int code){
  int i, ret;
  int leng;
  
  leng = I_log(code, &ret);
  for (i = 1; i < leng; i++)
#if 0
    egg.flush_need = bit_out(fp_out, 0);
#else
    Currentp = bit_out(Currentp, 0, & egg.flush_need, & OutLen);
#endif
  fit_code(code, ret);
}



/*--------------------------------------------------------------------------*
 * もと tree.c だった部分
 * これはTree型のセルを作って返す関数です。
 *--------------------------------------------------------------------------*/
LOCAL Tree *talloc(void){
  malloc_count++;
  return ((Tree *) malloc(sizeof(Tree)));
}

/*--------------------------------------------------------------------------*
 * 同じく解放する関数です。
 *--------------------------------------------------------------------------*/
LOCAL void tfree(Tree* p){
  if(p->left != NULL)  tfree(p->left);
  if(p->right != NULL) tfree(p->right);
  free_count++;
  free(p);
}

/*--------------------------------------------------------------------------*
 * 二分木の挿入操作
 *--------------------------------------------------------------------------*/
#ifdef ORIGINAL
LOCAL void insert(unsigned long x1, unsigned char x2, Tree** p,
                  int* b_ps, int b_pe, int* ret, int* leng) /* 4記号の挿入 */
/* retはこれまでの中で */
/* もっとも最近のその記号の絶対バッファ位置 */
/* 新規なら -1 */
/* lengは一致記号数(2~4) */
{
    if ((*p) == NULL) {
	(*p) = talloc();
	if ((*p) == NULL) {
	    fprintf(stderr, "MEMORY FULL!\n");
	    exit(1);
	}
	(*p)->key1 = x1;
	(*p)->key2 = x2;
	(*p)->left = (*p)->right = NULL;
	(*p)->buf_start_pos = b_pe;
	(*p)->buf_end_pos = b_pe;
	(*ret) = -1;
    } else {
	if (x1 == (*p)->key1) {
	    (*leng) = 3;
	    (*b_ps) = (*p)->buf_start_pos;	/* 3記号一致 */
	    if (x2 == (*p)->key2) {	/* 4記号一致 */
		(*leng) = 4;
		(*ret) = (*p)->buf_end_pos;
		(*p)->buf_end_pos = b_pe;
	    } else if (x2 < (*p)->key2)
		insert(x1, x2, &((*p)->left), b_ps, b_pe, ret, leng);
	    else
		insert(x1, x2, &((*p)->right), b_ps, b_pe, ret, leng);
	} else {
	    if ((x1 & 0x0ff0) == ((*p)->key1 & 0x0ff0)) {
	      /* 2記号の一致 */
	      (*leng) = 2;
	      (*b_ps) = (*p)->buf_start_pos;
	    }
	    if (x1 < (*p)->key1)
		insert(x1, x2, &((*p)->left), b_ps, b_pe, ret, leng);
	    else
		insert(x1, x2, &((*p)->right), b_ps, b_pe, ret, leng);
	}
    }
}
#else

#define USE_HASH
/* #define DEBUG_HASH */
#ifdef  USE_HASH
typedef struct {
  Tree* treep;
  unsigned long x1;
  unsigned char x2;
} HTABLE;
/* ハッシュテーブル。リストぶらさがり型ではないので、衝突すると上書きされる */
static HTABLE htable[HASH_SIZE];

#ifdef DEBUG_HASH
/* デバッグ用。ハッシュにヒットした回数、外れた回数を数える */
static long hitctr, unhitctr;
/* デバッグ用。衝突の発生回数を数える */
static long colctr;
#endif
/* ハッシュ関数 */
static unsigned short hashfunc(unsigned long x1, unsigned char x2){
#if 1 /* 簡単すぎる(ヒット率30%程度、使用セル約400) */
  return (unsigned short)((x1>>5+x2) & ((unsigned long)(HASH_SIZE-1)));
#else /* 衝突は減るが、複雑なので時間はかかる */
  unsigned long r;
  r = ( ((x1 & 0xFFFF0000) >> 15) + (x1 & 0x0000FFFF)
    + ((unsigned long)x2 << 2) );
  return (unsigned short) (r & (HASH_SIZE-1));
#endif
}
/* 初期化。必要 */
static void hashinit(void){
  int i;
  for(i=0; i<HASH_SIZE; i++){
    htable[i].treep = NULL;
    htable[i].x1 = (unsigned long)(-1); /* 適当な数 */
    htable[i].x2 = (unsigned char)(-1);
  }
#ifdef DEBUG_HASH
  hitctr = 0; unhitctr = 0; colctr = 0;
#endif
}

#ifdef DEBUG_HASH
/* デバッグ用。使われなかったセルの数を返す */
static int getUnusedCells(void){
  int i;
  int ctr = 0;
  for(i=0; i<HASH_SIZE; i++){
    if(htable[i].treep == NULL){ ctr++; }
  }
  return ctr;
}
#endif

/* ハッシュ位置に同じデータがあれば、そのtreepメンバを返す。 */
/* 何もセットされてないか、自分と違うのがセットされていたら、 */
/* NULLを返す。 */
static Tree* hashgetp(unsigned long x1, unsigned char x2){
  int index = hashfunc(x1, x2);
  if(htable[index].x1 != x1 || htable[index].x2 != x2){
    return NULL;
  }
  else{
#ifdef COMPILE_VERBOSELY
    fprintf(stdout, "got! x1=%ld x2=%ld [%d] (%p) key1=%ld key2=%d\n",
	    x1,  x2,  index, htable[index].treep,
	    htable[index].treep->key1, htable[index].treep->key2);
#endif
    return htable[index].treep;
  }
}
/* deleteoで削除された(x1,x2)をハッシュからも削除 */
/* (ダミー値を入れてマッチしないようにする) */
static void hashunset(unsigned long x1, unsigned char x2){
  int index = hashfunc(x1, x2);
  htable[index].x1 = (unsigned long)(-1);
  htable[index].x2 = (unsigned char)(-1);
}
/* 常に上書きする */
static void hashsetp(Tree* p, unsigned long x1, unsigned char x2){
  int index = hashfunc(x1, x2);
#ifdef COMPILE_VERBOSELY
  if(htable[index].treep != NULL){
    fprintf(stdout, "set overwritten: old: x1=%d, x2=%d, p=%p\n",
	    htable[index].x1, htable[index].x2, htable[index].treep);
  }
#endif
#ifdef DEBUG_HASH
  if(htable[index].treep != NULL){
    colctr++;
    fprintf(stdout, "Ouch! old(%lx,%x) new(%lx,%x)\n",
	    htable[index].x1, htable[index].x2, x1, x2);
  }
#endif
  htable[index].treep = p;
  htable[index].x1 = x1;
  htable[index].x2 = x2;
#ifdef COMPILE_VERBOSELY
  fprintf(stdout, "set. x1=%ld x2=%ld [%d] (%p) key1=%ld key2=%d\n",
	  x1, x2, index, htable[index].treep, htable[index].treep->key1,
	  htable[index].treep->key2);
#endif
}
#endif /* USE_HASH */


/* 記号の挿入。もっとも時間を食う関数 */
LOCAL void insert(unsigned long x1, unsigned char x2, Tree** p,
                  int* b_ps, int b_pe, int* ret, int* leng){
  unsigned short x1_0ff0 = (unsigned short)(x1 & 0x0ff0);

#ifdef USE_HASH
  int hashgotf = 0;
  static Tree* q;
  q = hashgetp(x1, x2);
  if(q != NULL){
    p = &q;
/*---
    if (x1 != (*p)->key1){
      fprintf(stdout, "urano! x1=%ld, x2=%ld, key1=%ld, key2=%d, *p=%p\n",
	      x1, x2, (*p)->key1, (*p)->key2, *p);
      exit(1);
    }
    else if(x2 != (*p)->key2){
      fprintf(stdout, "urano2! x1=%ld, x2=%ld, key1=%ld, key2=%d, *p=%p\n",
	      x1, x2, (*p)->key1, (*p)->key2, *p);
      exit(1);
    }
 ---*/
    hashgotf = 1;
#ifdef DEBUG_HASH
    hitctr++;
#endif
#if 1
    /* 下のwhileループの「４記号一致」の処理をバイパス実行するのだ */
    (*b_ps) = (*p)->buf_start_pos;
    (*leng) = 4;
    (*ret) = (*p)->buf_end_pos;
    (*p)->buf_end_pos = b_pe;
    return; /* 見つかった */
#endif
  }
#ifdef DEBUG_HASH
  else{
    unhitctr++;
  }
#endif
#endif /* USE_HASH */

  while(*p != NULL){
    /* 木をたどる */
    if (x1 == (*p)->key1) { /* 3記号以上一致している */
      (*leng) = 3;
      (*b_ps) = (*p)->buf_start_pos; /* 3記号一致 */

      if(x2 < (*p)->key2){
        p = & ((*p)->left); /* 左の枝 */
      }
      else if(x2 > (*p)->key2){
        p = & ((*p)->right); /* 右の枝 */
      }
      else{
	/* 4記号一致 */
	(*leng) = 4;
	(*ret) = (*p)->buf_end_pos;
	(*p)->buf_end_pos = b_pe;
#ifdef USE_HASH
	if(! hashgotf){
	  hashsetp(*p, x1, x2);
	}
#endif
	return; /* 見つかった */
      }
    }
    else {
      if (x1_0ff0 == (*p)->key1_0ff0){ /* 2記号の一致 */
	(*leng) = 2;
	(*b_ps) = (*p)->buf_start_pos;
      }
      if (x1 < (*p)->key1) p = & ((*p)->left);  /* 左の枝 */
      else                 p = & ((*p)->right); /* 右の枝 */
    }
  }

  /* *pがNULLのとこまで来たら */

  if(hashgotf){
    fprintf(stdout, "urano null!\n");
    exit(1);
  }

  if(((*p) = talloc()) == NULL){
    fprintf(stderr, "MEMORY FULL!\n");
    return;
  }
  (*p)->key1 = x1;
  (*p)->key1_0ff0 = x1_0ff0;
  (*p)->key2 = x2;
  (*p)->left = (*p)->right = NULL;
  (*p)->buf_start_pos = b_pe;
  (*p)->buf_end_pos = b_pe;
  (*ret) = -1;
}

#endif /* ORIGINAL */


/* 4記号の削除 */
#ifdef ORIGINAL1025
LOCAL void deleteo(unsigned long x1, unsigned char x2, Tree** p, int b_p){
  Tree *p1, *p2, *p3;
  if ((*p) == NULL) {
    fprintf(stderr, "DELETE SEARCH FAULT.\n");
    return;
  }
  else if ((*p)->key1 == x1) {
    if ((*p)->key2 == x2) {
      if (b_p == -1) {
	p3 = (*p);
	if ((*p)->right == NULL)
	  (*p) = (*p)->left;
	else if ((*p)->left == NULL)
	  (*p) = (*p)->right;
	else {
	  p2 = (*p)->right;
	  if (p2->left == NULL) {
	    p2->left = (*p)->left;
	    (*p) = p2;
	  }
	  else {
	    while (p2->left != NULL) {
	      p1 = p2;
	      p2 = p2->left;
	    }
	    p1->left = p2->right;
	    p2->left = (*p)->left;
	    p2->right = (*p)->right;
	    (*p) = p2;
	  }
	}
	free(p3);
	free_count++;
      }
      else
	(*p)->buf_start_pos = b_p;
    }
    else if (x2 < (*p)->key2)
      deleteo(x1, x2, &((*p)->left), b_p);
    else
      deleteo(x1, x2, &((*p)->right), b_p);
  }
  else if (x1 < (*p)->key1)
    deleteo(x1, x2, &((*p)->left), b_p);
  else
    deleteo(x1, x2, &((*p)->right), b_p);
}
#else
LOCAL void deleteo(unsigned long x1, unsigned char x2, Tree** p, int b_p){
  Tree *p1, *p2, *p3;

  while(*p != NULL){
    /* 木をたどる */
    if ((*p)->key1 == x1) {
      if ((*p)->key2 == x2) {
	if (b_p == -1) {
	  p3 = (*p);
	  if ((*p)->right == NULL)
	    (*p) = (*p)->left;
	  else if ((*p)->left == NULL)
	    (*p) = (*p)->right;
	  else {
	    p2 = (*p)->right;
	    if (p2->left == NULL) {
	      p2->left = (*p)->left;
	      (*p) = p2;
	    }
	    else {
	      while (p2->left != NULL) {
		p1 = p2;
		p2 = p2->left;
	      }
	      p1->left = p2->right;
	      p2->left = (*p)->left;
	      p2->right = (*p)->right;
	      (*p) = p2;
	    }
	  }
	  free(p3);
	  free_count++;
#ifdef USE_HASH
	  hashunset(x1, x2);
#endif
	}
	else
	  (*p)->buf_start_pos = b_p;
	return;
      }
      else if (x2 < (*p)->key2)
	p = & ((*p)->left);  /* 左の枝 */
      else
	p = & ((*p)->right);  /* 右の枝 */
    }
    else if (x1 < (*p)->key1)
      p = & ((*p)->left);  /* 左の枝 */
    else
      p = & ((*p)->right);  /* 右の枝 */
  }
  fprintf(stdout, "DELETE SEARCH FAULT for x1=%ld x2=%ld\n", x1, x2);
}
#endif


/*--------------------------------------------------------------------------*
 * 本体部分
 *--------------------------------------------------------------------------*/
LOCAL int get_c(void){
  /* static int chr = 0; */
  if(InLen > InTotalBytes){
    egg.get_c_chr = EOF;
    InLen--;
  }
  if(egg.get_c_chr != EOF){
    egg.get_c_chr = * Currenti++;
    InLen++;
  }
  return egg.get_c_chr;
}

LOCAL void put_code(int h, int p, int lng, int part){
  /* LZBフォーマットの2値出力 */
  int tame;
  Currentp = bit_out(Currentp, h, & tame, & OutLen);

  if (h == 0){
    fit_code(p, 256);
  }
  else {
    pos_code2(p, part);
    leng_code(lng - 3);
  }
}

/********************************************** リング buffer 操作 */
static int buf[BUF_SIZE];
static int connect_buf[BUF_SIZE];

#ifdef ORIGINAL1025
LOCAL int get_buf_e(int pos){
  if (pos >= egg.buf_leng){
    return -1;
  }
  if (egg.buf_leng == BUF_SIZE){
#ifdef ORIGINAL
    pos = (pos + egg.buf_point) % BUF_SIZE;
#else
    /*
    pos += egg.buf_point;
    while(pos >= BUF_SIZE){
      pos -= BUF_SIZE;
    }
    */
    pos = ((pos + egg.buf_point) & (BUF_SIZE-1));
#endif
  }
  return buf[pos];
}
#else
/* このマクロの引数は複数回評価されます。非常に注意 */
#define get_buf_e(z) ((int) ( (((int)(z))>=egg.buf_leng) ? (-1) : buf [ (egg.buf_leng == BUF_SIZE) ? ( (((int)(z)) + egg.buf_point) & (BUF_SIZE-1) ) : ((int)(z)) ] ) )
#endif

LOCAL int put_buf_e(int c){	/* egg.buf_leng を返す */
  buf[egg.buf_point] = c;
#if 0
  egg.buf_point = (egg.buf_point + 1) % BUF_SIZE;
#else
  egg.buf_point = ( (egg.buf_point + 1) & (BUF_SIZE-1) );
#endif
  egg.buf_leng = (egg.buf_leng < BUF_SIZE) ? egg.buf_leng + 1 : BUF_SIZE;
  return (egg.buf_leng);
}

#if 0
LOCAL void fill_buf(int c){
  /* buffer の残りを c で埋め尽くす */
  while (egg.buf_leng < BUF_SIZE)
    put_buf_e(c);
}
#endif /* deprecated */

#ifdef ORIGINAL1025
LOCAL int get_real_pos(int pos){
  /* buffer 上の位置 pos の配列上の位置を返す */
  if (pos >= egg.buf_leng)    return (-1);
  if (egg.buf_leng == BUF_SIZE){
#ifdef ORIGINAL
    pos = (pos + egg.buf_point) % BUF_SIZE;
#else
    /*
    pos += egg.buf_point;
    while(pos >= BUF_SIZE){
      pos -= BUF_SIZE;
    }
    */
    pos = ((pos + egg.buf_point) & (BUF_SIZE-1));
#endif
  }
  return pos;
}
#else
/* このマクロは引数が複数回評価されます。非常に注意 */
#define get_real_pos(z)  ( ((z)>=egg.buf_leng) ? (-1) : ( (egg.buf_leng==BUF_SIZE) ? (((z) + egg.buf_point) & (BUF_SIZE-1)) : (z) ) )
#endif

#ifdef ORIGINAL1025
LOCAL int get_ring_pos(int pos){
  /* 配列上の位置 pos の buffer 上の位置を返す */
  if (egg.buf_leng == BUF_SIZE){
#ifdef ORIGINAL
    pos = (BUF_SIZE + pos - egg.buf_point) % BUF_SIZE;
#else
    /*
    pos += BUF_SIZE - egg.buf_point;
    while(pos >= BUF_SIZE){
      pos -= BUF_SIZE;
    }
    */
    pos = ((BUF_SIZE + pos - egg.buf_point) & (BUF_SIZE-1));
#endif
  }
  return pos;
}
#else
/* このマクロは引数が複数回評価されます。非常に注意 */
#define get_ring_pos(z)  ((egg.buf_leng==BUF_SIZE) ? ((BUF_SIZE+(z)-egg.buf_point) & (BUF_SIZE-1)) : (z))
#endif

#ifdef ORIGINAL1025
LOCAL int get_buf_e_leng(void){
  /* buffer の要素数を返す */
  return egg.buf_leng;
}
#else
#define get_buf_e_leng()  (egg.buf_leng)
#endif

/********************************************** buffer の仕切り 操作 */
LOCAL int move_part_e(int num){	/* 窓枠の移動数 を返す */
  int dif, i, dum = 0, dum1 = 0;
  for (i = 1; i < num; i++)
    set_tree(egg.part_pos + i, &dum, &dum1);
  egg.part_pos += num;
  dif = egg.part_pos - REG_PART_POS;
  if (dif > 0) {
    egg.part_pos = REG_PART_POS;
  }
  return dif;
}

#ifdef ORIGINAL1025
LOCAL int get_search_code_e(int pos){	/* pos(0~)番目の未符号化記号 を返す */
  return (get_buf_e(egg.part_pos + pos));
}
#else
#define get_search_code_e(z)    (get_buf_e(egg.part_pos + (z)))
#endif

#ifdef ORIGINAL1025
LOCAL int get_part_e(void){ /* 仕切りの位置を返す */
  return egg.part_pos;
}
#else
#define get_part_e()  (egg.part_pos)
#endif

/********************************************** buffer 操作 */

static int buf_remain;		/* buffer 内の記号数 */

LOCAL void pre_set_buf(void){
  int chr;
  chr = get_c();
  if (chr != EOF)
    buf_remain = put_buf_e(chr);
  else
    put_buf_e(-2);
}

LOCAL int set_buf(void){ /* buffer に新たに記号を入力 入力終了かどうかを返す*/
  int chr;
  chr = get_c();
  if (chr != EOF){
    buf_remain = put_buf_e(chr);
  }
  else {
    put_buf_e(-2);
    buf_remain--;
  }
  return chr;
}

LOCAL void init_buf(void){
  /* buffer の初期化 buffer いっぱいに入力する */
  int i;
  for (i = 0; i < BUF_SIZE; i++){
    pre_set_buf();
  }
}

LOCAL void mod_buf(int num){
  /* buffer の更新操作 */
  int i, shift;
  shift = move_part_e(num);
  if (shift > 0)
    del_tree(shift);
  for (i = 0; i < shift; i++)
    set_buf();
}

LOCAL int do_cont(void){  /* 符号化 終了判定 */
  if (get_buf_e(get_part_e()+ 3) != -2)
    return 1;
  else
    return 0;
}

/********************************************** 最長一致記号列探索 */
LOCAL int search(int pos0, int pos1, int *lng){	/* 一致記号数探索 */
  int p0, p1, pe = get_buf_e_leng()- 4, f = 0;

  p0 = get_ring_pos(pos0);
  p1 = get_ring_pos(pos1);
  (*lng) = 0;

  while (p1 < pe &&
	 (egg.now_match_leng + (*lng)) < MAX_MATCH_LENG &&
	 get_buf_e(p0) == get_buf_e(p1) ){
    p0++;
    p1++;
    (*lng)++;
  }
  if (p1 == pe)
    f = 1;			/* 未符号化bufferを超えても一致 */
  else if ((egg.now_match_leng + (*lng)) == MAX_MATCH_LENG)
    f = 2;			/* 上限を超えても一致 */
  return f;
}

LOCAL int set_tree(int pos, int *match_pos, int *match_leng){
  /* 4文字列を2分木に挿入 */
  /* static int ps_back = 0; */
  int i, ret = 1, ps = 0, pe;
  unsigned long x1 = (unsigned long) 0;
  unsigned char x2 = 0;

  pe = get_real_pos(pos);

  for (i = 0; i < 3; i++)
    x1 = x1 * (unsigned long) 256 + (unsigned long) get_buf_e(pos + i);
  x2 = (unsigned char) get_buf_e(pos + 3);

  insert(x1, x2, &T, &ps, pe, &ret, match_leng);

  if ((*match_leng) == 4)
    connect_buf[ret] = pe;
  connect_buf[pe] = -1;

  (*match_pos) = ps;
  return (0);
}

LOCAL void encode(int ps, int pe){ /* 最長一致記号列探索 */
  int f, ff, match_pos, leng = 0, lng = 0, dum = 0, dum1 = 0, p_pos;

  egg.now_match_leng = 0;
  f = search(ps, pe, &leng);
  match_pos = get_ring_pos(ps);
  switch (f) {
  case 0:			/* 最長一致がバッファ内で収まる */
    ps = connect_buf[ps];
    while (ps != pe) {
      ff = search(ps, pe, &lng);
      if (leng <= lng) {
	leng = lng;
	match_pos = get_ring_pos(ps);
      }
      ps = connect_buf[ps];
    }
    put_code(1, match_pos, leng, get_part_e());
    mod_buf(leng);
    break;

  case 1:			/* 最長一致が未符号化bufferを超える */
    p_pos = get_part_e();
    egg.now_match_leng = leng;
    ps = (ps + leng) % BUF_SIZE;
    pe = (pe + leng) % BUF_SIZE;
    mod_buf(leng);
    while ((ff = search(ps, pe, &lng)) != 0) {
      leng += lng;
      egg.now_match_leng += lng;
      ps = (ps + lng) % BUF_SIZE;
      pe = (pe + lng) % BUF_SIZE;
      set_tree(get_part_e(), &dum, &dum1);
      mod_buf(lng);
      if (ff == 2) {
	put_code(1, match_pos, leng, p_pos);
	p_pos = get_part_e();
	leng = 0;
	egg.now_match_leng = 0;
	match_pos = get_ring_pos(ps);
      }
    }
    leng += lng;
    put_code(1, match_pos, leng, p_pos);
    if (lng != 0)
      set_tree(get_part_e(), &dum, &dum1);
    mod_buf(lng);
    break;

  case 2:			/* 一致長が上限を超える */
    put_code(1, match_pos, leng, get_part_e());
    ps = (ps + leng) % BUF_SIZE;
    pe = (pe + leng) % BUF_SIZE;
    mod_buf(leng);
    p_pos = get_part_e();
    match_pos = get_ring_pos(ps);
    leng = 0;
    while ((ff = search(ps, pe, &lng)) != 0) {
      leng += lng;
      egg.now_match_leng += lng;
      ps = (ps + lng) % BUF_SIZE;
      pe = (pe + lng) % BUF_SIZE;
      set_tree(get_part_e(), &dum, &dum1);
      mod_buf(lng);
      if (ff == 2) {
	put_code(1, match_pos, leng, p_pos);
	p_pos = get_part_e();
	leng = 0;
	egg.now_match_leng = 0;
	match_pos = get_ring_pos(ps);
      }
    }
    leng += lng;
    put_code(1, match_pos, leng, p_pos);
    if (lng != 0)
      set_tree(get_part_e(), &dum, &dum1);
    mod_buf(lng);
    break;
  }
}

LOCAL void del_tree(int shift){
  /* 2分木からbuffer 先頭4記号の列をshift 個削除する */
  unsigned long x1 = 0;
  unsigned char x2 = 0;
  int i, j, next;

  for (i = 0; i < shift; i++) {
    for (j = 0; j < 3; j++)
      x1 = x1 * 256 + (unsigned long) get_buf_e(i + j);
    x2 = (unsigned char) get_buf_e(i + 3);

    next = connect_buf[get_real_pos(i)];
    connect_buf[get_real_pos(i)] = -2;

    deleteo(x1, x2, &T, next);
    x1 = 0;
  }
}


/*-------------------------------------------------------------------------*
 * srcbufからsrclenバイトのデータをエンコードしてdestbufから始まる領域に
 * 格納し、そのバイト数をdestlenに格納します。keyString と keyLength は
 * 暗号化キーとその長さ(バイト数)です。キーの長さに制限はありませんが、
 * 5～10バイト程度を想定しています。
 * destbuf から後には必要なだけ格納用の領域を確保しておく必要があります。
 * destlen は初期化してない状態でも構いません。
 * JPEG画像などデータによっては元よりもデータサイズが増える場合もあるので、
 * destbufに確保しておく領域のバイト数は srclen の倍程度がおすすめです。
 * (確保しているサイズを越えたかどうかのチェックなどはいっさいしません)
 * 結果的にもとより(データのみの)サイズが増える場合には、圧縮せずに
 * ヘッダを前につけ、データをバイト単位のエンコードをかけてコピーして
 * 返します。この場合はdestlenはsrclenよりヘッダのサイズだけ増えます。
 *-------------------------------------------------------------------------*/
int EncodeData(char* destbuf, char* srcbuf, long* destlen, long srclen,
	       char* keyString, int keyLength){
  int i = 0, chr;
  int s_c, match_pos = 0, match_leng;

  T = NULL;
  BitioInit((unsigned char* )keyString, keyLength);
#ifdef USE_HASH
  hashinit();
#endif

  Currenti = (unsigned char* )srcbuf;
  InLen = 0;
  InTotalBytes = srclen;

#ifdef INCLUDE_SIZE_INTO_DATA
  {
    unsigned char* q = (unsigned char* )destbuf;
#if 1
    unsigned char* p = (unsigned char* )ID_STRING; /* 識別用文字列を埋め込み */
    for(; *p; *q++ = *p++){ ; }
    *q++ = ID_COMPRESSED; /* 「圧縮されている」フラグ */
#endif
    /* サイズ情報の埋め込み */
    *q++ = (unsigned char)((srclen >> 24) & 0xFF);
    *q++ = (unsigned char)((srclen >> 16) & 0xFF);
    *q++ = (unsigned char)((srclen >>  8) & 0xFF);
    *q++ = (unsigned char)((srclen      ) & 0xFF);
    /* 順番を変えたりしてもOK(デコード側と整合性をとること) */
  }
  Currentp = (unsigned char* )(destbuf + HEADER_SIZE);
  OutLen = HEADER_SIZE;
#else
  Currentp = (unsigned char* )destbuf;
  OutLen = 0;
#endif

  /* 最初 destlen は適当な値(領域のサイズ)を指定しておけばOK */
  /* ただし、JPEG画像などもともとランダム性の高いデータの場合 */
  /* 元よりサイズが増えてしまうことが多いので、最大２倍は */
  /* とっておくことにします。 */

  initEncode();
  init_buf();

  set_tree(0, &match_pos, &match_leng);
  put_code(0, get_buf_e(0), 0, 0); /* 最初の記号は無条件に記号の符号化 */
  mod_buf(1);

  /* fprintf(stderr, "OK, entering loop...\n"); */
  while (do_cont()){
    match_leng = 0;		/* 最長一致記号数 */
    egg.now_match_leng = 0;
    match_pos = 0;		/* 最長一致記号位置 */

    s_c = get_search_code_e(0);	/* s_c は 検索記号 */

    set_tree(get_part_e(), &match_pos, &match_leng);	
    /* 仕切りの記号を符号化 */

    if (match_leng < 4) {	/* 不一致符号化 */
      put_code(0, s_c, 0, get_part_e());
      mod_buf(1);
    }
    else{
      encode(match_pos, get_real_pos(get_part_e()));
    }
  }
  i = 0;			/* 有効一致記号数未満の残り */
  {
    while ((chr = get_search_code_e(i)) != -2) {
      i++;
      Currentp = bit_out(Currentp, 0, & egg.flush_need, & OutLen);
      fit_code(chr, 256);
    }
  }
  if (egg.flush_need != 0){
    flush(Currentp, & OutLen);
  }
  * destlen = OutLen;
  tfree(T);
/*  fprintf(stderr, "malloc=%d, free=%d\n", malloc_count, free_count);*/
#ifdef DEBUG_HASH
  fprintf(stderr, "hash hit:%ld, unhit:%ld (%6.2f%%) unused = %d colli:%ld\n",
	  hitctr, unhitctr, (double)(hitctr)*100.0/(hitctr+unhitctr),
	  getUnusedCells(), colctr);
#endif
  fprintf(stderr, "%ld => %ld bytes\n", srclen, * destlen);

#ifdef INCLUDE_SIZE_INTO_DATA
  /* もとよりサイズが増えた場合、圧縮(じつは増えてる)データを破棄し、*/
  /* もとのデータにエンコードだけかけます。 */
  if(* destlen > srclen + HEADER_SIZE){
    unsigned char* p, * q;
    long i;
    fprintf(stderr, "Ouch. size increased => non-compression mode.\n");
    /* 「圧縮されていない」フラグ */
    * (destbuf + ID_STRING_LEN) = ID_NOT_COMPRESSED;
    Currentp = (unsigned char* )(destbuf + HEADER_SIZE);
    * destlen = srclen + HEADER_SIZE;
    q = (unsigned char* )destbuf + HEADER_SIZE;
    BitioInit((unsigned char* )keyString, keyLength);
    for(p=(unsigned char* )srcbuf,i=0; i<srclen; i++,p++,q++){
      *q = hoencrypt(*p, (unsigned char* )keyString, keyLength);
    }
    fprintf(stderr, "%ld => %ld bytes\n", srclen, * destlen);
  }
#endif

  return 1;
}



/****************************************************************************
 *--------------------------------------------------------------------------*
 * メインの解凍はここから
 *--------------------------------------------------------------------------*
 ****************************************************************************/

#define DOUT fprintf
#define BITIN(a)  bit_in(Currenti, (a), & InLen)

typedef struct {
  int start_pos; /* put_buf_d()参照(0) */
  int end_pos;   /* put_buf_d()参照(0) */
  int part_pos;  /* move_part_d()参照(0) */
} deGSTATIC;

static deGSTATIC dgg;
#if 0 /* 再定義エラーを避ける */
static unsigned char* Currenti, * Currento;
static long InLen, OutLen;
static long InTotalBytes, OutTotalBytes;
#else
static unsigned char* Currento;
static long OutTotalBytes;
#endif
LOCAL int get_head_bit(void);
LOCAL int get_match_pos(int part);
LOCAL int get_match_pos2(int part);
LOCAL int get_match_leng(void);
LOCAL int get_chr_code(void);
LOCAL void put_txt(int chr);
LOCAL int put_buf_d(int chr);
LOCAL int get_buf_d(int num);
LOCAL void shift_buf(int num);
LOCAL int move_part_d(int num);
LOCAL int get_search_code_d(int pos);
LOCAL int get_part_d(void);
LOCAL int decode(void);


/*--------------------------------------------------------------------------*
 * 広域データを初期化します。DecodeData()に呼ばれます。
 *--------------------------------------------------------------------------*/
LOCAL void initDecode(void){
  dgg.start_pos = 0;
  dgg.end_pos = 0;
  dgg.part_pos = 0;
}


LOCAL int get_head_bit(void){
  /* 先頭１bit 読込み */
  int c;
  Currenti = BITIN(& c);

  if(InLen >= InTotalBytes || c < 0){
    return -1;
  }
  return c;
}


LOCAL int get_match_pos(int part){
  /* 一致列位置読込み 引数は符号化時の位置範囲 */
  int filt = 1;
  int code = 0, bit;

  while (filt < part) {
    filt = filt << 1;
    Currenti = BITIN(& bit);
    code = (code << 1) + bit;
  }
  return code;
}

LOCAL int get_match_pos2(int part){
  int filt = 1;
  int code = 0, bit;
  while ((filt << 1) < part) {
    filt = filt << 1;
    Currenti = BITIN(& bit);
    code = (code << 1) + bit;
  }
  filt = filt << 1;
  if (code >= filt - part) {
    Currenti = BITIN(& bit);
    code = (code << 1) + bit;
    code = code - filt + part;
  }
  return code;
}

LOCAL int get_match_leng(void){
  /* 一致列長 読込み */
  int i, leng = 0, code = 1, bit;

  while(1){
    Currenti = BITIN(& bit);
    if(bit != 0) break;
    leng++;
  }

  for (i = 0; i < leng; i++) {
    Currenti = BITIN(& bit);
    code = (code << 1) + bit;
  }
  return (code + 3);
}

LOCAL int get_chr_code(void){
  /* 記号 読込み */
  int i, bit, code = 0;
  for (i = 0; i < 8; i++) {
    Currenti = BITIN(& bit);
    if(InLen >= InTotalBytes || bit < 0){
      return (-1);
    }
    else{
      code = (code << 1) + bit;
    }
  }
  return code;
}

LOCAL void put_txt(int chr){  /* 復号記号 出力操作 */
  if(OutTotalBytes <= 0 || OutLen < OutTotalBytes){
    OutLen++;
    * Currento++ = chr;
  }
}

/********************************************** リング buffer 操作 */

#if 0
static int buf[BUF_SIZE];
/* エンコード側でも同じ名前で使われていた。サイズも同じなので使いまわしちゃるっす */
#endif
#if 0
static int start_pos = 0, end_pos = 0;
#endif

LOCAL int put_buf_d(int chr){
  /* バッファの残り数を返す */
  int rem;
  buf[dgg.end_pos] = chr;
  dgg.end_pos = (dgg.end_pos + 1) % BUF_SIZE;
  rem = dgg.start_pos - dgg.end_pos;
  if (rem < 0)
    rem = BUF_SIZE + rem;
  return (rem);
}

LOCAL int get_buf_d(int num){	/* num番目のデータを返す */
  int chr = -1;
  int pos = (dgg.start_pos + num) % BUF_SIZE;
  int leng = (BUF_SIZE + dgg.end_pos - dgg.start_pos) % BUF_SIZE;
  if (leng == 0)
    leng = BUF_SIZE;
  if (num < leng)
    chr = buf[pos];
  return (chr);
}

LOCAL void shift_buf(int num){	/* バッファの内容を左へnum個シフトする */
  dgg.start_pos = (dgg.start_pos + num) % BUF_SIZE;
}

/********************************************** buffer の仕切り 操作 */
#if 0
static int part_pos = 0;		/* 仕切りの位置 */
#endif
LOCAL int move_part_d(int num){
    /* 仕切りを右へnum個移動する 移動できなかった個数を返す*/
    int dif, i;
    for (i = 0; i < num; i++)
	put_txt(get_buf_d(dgg.part_pos + i));
    dgg.part_pos += num;
    dif = dgg.part_pos - REG_PART_POS;
    if (dif > 0) {
	dgg.part_pos = REG_PART_POS;
	shift_buf(dif);
    }
    return (dif);
}

LOCAL int get_search_code_d(int pos)
{				/* pos(0~)番目の未符号化文字を返す */
    return (get_buf_d(dgg.part_pos + pos));
}

LOCAL int get_part_d(void)
{				/* 仕切りの位置を返す */
    return (dgg.part_pos);
}

/********************************************** 復号化 */

LOCAL int decode(void){
  int head, pos, leng, chr, i, rem, lng, sft;

  head = get_head_bit();

  if (head == -1)
    return (-1);		/* 読込み終了なら復号化終了 */
  else if (head == 0) {
    chr = get_chr_code();	/* 記号の復号 */
    if (chr == -1)
      return (-1);
    rem = put_buf_d(chr);
    move_part_d(1);
  }
  else {
    pos = get_match_pos2(get_part_d());	/* 一致記号列の復号 */
    leng = get_match_leng();
    lng = leng;
    sft = 0;
    
    for (i = 0; i < leng; i++) {
      rem = put_buf_d(get_buf_d(pos + i));
      if (rem == 0) {
	move_part_d(sft);
	pos = pos - sft;
	lng = lng - sft;
	sft = 0;
      }
      sft++;
    }
    move_part_d(lng);
  }
  return 1;
}


/*--------------------------------------------------------------------------*
 * pから始まるデータ領域がこのルーチンを通っているかどうかを返します。
 *--------------------------------------------------------------------------*/
int IsPassedCompression(char* p){
  return ( (memcmp(p, ID_STRING, ID_STRING_LEN) == 0) ? 1 : 0 );
}

/*--------------------------------------------------------------------------*
 * pから始まるデータ領域が圧縮されているかどうかを返します。
 *--------------------------------------------------------------------------*/
int IsCompressed(char* p){
  return (  (* (p+ID_STRING_LEN) == ID_COMPRESSED) ? 1 : 0 );
}

/*--------------------------------------------------------------------------*
 * エンコードされたデータ列の先頭を指すポインタpからそのデコード後のデータ
 * サイズを算出して返します。
 *--------------------------------------------------------------------------*/
long GetOriginalDataSize(char* p){
  long destlen;
  p += SIZE_INFO_OFFSET;
  destlen = ((((long)p[0]) << 24) & 0xFF000000) +
            ((((long)p[1]) << 16) & 0x00FF0000) +
            ((((long)p[2]) <<  8) & 0x0000FF00) +
            ((((long)p[3])      ) & 0x000000FF);
  return destlen;
}


/*-------------------------------------------------------------------------*
 * srcbufからsrclenバイトのデータをデコードしてdestbufから始まる領域に
 * 格納し、そのバイト数をdestlenに格納します。keyString と keyLength は
 * 暗号化キーとその長さ(バイト数)です。キーの長さに制限はありませんが、
 * 5～10バイト程度を想定しています。
 * destbuf から後には必要なだけ格納用の領域を確保しておく必要があります。
 * destbufとdestlenを適切に初期化しておく
 * 方法は2つあり、1つは先に destlen = GetOriginalDataSize()関数を使って
 * 元データのサイズを取得しておき、その大きさぴったりの領域を
 * destbuf = malloc(destlen) で確保し、そのままのdestbufとdestlenを
 * 使うことです。2つめは適当に大きなサイズの領域を destbuf に確保しておき、
 * destlen には **正でない** 適当な値を入れて使うことです。後者の場合は
 * 正常終了ならデコードされた元データのサイズが destlen に返って来ますが、
 * 元データのサイズが驚異的に大きかった場合には、データが入り切らずに異常終了
 * する危険があります。
 * (確保しているサイズを越えたかどうかのチェックなどはいっさいしません)
 *-------------------------------------------------------------------------*/
int DecodeData(char* destbuf, char* srcbuf, long* destlen, long srclen,
	       char* keyString, int keyLength){
#ifdef INCLUDE_SIZE_INTO_DATA
  Currenti = (unsigned char* )(srcbuf + HEADER_SIZE);
  InTotalBytes = srclen - HEADER_SIZE;
#else
  Currenti = (unsigned char* )srcbuf;
  InTotalBytes = srclen;
#endif
  Currento = (unsigned char* )destbuf;
  InLen = 0;
  OutLen = 0;
  OutTotalBytes = * destlen;
  /* OutTotalBytes ... see put_txt() */
  BitioInit((unsigned char* )keyString, keyLength);

#ifdef INCLUDE_SIZE_INTO_DATA
  if(* (srcbuf+ID_STRING_LEN) == ID_NOT_COMPRESSED){
    /* 圧縮されていない */
    unsigned char* p, * q;
    long i;
    * destlen = srclen - HEADER_SIZE;
    p = (unsigned char* )srcbuf + HEADER_SIZE;
    q = (unsigned char* )destbuf;
    for(i=0; i<*destlen; i++,p++,q++){
      *q = hoencrypt(*p, (unsigned char* )keyString, keyLength);
    }
  }
  else{
    initDecode();
    while(decode()!= -1){ ; }
  }
#else
  initDecode();
  while(decode()!= -1){ ; }
#endif

#ifdef INCLUDE_SIZE_INTO_DATA
  /*
    destlenが不明な場合に限り、ここで取得して返してやります。
    OutLenは全く使いません。
   */
  if(* destlen <= 0){
    * destlen = GetOriginalDataSize(srcbuf);
  }
#else
  * destlen = OutLen;
#endif

  fprintf(stderr, "%ld => %ld bytes\n", srclen, * destlen);
  return 1;
}

/* END */
