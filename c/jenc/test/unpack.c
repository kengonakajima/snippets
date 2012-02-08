/*---------------------------------------------------------------------------*
 * unpack.cpp
 * 98-11-04 by eikeda@titan.co.jp
 *
 * TODO
 * * �T�C�Y��������ꍇ�ɖ����k�ϊ�(�G���R�[�h�͂���)
 * * ==> ���肵�ăf�R�[�h
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #define DEBUG */
#define LOCAL static
#define EXPORTED

#define INCLUDE_SIZE_INTO_DATA /* �T�C�Y�����f�[�^�̐擪�ɓ���� */
#define ID_STRING          "DaMe"
#define ID_STRING_LEN      4
#define ID_COMPRESSED      '8'
#define ID_NOT_COMPRESSED  '9'
#define SIZE_INFO_OFFSET   5 /* ID_STRING�̒����A���k�t���O1 */
#define HEADER_SIZE        9 /* ID_STRING�̒����A���k�t���O1�A�T�C�Y���4 */

/*-------------------------------------------------------------------------*
 * srcbuf����srclen�o�C�g�̃f�[�^���G���R�[�h����destbuf����n�܂�̈��
 * �i�[���A���̃o�C�g����destlen�Ɋi�[���܂��BkeyString �� keyLength ��
 * �Í����L�[�Ƃ��̒���(�o�C�g��)�ł��B�L�[�̒����ɐ����͂���܂��񂪁A
 * 5�`10�o�C�g���x��z�肵�Ă��܂��B
 * destbuf �����ɂ͕K�v�Ȃ����i�[�p�̗̈���m�ۂ��Ă����K�v������܂��B
 * destlen �͏��������ĂȂ���Ԃł��\���܂���B
 * JPEG�摜�Ȃǃf�[�^�ɂ���Ă͌������f�[�^�T�C�Y��������ꍇ������̂ŁA
 * destbuf�Ɋm�ۂ��Ă����̈�̃o�C�g���� srclen �̔{���x���������߂ł��B
 * (�m�ۂ��Ă���T�C�Y���z�������ǂ����̃`�F�b�N�Ȃǂ͂����������܂���)
 *-------------------------------------------------------------------------*/
EXPORTED int EncodeData(char* destbuf, char* srcbuf, long* destlen, long srclen,
	       char* keyString, int keyLength);

/*--------------------------------------------------------------------------*
 * �G���R�[�h���ꂽ�f�[�^��̐擪���w���|�C���^p���炻�̃f�R�[�h��̃f�[�^
 * �T�C�Y���Z�o���ĕԂ��܂��B
 *--------------------------------------------------------------------------*/
EXPORTED long GetOriginalDataSize(char* srcbuf);

/*-------------------------------------------------------------------------*
 * srcbuf����srclen�o�C�g�̃f�[�^���f�R�[�h����destbuf����n�܂�̈��
 * �i�[���A���̃o�C�g����destlen�Ɋi�[���܂��BkeyString �� keyLength ��
 * �Í����L�[�Ƃ��̒���(�o�C�g��)�ł��B�L�[�̒����ɐ����͂���܂��񂪁A
 * 5�`10�o�C�g���x��z�肵�Ă��܂��B
 * destbuf �����ɂ͕K�v�Ȃ����i�[�p�̗̈���m�ۂ��Ă����K�v������܂��B
 * destbuf��destlen��K�؂ɏ��������Ă���
 * ���@��2����A1�͐�� destlen = GetOriginalDataSize()�֐����g����
 * ���f�[�^�̃T�C�Y���擾���Ă����A���̑傫���҂�����̗̈��
 * destbuf = malloc(destlen) �Ŋm�ۂ��A���̂܂܂�destbuf��destlen��
 * �g�����Ƃł��B2�߂͓K���ɑ傫�ȃT�C�Y�̗̈�� destbuf �Ɋm�ۂ��Ă����A
 * destlen �ɂ� **���łȂ�** �K���Ȓl�����Ďg�����Ƃł��B��҂̏ꍇ��
 * ����I���Ȃ�f�R�[�h���ꂽ���f�[�^�̃T�C�Y�� destlen �ɕԂ��ė��܂����A
 * ���f�[�^�̃T�C�Y�����ٓI�ɑ傫�������ꍇ�ɂ́A�f�[�^������؂炸�Ɉُ�I��
 * ����댯������܂��B
 * (�m�ۂ��Ă���T�C�Y���z�������ǂ����̃`�F�b�N�Ȃǂ͂����������܂���)
 *-------------------------------------------------------------------------*/
