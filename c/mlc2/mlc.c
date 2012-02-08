/*********************************************************

  mlc.c -- 多倍長演算ライブラリ
  by Thai Akasaka 2000/7/12

　すべて正の数についてのみ計算可能！！
　引数や結果が負の場合はエラーを返す。

　正の多倍長整数について以下の演算が可能：　
　●比較(a<=>b): 
	int ml_cmp( LINT a, LINT b );
　●足し算(a+b): 
	int ml_add( LINT a, LINT b, LINT* result );
　●引き算(a-b): 
	int ml_sub( LINT a, LINT b, LINT* result );
　●かけ算(a*b): 
	int ml_mul( LINT a, LINT b, LINT* result );
　●累乗剰余(a^b mod n)： 
	int ml_powm( LINT a, LINT b, LINT n, LINT* result );
  いずれも成功したら 1 , 失敗したら -1 を返す。
　
　補助関数：
　●LINT 型を string に変換: 
	int ml_LINT2Str( char* str, LINT num );
　●string をLINT型に変換
	int ml_str2LINT( char* str, LINT* result, int base );
  いずれも成功したら 1 , 失敗したら -1 を返す。


**********************************************************/

#include "mlc.h"


/* プロトタイプ宣言 */
int ml_copy_LINT( LINT* from, LINT* to );
LINT mlmod1(LINT a, LINT *n, LINT *nn, int d, int nntop, int nnlen);





/*************************************************

  ml_LINT2Str -- LINT 型を string に変換

  引数
	char* str:	LINT型が文字列として格納されるアドレス。
				長さは MAXLEN*4 以上確保しなければならない。
	LINT num:	変換すべき数値
  返り値
	成功の場合	1 を返す
	errorの場合 -1 を返す
  
*************************************************/
int ml_LINT2Str( char *str, LINT num )
{
    int  i, j;
    char *ss;
    int  x, cx;

    if( num.len == 0 || num.len > MAXLEN ){
        *str++ = '0';
        *str = '\0';
        return -1;
    }

	/* num == 0 の場合の特別処理 (下でばぐるため) */
	if( num.len == 1 && num.num[1] == 0 ){
		*str++ = '0';
		*str = '\0';
		return 1;
	}

    ss = str - 1;
    for ( i = num.len, cx = 0; i > 0; i-- ) {
        x = num.num[++cx];
        for( j = 1; j < BASE; j*=10 ) {
            *++ss = x % 10 + '0';
            x /= 10;
        }
    }
    while (*ss == '0') ss--; // num == 0 の場合、ばぐる。


    *(ss + 1) = '\0';
    while (str < ss) {
        x = *str;
        *str++ = *ss;
        *ss-- = x;
    }
	return 1;
    
}




/*************************************************

  ml_str2LINT -- string をLINT型に変換

  引数
  	char *str: 数字の文字列
  	int len: strの長さ
	LINT *nn: 結果が格納される
	int base: 10進か16進かどちらか。
　返り値(int)
	成功の場合	1 を返す
	errorの場合 -1 を返す

*************************************************/
int ml_str2LINT( char* str, int len, LINT* nn, int base )
{
    char *ss, *start;
    int  k, x, cnn;
	LINT t16, t16p, t1, t2, tret;

	if( base != 10 && base != 16 ) return -1;

	start = str;
    while ( *str == ' ' || *str == '\t' || *str == '0' ) {
		str++;
		if( str - start > len ) return -1;
	}
    ss = str;

	if( base == 10 ){

		while( *ss >= '0' && *ss <= '9' ) {
			ss++;	
			if( ss - start > len ) return -1;
		}
		
		if ( ss == str ) {
			nn->len = 0;
			nn->num[1] = 0;
			return 1;
		}
		
		x = 0;
		k = 1;
		cnn = 0;
		do {
			x += ( *--ss - '0' ) * k;
			k *= 10;
			if ( k == BASE || ss == str ){
				nn->num[++cnn] = x;
				x = 0;
				k = 1;
				if( cnn >= MAXLEN ) return -1;
			}
		} while ( ss != str );
		
		nn->len = cnn;
		return 1;
	
	} else if( base == 16 ){
		
		while( 1 ){
			if( ( *ss >= '0' && *ss <= '9' ) || *ss >= 'a' && *ss <= 'f' ) {
				ss++;	
				if( ss - start > len ) return -1;
			} else if( *ss == '\0' || *ss == '\n' ) { 
				break;
			} else return -1;
		}
		
		if ( ss == str ) {
			nn->len = 0;
			nn->num[1] = 0;
			return 1;
		}
		
		ss--;
		if( *ss >= '0' && *ss <= '9' ) {
			if( ml_int2LINT( *ss - '0', &tret )  < 0 ) return -1;
		} else if( *ss >= 'a' && *ss <= 'f' ) {
			if( ml_int2LINT( *ss - 'a' + 10, &tret )  < 0 ) return -1;
		}
		if( ml_int2LINT( 16, &t16 )  < 0 ) return -1;
		if( ml_int2LINT( 16, &t16p )  < 0 ) return -1;
		
		for( ; ss!= str ;){
			ss-- ;
			if( *ss >= '0' && *ss <= '9' ){
				if( ml_int2LINT( *ss - '0', &t1 )  < 0 ) return -1;
			} else if( *ss >= 'a' && *ss <= 'f' ){
				if( ml_int2LINT( *ss - 'a' + 10, &t1 ) < 0 ) return -1;
			} else return -1;
			if( ml_mul( t1, t16p, &t2 ) < 0 ) return -1;
			if( ml_add( tret, t2, &tret ) < 0 ) return -1;
			if( ml_mul( t16, t16p, &t16p ) < 0 ) return -1;
		}
		
		ml_copy_LINT( &tret, nn );
		return 1;
		
	}


	return -1;

}




