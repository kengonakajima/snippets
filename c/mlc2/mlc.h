#ifndef _MLC_H_
#define _MLC_H_

/***************
  mlc.h
****************/

#define     BASE        (10000)		/* LINT��¤�Τ�num[i]��Ĺ���� �Ѥ����餢����*/
#define 	MAXLEN	 	(1000)		/* num[] ���礭�� */

/* LINT ��¤�� */
typedef struct {
	int error; /* �ƴؿ������顼����������� 1 �Ȥʤ� */
	int len;	/* num[] ��Ĺ�� */
	int num[MAXLEN+1]; /* num[0] �ϳƴؿ����׻��˻��Ѥ���Τ�
					   �ºݤο��ͤ� num[1] ���顣��*/
    int sign;
} LINT;

int ml_str2LINT( char *str, int len, LINT *out, int base );
int ml_LINT2Str( char *str, LINT num );
int ml_int2LINT( int n, LINT* out );
int mlsmod( LINT a, int n );
int ml_powm(LINT a, LINT b, LINT n, LINT* out);

int ml_cmp( LINT a, LINT b );
int ml_add( LINT a, LINT b, LINT *out );
int ml_sub( LINT a, LINT b, LINT *out );
int ml_mul( LINT a, LINT b, LINT *out );

#endif //_MLC_H_