EXPORTED int DecodeData(char* destbuf, char* srcbuf, long* destlen, long srclen,
	       char* keyString, int keyLength);


/* �������̐錾 */
#define BUF_SIZE       8192 /* buf_size = buffer�̎Q�ƕ�+ buffer�̕������� */
#define HASH_SIZE     32728 /* insert()�Q�ƁB�G���R�[�h���̂ݎg�p */
/* ���݂͕K��2�ׂ̂���ɂ��邱�� */
#if 0
#define BUF_SIZE_EXP     13 /* log_2 8192 */
#endif

/*--------------------------------------------------------------------------*
 * ���b�`������! Windows�ł̓R���p�C��(VC++)���ւ�ȍœK�������Ă��܂�
 * �悤�Ȃ̂ŁA��΂�2�ׂ̂���ɂ��Ă͂����܂���B
 *--------------------------------------------------------------------------*/
#if 1
#define REG_PART_POS   2190 /* buffer �̎Q�ƕ��̍ő�̈� */
#define MAX_MATCH_LENG 2190 /* �Œ���v�n�񒷂̏�� */
#else
#define REG_PART_POS   4096 /* buffer �̎Q�ƕ��̍ő�̈� */
#define MAX_MATCH_LENG 4096 /* �Œ���v�n�񒷂̏�� */
#endif



/*****************************************************************************
 *---------------------------------------------------------------------------*
 * �ŏ�ʊ֐��͂���ł��B
 * mode�͗낪�G���R�[�h(���k)�A�P���f�R�[�h(�W�J)�ł��B
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
    /* �o�̓f�[�^�T�C�Y�����肵�܂��B */
    if(mode == 1){
        destlen = GetOriginalDataSize(p_in);
    }
    else{
        destlen = srclen * 2;
    }
    /* �o�̓f�[�^�p�̈���m�� */
    if((p_out = (char* )malloc(destlen)) == NULL){
        fprintf(stderr, "Error: out of memory.\n");
        free(p_in);
        return 0;
    }
    /* ���k�G���R�[�h���f�R�[�h���s */
    if(mode == 1){
        DecodeData(p_out, p_in, & destlen, srclen, key, keylen);
    }
    else{
        EncodeData(p_out, p_in, & destlen, srclen, key, keylen);
    }
    /* ���̗̈�ɏ����߂��܂��B */
    * bytelength = destlen;
    memcpy(bytedata, p_out, destlen);
    free(p_out);
    return 1;
}





/*****************************************************************************
 *---------------------------------------------------------------------------*
 * �r�b�gI/O�Ƃ��ȒP�ȈÍ����Ƃ�
 * �Í����͊�{���̊�{�A�r���_���a���g���������Ε����A�ΏƂ̃��`���P���Ȃ��̂ł��B
 * �����ƕ��G�ŕ��ׂ�������Ȃ���͂Ȃ����Ȃ�
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
 * �ÓI�œ����I�ȕϐ������������܂��B
 * �����ɂQ�X���b�h�ȏ�ň��k�W�J���邱�Ƃ��l���Ă��܂���B
 * EncodeData()��DecodeData()�ŏ����J�n�O�ɕK���Ă΂Ȃ��ƂQ��ڈȍ~�̓G���[�ɂȂ邼��
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
 * ���ɊȒP�ȈÍ���
 * �����ƍH�v�\��ł��B�ʂɃG���R�[�h�ƃf�R�[�h�ňႤ�֐��ɂ��Ă������̂����A
 * ����ł͂��܂�Ӗ�������܂���B
 * bit_in(), bit_out() �ɌĂ΂�܂��B
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
 * p ����1�r�b�g(���ۂɂ�1�o�C�g�P�ʂ�)�ǂݐi�߂܂��B
 * �ŁA�|�C���^��Ԃ��A�ǂ񂾃r�b�g�� v �Ɋi�[�B�ǂ񂾑��o�C�g���� lenp
 * �ɒ~�ς��܂��B
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
    /* * lenp ���̂܂� */
    gg.infilt = gg.infilt >> 1;
  }
  * v = ((gg.inbyte & gg.infilt) == 0) ? 0 : 1;
  return q;
}


/*---------------------------------------------------------------------------*
 * p����Pbit�����������߂܂��B�r�b�g�P�ʂŌĂ΂�܂������ۂ͂Pbyte�ɂȂ������_��
 * �o�b�t�@�ɏ����o���A�|�C���^��i�߂܂��B 
 * ���܂��Ă���bit����tame�ɕԂ��܂��Blenp�͌��݂܂łɏ������񂾃o�C�g�����Ǘ�����
 * �ϐ��ւ̃|�C���^�ł��B
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
    /* * lenp ���̂܂�  */
    q = p;
  }
  * tame = gg.outcnt;
  return q;
}