/****************************************
　ml_int2LINT -- int型をLINT型に変換

  引数
	int n: 変換すべき int
	LINT *out: 結果が入る
　返り値(int)
	成功の場合	1 を返す
	errorの場合 -1 を返す

****************************************/
int ml_int2LINT( int n, LINT *out )
{
    int i;

    out->len = 0;
	if( n < 0 ) return -1;
	if( n == 0 ) {
		out->len = 1;
        out->num[1] = 0;
        return 1;
    }

	for( i = 0; n > 0 ; ) {
        out->num[++i] = n % BASE;
        n /= BASE;
		if( i >= MAXLEN ) return -1;
    }
    out->len = i;
    return 1;
    
}



/**************************************

  ml_cmp -- ２数の大小比較 ( a <=> b )

　引数
	LINT a, LINT b: 比較すべき数
　返り値(int)
	a > b の場合	1 を返す 
	a < b の場合	-1 を返す
	a = b の場合	0 を返す
	errorの場合		-2 を返す

**************************************/
int ml_cmp( LINT a, LINT b )
{

	int la, lb, lr;
	int l;

	la = a.len;
	lb = b.len;
	if( la > MAXLEN || lb > MAXLEN ) return -2;
	if( la > lb ) return 1;
	else if( la < lb ) return -1;

	lr = la; // lr=la=lb
	for( l = lr; l>0; l-- ){
		if( a.num[l] > b.num[l] ) return 1;
		else if( a.num[l] < b.num[l] ) return -1;
	}
	return 0;


}


/**************************************

  ml_add -- たし算 ( a + b )

　引数
	LINT a, LINT b: 足すべき数
	LINT *out: 結果が入る
　返り値(int)
	成功の場合	1 を返す
	errorの場合 -1 を返す

**************************************/
int ml_add( LINT a, LINT b, LINT *ret )
{
    int i;
    int lr, la, lb;
    int cr, ca, cb;
    int x;

	if( a.len > MAXLEN || b.len > MAXLEN ) return -1;
    la = a.len;
    lb = b.len;
    lr = ( la >= lb ) ? la : lb;

    x = 0; cr = ca = cb = 0;
    for ( i = 1; i <= lr ; i++ ) {
        if ( i <= la ) x += a.num[++ca];
        if ( i <= lb ) x += b.num[++cb];
        if ( x < BASE ) {
            ret->num[++cr] = x;
            x = 0;
        } else {
            ret->num[++cr] = x - BASE;
            x = 1;
        }
		if( cr >= MAXLEN ) return -1;
    }
    ret->num[++cr] = x;
    ret->len = lr + x;
    return 1;

}




