/* zlib を使う例 */
/* コンパイル: gcc -Wall comptest.c -o comptest -lz */
/* -lz は libz.a をリンクする（たぶん /usr/local/lib にある）*/

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>               /* /usr/local/include/zlib.h */

#define INBUFSIZ   1024         /* 入力バッファサイズ（任意） */
#define OUTBUFSIZ  1024         /* 出力バッファサイズ（任意） */

z_stream z;                     /* ライブラリとやりとりするための構造体 */

char inbuf[INBUFSIZ];           /* 入力バッファ */
char outbuf[OUTBUFSIZ];         /* 出力バッファ */
FILE *fin, *fout;               /* 入力・出力ファイル */

void do_compress(void)          /* 圧縮 */
{
    int count, flush, status;

    /* すべてのメモリ管理をライブラリに任せる */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* 初期化 */
    /* 第2引数は圧縮の度合。0〜9 の範囲の整数で，0 は無圧縮 */
    /* Z_DEFAULT_COMPRESSION (= 6) が標準 */
        if (deflateInit(&z, Z_DEFAULT_COMPRESSION ) != Z_OK) {
        fprintf(stderr, "deflateInit: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }

    z.avail_in = 0;             /* 入力バッファ中のデータのバイト数 */
    z.next_out = outbuf;        /* 出力ポインタ */
    z.avail_out = OUTBUFSIZ;    /* 出力バッファのサイズ */

    /* 通常は deflate() の第2引数は Z_NO_FLUSH にして呼び出す */
    flush = Z_NO_FLUSH;

    while (1) {
        if (z.avail_in == 0) {  /* 入力が尽きれば */
            z.next_in = inbuf;  /* 入力ポインタを入力バッファの先頭に */
            z.avail_in = fread(inbuf, 1, INBUFSIZ, fin); /* データを読み込む */

            /* 入力が最後になったら deflate() の第2引数は Z_FINISH にする */
            if (z.avail_in < INBUFSIZ) flush = Z_FINISH;
        }
        status = deflate(&z, flush); /* 圧縮する */
        if (status == Z_STREAM_END) break; /* 完了 */
        if (status != Z_OK) {   /* エラー */
            fprintf(stderr, "deflate: %s\n", (z.msg) ? z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* 出力バッファが尽きれば */
            /* まとめて書き出す */
            if (fwrite(outbuf, 1, OUTBUFSIZ, fout) != OUTBUFSIZ) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* 出力バッファ残量を元に戻す */
            z.avail_out = OUTBUFSIZ; /* 出力ポインタを元に戻す */
        }
    }

    /* 残りを吐き出す */
    if ((count = OUTBUFSIZ - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, fout) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* 後始末 */
    if (deflateEnd(&z) != Z_OK) {
        fprintf(stderr, "deflateEnd: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }
}

void do_decompress(void)        /* 展開（復元） */
{
    int count, status;

    /* すべてのメモリ管理をライブラリに任せる */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* 初期化 */
    z.next_in = Z_NULL;
    z.avail_in = 0;
    if (inflateInit(&z) != Z_OK) {
        fprintf(stderr, "inflateInit: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }

    z.next_out = outbuf;        /* 出力ポインタ */
    z.avail_out = OUTBUFSIZ;    /* 出力バッファ残量 */
    status = Z_OK;

    while (status != Z_STREAM_END) {
        if (z.avail_in == 0) {  /* 入力残量がゼロになれば */
            z.next_in = inbuf;  /* 入力ポインタを元に戻す */
            z.avail_in = fread(inbuf, 1, INBUFSIZ, fin); /* データを読む */
        }
        status = inflate(&z, Z_NO_FLUSH); /* 展開 */
        if (status == Z_STREAM_END) break; /* 完了 */
        if (status != Z_OK) {   /* エラー */
            fprintf(stderr, "inflate: %s\n", (z.msg) ? z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* 出力バッファが尽きれば */
            /* まとめて書き出す */
            if (fwrite(outbuf, 1, OUTBUFSIZ, fout) != OUTBUFSIZ) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* 出力ポインタを元に戻す */
            z.avail_out = OUTBUFSIZ; /* 出力バッファ残量を元に戻す */
        }
    }

    /* 残りを吐き出す */
    if ((count = OUTBUFSIZ - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, fout) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* 後始末 */
    if (inflateEnd(&z) != Z_OK) {
        fprintf(stderr, "inflateEnd: %s\n", (z.msg) ? z.msg : "???");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int c;

    if (argc != 4) {
        fprintf(stderr, "Usage: comptest flag infile outfile\n");
        fprintf(stderr, "  flag: c=compress d=decompress\n");
        exit(0);
    }
    if (argv[1][0] == 'c') {
        c = 1;
    } else if (argv[1][0] == 'd') {
        c = 0;
    } else {
        fprintf(stderr, "Unknown flag: %s\n", argv[1]);
        exit(1);
    }
    if ((fin = fopen(argv[2], "r")) == NULL) {
        fprintf(stderr, "Can't open %s\n", argv[2]);
        exit(1);
    }
    if ((fout = fopen(argv[3], "w")) == NULL) {
        fprintf(stderr, "Can't open %s\n", argv[3]);
        exit(1);
    }
    if (c) do_compress(); else do_decompress();
    fclose(fin);
    fclose(fout);
    return 0;
}