/*---------------------------------------------------------------------------*
 * �ǂݍ��ݏI�����Ɏc���Ă���r�b�g���͂������܂��B
 * EncodeData()�ɌĂ΂�܂��B
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
 * ASCII�G���R�[�h(BASE64�݂����Ȃ�)���s�Ȃ��܂��B
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
 * �����̐ÓI�ȕϐ������������܂��B�����O�ɕK���Ă΂Ȃ��Ƃ����܂���B
 *---------------------------------------------------------------------------*/
LOCAL void Uuen_Init(void){
  hgg.infilt = 1;
  hgg.inbyte = 0;
  hgg.outcnt = 0;
  hgg.outbyte = 0;
}

/*---------------------------------------------------------------------------*
 * ASCII�G���R�[�h�P�r�b�g�ǂݍ���
 *---------------------------------------------------------------------------*/
LOCAL unsigned char* uubit_in(unsigned char* p, int* v, long* lenp){
  unsigned char* q;

  if(p == NULL){
    /* static�ϐ������������� */
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
    /* * lenp ���̂܂� */
    hgg.infilt = hgg.infilt >> 1;
  }
  * v = ((hgg.inbyte & hgg.infilt) == 0) ? 0 : 1;
  return q;
}


/*---------------------------------------------------------------------------*
 * ASCII�G���R�[�h�P�r�b�g��������(���ۂɂ�1�o�C�g�P��)
 * ���܂��Ă���r�b�g����tame�ɕԂ��܂��B
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
    /* * lenp ���̂܂�  */
    q = p;
  }
  * tame = hgg.outcnt;
  return q;
}

#if 0
/* �Ǎ��ݏI�����Ɏc���Ă���bit��|���o�� */
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
 * ���C���̈��k�͂�������
 *--------------------------------------------------------------------------*
 ****************************************************************************/
static int malloc_count = 0, free_count = 0; /* �f�o�b�O�p������ */
struct node {
    unsigned long key1;	/* �擪3�����̃R�[�h */
    unsigned char key2;	/* �c��1�����̃R�[�h */
    unsigned short key1_0ff0; /* key1 & 0x0ff0 �̒l�Binsert_slave�Q�� */
    struct node *left;
    struct node *right;
    int buf_start_pos;
    int buf_end_pos;	/* �����Ƃ��ŋ߉�����ꂽ�����̐�΃o�b�t�@�ʒu */
};

typedef struct node Tree;

