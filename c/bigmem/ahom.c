#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

/*
	大きいメモリを扱うテスト

	400MByte程度のメモリがあって、ほとんどの場所がアクセスされていない
	が、部分的にはかなり頻繁にアクセスされているような場合、スワップ
	の領域はどのようになり、メモリ使用量はどのようになるのかをしらべる。

	*-----*-----*-----*-----*-----*-----*-----*-----*-----*


	-*-----*-----*-----*-----*-----*-----*-----*-----*-----
	--*-----*-----*-----*-----*-----*-----*-----*-----*----
	---*-----*-----*-----*-----*-----*-----*-----*-----*---
	----*-----*-----*-----*-----*-----*-----*-----*-----*--
	-----*-----*-----*-----*-----*-----*-----*-----*-----*-

	というようにアクセスしてみる。
*/   

#define BSIZ	(50*1024*1024)

#define INTERVAL 4096*512		/* バイトごとに */
#define AMOUNT	 1			/* バイトの連続領域をアクセス */

double getUTimeDouble(void);

int main(int argc ,char **argv )
{
	int i,j ;
	int c;
	int start =0;
	char *mem;
	double t;
	int bsiz = BSIZ;
	
	mem = (char *)malloc( BSIZ );
	if( mem == NULL ) {
		fprintf( stderr,"fuck!");
		exit(0);
	}

	/* print estimated time */

	c = start;
	for(i=0;;i++){
		t = getUTimeDouble();
		for(j=0;j<AMOUNT;j++){
			mem[c+j] = j;
			/*int mmmm = mem[c+j];*/
			
		}

		fprintf( stderr,"Time:%f c:%x\n" , getUTimeDouble() - t , c );
		c += INTERVAL;
		if( c >= (BSIZ-AMOUNT)) break;
	}
	
}

double getUTimeDouble(void)
{

	struct timeval tm;

	gettimeofday( &tm , NULL );


    return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}

