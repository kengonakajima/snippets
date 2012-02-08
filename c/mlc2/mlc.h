#ifndef _MLC_H_
#define _MLC_H_

/***************
  mlc.h
****************/

#define     BASE        (10000)		/* LINT構造体のnum[i]の長さ。 変えたらあかん！*/
#define 	MAXLEN	 	(1000)		/* num[] の大きさ */

/* LINT 構造体 */
typedef struct {
	int error; /* 各関数がエラーを生じた場合 1 となる */
	int len;	/* num[] の長さ */
	int num[MAXLEN+1]; /* num[0] は各関数が計算に使用するので
					   実際の数値は num[1] から。　*/
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