typedef struct {
  /*int pos_code_filt;*/  /* pos_code()���Q��(�����l1) */
  int pos_code2_filt; /* pos_code2()���Q��(1) */
  int flush_need;     /* �t�@�C���o�͎���bit�|���̂Ă̗L��(0) */
  int get_c_chr;      /* get_c()�Q��(0) */
  int buf_point;      /* �z��buf[]�̐�Έʒu(0) */
  int buf_leng;       /* �z��buf[]�̓��͋L����(0) */
  int part_pos;       /* �d�؂�̈ʒu(move_part_e()�Q��)(0) */
  int now_match_leng; /* search()�Q��(0) */
  /*int set_tree_ps_back;*/ /* set_tree()�Q��(0) */
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
 * �L��f�[�^�����������܂��BEncodeData()�ɌĂ΂�܂��B
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
 * ����bin_code.c���������[�`��
 * ����͊ȒP�Ȑ����x�[�X�̑ΐ��̌v�Z�ł��B
 *--------------------------------------------------------------------------*/
LOCAL int I_log(int num, int *filt){
  /* �����p�ΐ� log2 num ��Ԃ� filt=2^(log2 num) */
  int i = 0;
  (*filt) = 1;
  for (i = 0; (*filt) <= num; i++)
    *filt = (*filt) << 1;
  return (i);
}


/*--------------------------------------------------------------------------*
 * �r�b�g��filt�ŕ���code�𕄍������܂��B
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
  /* ��v�ʒu�̕����� */
  /* static int filt = 1; */
  int leng = p_pos;

  while (egg.pos_code_filt < leng){
    egg.pos_code_filt = egg.pos_code_filt << 1;
  }
  fit_code(code, egg.pos_code_filt);
}
#endif /* deprecated */

/*--------------------------------------------------------------------------*
 * ��v�ʒu�𕄍������܂�(���̂Q�A�^��)
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
 * ��v�L�����𕄍������܂��B
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
 * ���� tree.c ����������
 * �����Tree�^�̃Z��������ĕԂ��֐��ł��B
 *--------------------------------------------------------------------------*/
LOCAL Tree *talloc(void){
  malloc_count++;
  return ((Tree *) malloc(sizeof(Tree)));
}

/*--------------------------------------------------------------------------*
 * �������������֐��ł��B
 *--------------------------------------------------------------------------*/
LOCAL void tfree(Tree* p){
  if(p->left != NULL)  tfree(p->left);
  if(p->right != NULL) tfree(p->right);
  free_count++;
  free(p);
}

/*--------------------------------------------------------------------------*
 * �񕪖؂̑}������
 *--------------------------------------------------------------------------*/
#ifdef ORIGINAL
LOCAL void insert(unsigned long x1, unsigned char x2, Tree** p,
                  int* b_ps, int b_pe, int* ret, int* leng) /* 4�L���̑}�� */
/* ret�͂���܂ł̒��� */
/* �����Ƃ��ŋ߂̂��̋L���̐�΃o�b�t�@�ʒu */
/* �V�K�Ȃ� -1 */
/* leng�͈�v�L����(2~4) */
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
	    (*b_ps) = (*p)->buf_start_pos;	/* 3�L����v */
	    if (x2 == (*p)->key2) {	/* 4�L����v */
		(*leng) = 4;
		(*ret) = (*p)->buf_end_pos;
		(*p)->buf_end_pos = b_pe;
	    } else if (x2 < (*p)->key2)
		insert(x1, x2, &((*p)->left), b_ps, b_pe, ret, leng);
	    else
		insert(x1, x2, &((*p)->right), b_ps, b_pe, ret, leng);
	} else {
	    if ((x1 & 0x0ff0) == ((*p)->key1 & 0x0ff0)) {
	      /* 2�L���̈�v */
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
/* �n�b�V���e�[�u���B���X�g�Ԃ炳����^�ł͂Ȃ��̂ŁA�Փ˂���Ə㏑������� */
static HTABLE htable[HASH_SIZE];

#ifdef DEBUG_HASH
/* �f�o�b�O�p�B�n�b�V���Ƀq�b�g�����񐔁A�O�ꂽ�񐔂𐔂��� */
static long hitctr, unhitctr;
/* �f�o�b�O�p�B�Փ˂̔����񐔂𐔂��� */
static long colctr;
#endif
/* �n�b�V���֐� */
static unsigned short hashfunc(unsigned long x1, unsigned char x2){
#if 1 /* �ȒP������(�q�b�g��30%���x�A�g�p�Z����400) */
  return (unsigned short)((x1>>5+x2) & ((unsigned long)(HASH_SIZE-1)));
#else /* �Փ˂͌��邪�A���G�Ȃ̂Ŏ��Ԃ͂����� */
  unsigned long r;
  r = ( ((x1 & 0xFFFF0000) >> 15) + (x1 & 0x0000FFFF)
    + ((unsigned long)x2 << 2) );
  return (unsigned short) (r & (HASH_SIZE-1));
#endif
}
/* �������B�K�v */
static void hashinit(void){
  int i;
  for(i=0; i<HASH_SIZE; i++){
    htable[i].treep = NULL;
    htable[i].x1 = (unsigned long)(-1); /* �K���Ȑ� */
    htable[i].x2 = (unsigned char)(-1);
  }
#ifdef DEBUG_HASH
  hitctr = 0; unhitctr = 0; colctr = 0;
#endif
}

#ifdef DEBUG_HASH
/* �f�o�b�O�p�B�g���Ȃ������Z���̐���Ԃ� */
static int getUnusedCells(void){
  int i;
  int ctr = 0;
  for(i=0; i<HASH_SIZE; i++){
    if(htable[i].treep == NULL){ ctr++; }
  }
  return ctr;
}
#endif

/* �n�b�V���ʒu�ɓ����f�[�^������΁A����treep�����o��Ԃ��B */
/* �����Z�b�g����ĂȂ����A�����ƈႤ�̂��Z�b�g����Ă�����A */
/* NULL��Ԃ��B */
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
/* deleteo�ō폜���ꂽ(x1,x2)���n�b�V��������폜 */
/* (�_�~�[�l�����ă}�b�`���Ȃ��悤�ɂ���) */
static void hashunset(unsigned long x1, unsigned char x2){
  int index = hashfunc(x1, x2);
  htable[index].x1 = (unsigned long)(-1);
  htable[index].x2 = (unsigned char)(-1);
}
/* ��ɏ㏑������ */
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


/* �L���̑}���B�����Ƃ����Ԃ�H���֐� */
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
    /* ����while���[�v�́u�S�L����v�v�̏������o�C�p�X���s����̂� */
    (*b_ps) = (*p)->buf_start_pos;
    (*leng) = 4;
    (*ret) = (*p)->buf_end_pos;
    (*p)->buf_end_pos = b_pe;
    return; /* �������� */
#endif
  }
#ifdef DEBUG_HASH
  else{
    unhitctr++;
  }
#endif
#endif /* USE_HASH */

  while(*p != NULL){
    /* �؂����ǂ� */
    if (x1 == (*p)->key1) { /* 3�L���ȏ��v���Ă��� */
      (*leng) = 3;
      (*b_ps) = (*p)->buf_start_pos; /* 3�L����v */

      if(x2 < (*p)->key2){
        p = & ((*p)->left); /* ���̎} */
      }
      else if(x2 > (*p)->key2){
        p = & ((*p)->right); /* �E�̎} */
      }
      else{
	/* 4�L����v */
	(*leng) = 4;
	(*ret) = (*p)->buf_end_pos;
	(*p)->buf_end_pos = b_pe;
#ifdef USE_HASH
	if(! hashgotf){
	  hashsetp(*p, x1, x2);
	}
#endif
	return; /* �������� */
      }
    }
    else {
      if (x1_0ff0 == (*p)->key1_0ff0){ /* 2�L���̈�v */
	(*leng) = 2;
	(*b_ps) = (*p)->buf_start_pos;
      }
      if (x1 < (*p)->key1) p = & ((*p)->left);  /* ���̎} */
      else                 p = & ((*p)->right); /* �E�̎} */
    }
  }

  /* *p��NULL�̂Ƃ��܂ŗ����� */

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


/* 4�L���̍폜 */
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
    /* �؂����ǂ� */
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
	p = & ((*p)->left);  /* ���̎} */
      else
	p = & ((*p)->right);  /* �E�̎} */
    }
    else if (x1 < (*p)->key1)
      p = & ((*p)->left);  /* ���̎} */
    else
      p = & ((*p)->right);  /* �E�̎} */
  }
  fprintf(stdout, "DELETE SEARCH FAULT for x1=%ld x2=%ld\n", x1, x2);
}
#endif