/**************************************

　ml_sub -- 引き算 ( a - b )

　引数
　	LINT a, LINT b: a-b となる数
	LINT *out: 結果が入る
　返り値 (int)
	成功の場合		1 を返す
	結果が負の場合	-1 を返す
	errorの場合		-1 を返す
	
**************************************/                                      
int ml_sub( LINT a, LINT b, LINT *ret )
{
    int i;
    int la, lb;
    int cla, clb, clr;
    int  x;
		
	if( a.len > MAXLEN || b.len > MAXLEN ) return -1;
	if( ml_cmp( a, b ) < 0 ) return -1;

    la = a.len;
    lb = b.len;

    x = cla = clb = clr = 0;
    for ( i = 1; i <= la; i++ ) {
        x += a.num[++cla];
        if ( i <= lb ) x -= b.num[++clb];
        if ( x >= 0 ) {
            ret->num[++clr] = x;
            x = 0;
        } else {
            ret->num[++clr] = x + BASE;
            x = -1;
        }
		if( clr >= MAXLEN ) return -1;
    }
    while (--i > 0)
        if ( ret->num[clr--] != 0) break;
    ret->len = i;

    return 1;
    
}




/**************************************

　ml_mul -- かけ算 ( a * b )

  引数
	LINT a, LINT b: a * b となる数
	LINT *ret: 結果が入る
　返り値 (int)
	成功の場合	1 を返す
	errorの場合 -1 を返す

**************************************/
int ml_mul( LINT a, LINT b, LINT *ret )    
{
    int i, j;
    int la, lb;
    int cla, clb, clr;
    int ca = 1;
    long x;
    
    la = a.len;
    lb = b.len;

	if( la+lb > MAXLEN ) return -1;
    for ( i = la + lb; i > 0; i-- ) {
		ret->num[i] = 0;
	}

    cla = clb = clr = 0;

    for ( j = 1; j <= lb; j++ ) {
        ca = 0;
        for ( i = 1, cla = 0; i <= la; i++ ) {
            x = a.num[++cla];
            x = x * b.num[j] + ret->num[i+j-1] + ca;
            ret->num[i+j-1] = x % BASE;
            ca = x / BASE;
        }
        ret->num[i+j-1] = ca;
    }

    ret->len = ( ca != 0 ) ? la + lb : la + lb - 1;

    return 1;
    
}





/*************************************************

  ml_copy_LINT -- LINT を　LINT にコピー

*************************************************/
int ml_copy_LINT( LINT* from, LINT* to )
{
	int i;
	if( from->len > MAXLEN ) return -1;
	for( i = 0; i < from->len; i++ ){
		to->num[i+1] = from->num[i+1];
	}
	to->len = from->len;
	return 1;

}


/*************************************************

  ml_powm -- 累乗剰余計算 ( a^b mod n )

  引数
  　LINT a, LINT b, LINT n:  a^b mod n となる数
	LINT *ret: 結果が入る
　返り値 (int)
	成功の場合	1 を返す
	errorの場合 -1 を返す

**************************************************/
int ml_powm(LINT a, LINT b, LINT n, LINT *ret)
{
	LINT nn, kk;
	LINT x;
	int *bnum1, *btop, *p;
	int d, nntop, nnlen, i, t;

	if( n.len == 0 || n.len > MAXLEN || 
		a.len == 0 || a.len > MAXLEN || 
		b.len == 0 || b.len > MAXLEN ) {
		return -1;
	}

	if( a.len == 0 ) {
		if( ml_copy_LINT( &a, ret ) < 0 ) return -1;
		return 1;
	}
	if( b.len == 0 ) {
		if( ml_int2LINT( 1, ret ) < 0 ) return -1;
		return 1;
	}

	x.len = 1;
	x.num[1] = 1;
	nn = n;
	nnlen = nn.len;
	d = BASE / (nn.num[nnlen] + 1);
	if(d != 1){
		for(i = 1, t = 0; i <= nnlen; i++){
			nn.num[i] = (t += nn.num[i] * d) % BASE;
			t /= BASE;
		}
	}
	nntop = nn.num[nnlen];
	btop = b.num + b.len;
	bnum1 = b.num + 1;
	do{
		if(*bnum1 % 2 == 0)	{
			p = btop;
			t = 0;
			i = btop - b.num;
			while(i--){
				*p-- = (t += *p) / 2;
				t = (t % 2)? BASE: 0;
			}
			while(*btop == 0)	btop--;
			if( ml_mul( a, a, &kk ) < 0 ) return -1;
			a = mlmod1( kk, &n, &nn, d, nntop, nnlen);
			if( a.len == -1 ) return -1;
			if( a.len == 0 ){
				x.len = 0;
				break;
			}
		} else {
			(*bnum1)--;
			if( ml_mul( x, a, &kk ) < 0 ) return -1;
			x = mlmod1( kk, &n, &nn, d, nntop, nnlen);
			if( x.len == -1 ) return -1;
			if( x.len == 0 ) break;
		}
	} while( btop > bnum1 || *bnum1 );
			
	if( ml_copy_LINT( &x, ret ) < 0 ) return -1;

	if( ret->len < 1 ) return -1; //今のところばぐったとき、len=0となっている。
	return 1;
}



/*************************************
 
  mlsmod -- LINT mod int の剰余計算 ( a mod n )

  引数
	LINT a, int n: a mod n となる数
　返り値(int)
	結果
  
*************************************/
int mlsmod( LINT a, int n )
{
    int i, r;

    int basemod[MAXLEN];

    basemod[0] = BASE % n;
    for( i = 1; i < a.len; i++ ){
        basemod[i] = ( basemod[i-1] * basemod[0] ) % n;
    }

    r = ( a.num[1] % n );
    for( i = 1; i < a.len; i++ ){
        long t1;
		t1 = ( ( ( a.num[i+1] % n ) * basemod[i-1] ) % n );
		r = ( ( r + t1 ) % n );
    }
    return r;
}



/*************************************

  mlmod1 -- LINT mod LINT の剰余計算 ( a mod n )

  引数
	

  返り値
	LINT型　結果
	失敗の場合は、LINTのメンバに len = -1 を代入する。

**************************************/                                      
LINT mlmod1(LINT a, LINT *n, LINT *nn, int d, int nntop, int nnlen)
{
	LINT w;
	int *anum1, *nnnum1, *wnum1, *p, *q;
	int alen, i, ql, t, dx;

	w.error = 0;

	if( n->len == 1 ) {
		LINT ret;
		if( ml_int2LINT( mlsmod( a, n->num[1] ), &ret ) < 0 ) {
			ret.len = -1;
			return ret;
		}
		return ret;
//		return ml_int2LINT( mlsmod( a, n->num[1] ) );
	}
	if( n->len > ( alen = a.len ) ) return a;
	else if( alen == n->len ) {
		a.num[0] = 0;
		n->num[0] = 1;
		p = a.num + alen;
		q = n->num + alen;
		while( *p-- == *q-- );
		if( p == a.num ) {
			a.len = 0;
			return a;
		}
		if( *(++p) < *(++q) ) return a;
	}

	if( alen == MAXLEN ){
		a.error = 1;
//		fprintf(stderr, "Error : Too large in lmod1()\n");
		a.len = 0;
		return a;
	}
	anum1 = a.num + 1;
	if( d != 1 ){
		t = 0;
		p = anum1;
		i = alen;
		while( i-- ){
			*p = (t += *p * d) % BASE;
			t /= BASE;
			p++;
		}
		while( t ){
			*p++ = t % BASE;
			t /= BASE;
		}
		a.len = alen = --p - a.num;
	}
	ql = alen - nnlen + 1;
	if( ( dx = a.num[alen] / nntop ) == 0 )	{
		ql--;
		dx = ( a.num[alen] * BASE + a.num[alen - 1] ) / nntop;
	}
	a.num[0] = 1;
	w.len = 0;
	w.num[0] = 0;
	nnnum1 = nn->num + 1;
	wnum1 = w.num + 1;
	for(;;){
		ql--;
		for(;;)	{
			q = wnum1;
			i = ql;
			while( i-- )	*q++ = 0;
			t = 0;
			p = nnnum1;
			i = nnlen;
			while( i-- ){
				*q++ = (t += *p++ * dx) % BASE;
				t /= BASE;
			}
			while( t ){
				*q++ = t % BASE;
				t /= BASE;
			}
			if( alen > ( w.len = q - wnum1 ) )	break;
			else if( alen == w.len ){
				p = a.num + alen;
				q = w.num + alen;
				while( *q-- == *p-- );
				if( *(++q) < *(++p) ) break;
			}
			dx--;
		}
		for( i = w.len + 1, q = w.num + i; i <= alen; i++ )	*q++ = 0;
		p = anum1;
		q = wnum1;
		i = alen;
		while( i-- ){
			if(( *p -= *q++ ) < 0){
				*p += BASE;
				(*q)++;
			}
			p++;
		}
		while( *(--p) == 0 );
		if( ( alen = p - a.num ) == 0 || ql == 0 ) break;
		dx = ( a.num[alen] * BASE + a.num[ alen-1 ]) / nntop; 
	}
	if( d != 1 ){
		p = a.num + ( i = alen );
		t = 0;
		while( i-- ){
			*p = ( t += *p ) / d;
			t = (t % d) * BASE;
			p--;
		}
		while( a.num[alen] == 0 ) alen--;
	}
	a.len = alen;
	return a;
}