/*--------------------------------------------------------------------------*
 * �{�̕���
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
  /* LZB�t�H�[�}�b�g��2�l�o�� */
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

/********************************************** �����O buffer ���� */
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
/* ���̃}�N���̈����͕�����]������܂��B���ɒ��� */
#define get_buf_e(z) ((int) ( (((int)(z))>=egg.buf_leng) ? (-1) : buf [ (egg.buf_leng == BUF_SIZE) ? ( (((int)(z)) + egg.buf_point) & (BUF_SIZE-1) ) : ((int)(z)) ] ) )
#endif

LOCAL int put_buf_e(int c){	/* egg.buf_leng ��Ԃ� */
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
  /* buffer �̎c��� c �Ŗ��ߐs���� */
  while (egg.buf_leng < BUF_SIZE)
    put_buf_e(c);
}
#endif /* deprecated */

#ifdef ORIGINAL1025
LOCAL int get_real_pos(int pos){
  /* buffer ��̈ʒu pos �̔z���̈ʒu��Ԃ� */
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
/* ���̃}�N���͈�����������]������܂��B���ɒ��� */
#define get_real_pos(z)  ( ((z)>=egg.buf_leng) ? (-1) : ( (egg.buf_leng==BUF_SIZE) ? (((z) + egg.buf_point) & (BUF_SIZE-1)) : (z) ) )
#endif

#ifdef ORIGINAL1025
LOCAL int get_ring_pos(int pos){
  /* �z���̈ʒu pos �� buffer ��̈ʒu��Ԃ� */
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
/* ���̃}�N���͈�����������]������܂��B���ɒ��� */
#define get_ring_pos(z)  ((egg.buf_leng==BUF_SIZE) ? ((BUF_SIZE+(z)-egg.buf_point) & (BUF_SIZE-1)) : (z))
#endif

#ifdef ORIGINAL1025
LOCAL int get_buf_e_leng(void){
  /* buffer �̗v�f����Ԃ� */
  return egg.buf_leng;
}
#else
#define get_buf_e_leng()  (egg.buf_leng)
#endif

/********************************************** buffer �̎d�؂� ���� */
LOCAL int move_part_e(int num){	/* ���g�̈ړ��� ��Ԃ� */
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
LOCAL int get_search_code_e(int pos){	/* pos(0~)�Ԗڂ̖��������L�� ��Ԃ� */
  return (get_buf_e(egg.part_pos + pos));
}
#else
#define get_search_code_e(z)    (get_buf_e(egg.part_pos + (z)))
#endif

#ifdef ORIGINAL1025
LOCAL int get_part_e(void){ /* �d�؂�̈ʒu��Ԃ� */
  return egg.part_pos;
}
#else
#define get_part_e()  (egg.part_pos)
#endif

/********************************************** buffer ���� */

static int buf_remain;		/* buffer ���̋L���� */

LOCAL void pre_set_buf(void){
  int chr;
  chr = get_c();
  if (chr != EOF)
    buf_remain = put_buf_e(chr);
  else
    put_buf_e(-2);
}

LOCAL int set_buf(void){ /* buffer �ɐV���ɋL������� ���͏I�����ǂ�����Ԃ�*/
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
  /* buffer �̏����� buffer �����ς��ɓ��͂��� */
  int i;
  for (i = 0; i < BUF_SIZE; i++){
    pre_set_buf();
  }
}

LOCAL void mod_buf(int num){
  /* buffer �̍X�V���� */
  int i, shift;
  shift = move_part_e(num);
  if (shift > 0)
    del_tree(shift);
  for (i = 0; i < shift; i++)
    set_buf();
}

LOCAL int do_cont(void){  /* ������ �I������ */
  if (get_buf_e(get_part_e()+ 3) != -2)
    return 1;
  else
    return 0;
}

/********************************************** �Œ���v�L����T�� */
LOCAL int search(int pos0, int pos1, int *lng){	/* ��v�L�����T�� */
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
    f = 1;			/* ��������buffer�𒴂��Ă���v */
  else if ((egg.now_match_leng + (*lng)) == MAX_MATCH_LENG)
    f = 2;			/* ����𒴂��Ă���v */
  return f;
}

LOCAL int set_tree(int pos, int *match_pos, int *match_leng){
  /* 4�������2���؂ɑ}�� */
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

LOCAL void encode(int ps, int pe){ /* �Œ���v�L����T�� */
  int f, ff, match_pos, leng = 0, lng = 0, dum = 0, dum1 = 0, p_pos;

  egg.now_match_leng = 0;
  f = search(ps, pe, &leng);
  match_pos = get_ring_pos(ps);
  switch (f) {
  case 0:			/* �Œ���v���o�b�t�@���Ŏ��܂� */
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

  case 1:			/* �Œ���v����������buffer�𒴂��� */
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

  case 2:			/* ��v��������𒴂��� */
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
  /* 2���؂���buffer �擪4�L���̗��shift �폜���� */
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
 * srcbuf����srclen�o�C�g�̃f�[�^���G���R�[�h����destbuf����n�܂�̈��
 * �i�[���A���̃o�C�g����destlen�Ɋi�[���܂��BkeyString �� keyLength ��
 * �Í����L�[�Ƃ��̒���(�o�C�g��)�ł��B�L�[�̒����ɐ����͂���܂��񂪁A
 * 5�`10�o�C�g���x��z�肵�Ă��܂��B
 * destbuf �����ɂ͕K�v�Ȃ����i�[�p�̗̈���m�ۂ��Ă����K�v������܂��B
 * destlen �͏��������ĂȂ���Ԃł��\���܂���B
 * JPEG�摜�Ȃǃf�[�^�ɂ���Ă͌������f�[�^�T�C�Y��������ꍇ������̂ŁA
 * destbuf�Ɋm�ۂ��Ă����̈�̃o�C�g���� srclen �̔{���x���������߂ł��B
 * (�m�ۂ��Ă���T�C�Y���z�������ǂ����̃`�F�b�N�Ȃǂ͂����������܂���)
 * ���ʓI�ɂ��Ƃ��(�f�[�^�݂̂�)�T�C�Y��������ꍇ�ɂ́A���k������
 * �w�b�_��O�ɂ��A�f�[�^���o�C�g�P�ʂ̃G���R�[�h�������ăR�s�[����
 * �Ԃ��܂��B���̏ꍇ��destlen��srclen���w�b�_�̃T�C�Y���������܂��B
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
    unsigned char* p = (unsigned char* )ID_STRING; /* ���ʗp������𖄂ߍ��� */
    for(; *p; *q++ = *p++){ ; }
    *q++ = ID_COMPRESSED; /* �u���k����Ă���v�t���O */
#endif
    /* �T�C�Y���̖��ߍ��� */
    *q++ = (unsigned char)((srclen >> 24) & 0xFF);
    *q++ = (unsigned char)((srclen >> 16) & 0xFF);
    *q++ = (unsigned char)((srclen >>  8) & 0xFF);
    *q++ = (unsigned char)((srclen      ) & 0xFF);
    /* ���Ԃ�ς����肵�Ă�OK(�f�R�[�h���Ɛ��������Ƃ邱��) */
  }
  Currentp = (unsigned char* )(destbuf + HEADER_SIZE);
  OutLen = HEADER_SIZE;
#else
  Currentp = (unsigned char* )destbuf;
  OutLen = 0;
#endif

  /* �ŏ� destlen �͓K���Ȓl(�̈�̃T�C�Y)���w�肵�Ă�����OK */
  /* �������AJPEG�摜�Ȃǂ��Ƃ��ƃ����_�����̍����f�[�^�̏ꍇ */
  /* �����T�C�Y�������Ă��܂����Ƃ������̂ŁA�ő�Q�{�� */
  /* �Ƃ��Ă������Ƃɂ��܂��B */

  initEncode();
  init_buf();

  set_tree(0, &match_pos, &match_leng);
  put_code(0, get_buf_e(0), 0, 0); /* �ŏ��̋L���͖������ɋL���̕����� */
  mod_buf(1);

  /* fprintf(stderr, "OK, entering loop...\n"); */
  while (do_cont()){
    match_leng = 0;		/* �Œ���v�L���� */
    egg.now_match_leng = 0;
    match_pos = 0;		/* �Œ���v�L���ʒu */

    s_c = get_search_code_e(0);	/* s_c �� �����L�� */

    set_tree(get_part_e(), &match_pos, &match_leng);	
    /* �d�؂�̋L���𕄍��� */

    if (match_leng < 4) {	/* �s��v������ */
      put_code(0, s_c, 0, get_part_e());
      mod_buf(1);
    }
    else{
      encode(match_pos, get_real_pos(get_part_e()));
    }
  }
  i = 0;			/* �L����v�L���������̎c�� */
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
  /* ���Ƃ��T�C�Y���������ꍇ�A���k(���͑����Ă�)�f�[�^��j�����A*/
  /* ���Ƃ̃f�[�^�ɃG���R�[�h���������܂��B */
  if(* destlen > srclen + HEADER_SIZE){
    unsigned char* p, * q;
    long i;
    fprintf(stderr, "Ouch. size increased => non-compression mode.\n");
    /* �u���k����Ă��Ȃ��v�t���O */
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
 * ���C���̉𓀂͂�������
 *--------------------------------------------------------------------------*
 ****************************************************************************/

#define DOUT fprintf
#define BITIN(a)  bit_in(Currenti, (a), & InLen)

typedef struct {
  int start_pos; /* put_buf_d()�Q��(0) */
  int end_pos;   /* put_buf_d()�Q��(0) */
  int part_pos;  /* move_part_d()�Q��(0) */
} deGSTATIC;

static deGSTATIC dgg;
#if 0 /* �Ē�`�G���[������� */
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
 * �L��f�[�^�����������܂��BDecodeData()�ɌĂ΂�܂��B
 *--------------------------------------------------------------------------*/
LOCAL void initDecode(void){
  dgg.start_pos = 0;
  dgg.end_pos = 0;
  dgg.part_pos = 0;
}


LOCAL int get_head_bit(void){
  /* �擪�Pbit �Ǎ��� */
  int c;
  Currenti = BITIN(& c);

  if(InLen >= InTotalBytes || c < 0){
    return -1;
  }
  return c;
}


LOCAL int get_match_pos(int part){
  /* ��v��ʒu�Ǎ��� �����͕��������̈ʒu�͈� */
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
  /* ��v�� �Ǎ��� */
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
  /* �L�� �Ǎ��� */
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

LOCAL void put_txt(int chr){  /* �����L�� �o�͑��� */
  if(OutTotalBytes <= 0 || OutLen < OutTotalBytes){
    OutLen++;
    * Currento++ = chr;
  }
}

/********************************************** �����O buffer ���� */

#if 0
static int buf[BUF_SIZE];
/* �G���R�[�h���ł��������O�Ŏg���Ă����B�T�C�Y�������Ȃ̂Ŏg���܂킵�������� */
#endif
#if 0
static int start_pos = 0, end_pos = 0;
#endif

LOCAL int put_buf_d(int chr){
  /* �o�b�t�@�̎c�萔��Ԃ� */
  int rem;
  buf[dgg.end_pos] = chr;
  dgg.end_pos = (dgg.end_pos + 1) % BUF_SIZE;
  rem = dgg.start_pos - dgg.end_pos;
  if (rem < 0)
    rem = BUF_SIZE + rem;
  return (rem);
}

LOCAL int get_buf_d(int num){	/* num�Ԗڂ̃f�[�^��Ԃ� */
  int chr = -1;
  int pos = (dgg.start_pos + num) % BUF_SIZE;
  int leng = (BUF_SIZE + dgg.end_pos - dgg.start_pos) % BUF_SIZE;
  if (leng == 0)
    leng = BUF_SIZE;
  if (num < leng)
    chr = buf[pos];
  return (chr);
}

LOCAL void shift_buf(int num){	/* �o�b�t�@�̓��e������num�V�t�g���� */
  dgg.start_pos = (dgg.start_pos + num) % BUF_SIZE;
}

/********************************************** buffer �̎d�؂� ���� */
#if 0
static int part_pos = 0;		/* �d�؂�̈ʒu */
#endif
LOCAL int move_part_d(int num){
    /* �d�؂���E��num�ړ����� �ړ��ł��Ȃ���������Ԃ�*/
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
{				/* pos(0~)�Ԗڂ̖�������������Ԃ� */
    return (get_buf_d(dgg.part_pos + pos));
}

LOCAL int get_part_d(void)
{				/* �d�؂�̈ʒu��Ԃ� */
    return (dgg.part_pos);
}

/********************************************** ������ */

LOCAL int decode(void){
  int head, pos, leng, chr, i, rem, lng, sft;

  head = get_head_bit();

  if (head == -1)
    return (-1);		/* �Ǎ��ݏI���Ȃ畜�����I�� */
  else if (head == 0) {
    chr = get_chr_code();	/* �L���̕��� */
    if (chr == -1)
      return (-1);
    rem = put_buf_d(chr);
    move_part_d(1);
  }
  else {
    pos = get_match_pos2(get_part_d());	/* ��v�L����̕��� */
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
 * p����n�܂�f�[�^�̈悪���̃��[�`����ʂ��Ă��邩�ǂ�����Ԃ��܂��B
 *--------------------------------------------------------------------------*/
int IsPassedCompression(char* p){
  return ( (memcmp(p, ID_STRING, ID_STRING_LEN) == 0) ? 1 : 0 );
}

/*--------------------------------------------------------------------------*
 * p����n�܂�f�[�^�̈悪���k����Ă��邩�ǂ�����Ԃ��܂��B
 *--------------------------------------------------------------------------*/
int IsCompressed(char* p){
  return (  (* (p+ID_STRING_LEN) == ID_COMPRESSED) ? 1 : 0 );
}

/*--------------------------------------------------------------------------*
 * �G���R�[�h���ꂽ�f�[�^��̐擪���w���|�C���^p���炻�̃f�R�[�h��̃f�[�^
 * �T�C�Y���Z�o���ĕԂ��܂��B
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
 * srcbuf����srclen�o�C�g�̃f�[�^���f�R�[�h����destbuf����n�܂�̈��
 * �i�[���A���̃o�C�g����destlen�Ɋi�[���܂��BkeyString �� keyLength ��
 * �Í����L�[�Ƃ��̒���(�o�C�g��)�ł��B�L�[�̒����ɐ����͂���܂��񂪁A
 * 5�`10�o�C�g���x��z�肵�Ă��܂��B
 * destbuf �����ɂ͕K�v�Ȃ����i�[�p�̗̈���m�ۂ��Ă����K�v������܂��B
 * destbuf��destlen��K�؂ɏ��������Ă���
 * ���@��2����A1�͐�� destlen = GetOriginalDataSize()�֐����g����
 * ���f�[�^�̃T�C�Y���擾���Ă����A���̑傫���҂�����̗̈��
 * destbuf = malloc(destlen) �Ŋm�ۂ��A���̂܂܂�destbuf��destlen��
 * �g�����Ƃł��B2�߂͓K���ɑ傫�ȃT�C�Y�̗̈�� destbuf �Ɋm�ۂ��Ă����A
 * destlen �ɂ� **���łȂ�** �K���Ȓl�����Ďg�����Ƃł��B��҂̏ꍇ��
 * ����I���Ȃ�f�R�[�h���ꂽ���f�[�^�̃T�C�Y�� destlen �ɕԂ��ė��܂����A
 * ���f�[�^�̃T�C�Y�����ٓI�ɑ傫�������ꍇ�ɂ́A�f�[�^������؂炸�Ɉُ�I��
 * ����댯������܂��B
 * (�m�ۂ��Ă���T�C�Y���z�������ǂ����̃`�F�b�N�Ȃǂ͂����������܂���)
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
    /* ���k����Ă��Ȃ� */
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
    destlen���s���ȏꍇ�Ɍ���A�����Ŏ擾���ĕԂ��Ă��܂��B
    OutLen�͑S���g���܂���B
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
