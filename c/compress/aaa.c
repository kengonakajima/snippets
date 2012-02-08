#include <stdio.h>


#define CHAR_SIZE 256
#define NODE_SIZE 4096
#define BITS_LEN  12           /* 2^^12 = 4096 */

typedef struct {
    unsigned char chr;         /* 文字 */
    int      parent;           /* 親ノードへ */
    int      brother;          /* 兄弟ノードへ */
    int      child;            /* 子ノードへ */
} NODE;

long enLZW(unsigned char *code, long clen,
           unsigned char *text, long tlen)
{
    int  i, j;
    int  w, k, rv;
    long c, len;
    int  freeNode;
    NODE node[NODE_SIZE];
    static int wMask[BITS_LEN] =
        { 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1};
    static unsigned char bit0[8] = {~128,~64,~32,~16,~8,~4,~2,~1};
    static unsigned char bit1[8] = { 128, 64, 32, 16, 8, 4, 2, 1};

    
    if (tlen <= 0) return -1;

    for (i = 0; i <= CHAR_SIZE; i++) {
        node[i].chr     = (unsigned char)i;
        node[i].brother = i+1;
        node[i].parent  = node[i].child = 0;
    }
    node[CHAR_SIZE].brother = 0;
    freeNode = CHAR_SIZE + 1;
    
    /* 符号化と木の更新 */
    w = text[0];
    for (c = 1, i = 0, len = 0; ; c++) {
        k = (c >= tlen) ? CHAR_SIZE : text[c];

        /* wk が登録済 ? */
        for (rv = node[w].child; rv > 0; rv = node[rv].brother)
            if (node[rv].chr == k) break;
        if (rv > 0) w = rv;
        else {
            /* 参照番号を出力 */
            for (j = 0; j < BITS_LEN; j++) {
                if (w & wMask[j]) *code |= bit1[i];
                else              *code &= bit0[i];
                if (++i == 8) {
                    code++;
                    if (++len > clen) return -1L;
                    i = 0;
                }
            }
            /* wk を登録 */
            if (freeNode < NODE_SIZE) {
                node[freeNode].parent  = w;
                node[freeNode].chr     = k;
                node[freeNode].brother = node[w].child;
                node[freeNode].child   = 0;
                node[w].child = freeNode++;
            }
            w = k;
        }
        if (c == tlen+1) break;              /* EOF */
    }
    if (i > 0) len++;
    return (len > clen) ? -1L : len;
}

long deLZW(unsigned char *text, long tlen,
           unsigned char *code, long clen)
{
    int  i, j;
    int  w, k, rv;
    long c, len;
    int  freeNode;
    NODE node[NODE_SIZE];
    int  stack[NODE_SIZE]; int sp;
    static int wMask[BITS_LEN] =
        { 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1};
    static unsigned char bit1[8] = { 128, 64, 32, 16, 8, 4, 2, 1};

    
    if (clen <= 0) return -1;

    for (i = 0; i <= CHAR_SIZE; i++) {
        node[i].chr     = (unsigned char)i;
        node[i].brother = i+1;
        node[i].parent  = node[i].child = 0;
    }
    node[CHAR_SIZE].brother = 0;
    freeNode = CHAR_SIZE + 1;
    
    /* 復号化と木の更新 */
    for (i = 0, len = 0, c = 0, sp = 0; ; ) {
        for (j = 0, rv = 0; j < BITS_LEN; j++) {
            if (*code & bit1[i]) rv |= wMask[j];
            if (++i == 8) {
                code++;
                if (++c > clen) return -1L;
                i = 0;
            }
        }
        if (rv == CHAR_SIZE) break;              /* EOF */

        /* 参照番号に対応するノードからルートまで登りながら文字を出力 */
        if (rv >= freeNode) {
            stack[sp++] = k;                     /* 例外処理 */
            k = w;
        } else k = rv;

        while (k > CHAR_SIZE) {
            stack[sp++] = node[k].chr;
            k = node[k].parent;
        }
        stack[sp++] = k;

        while (sp) {
            if (++len > tlen) return -1L;
            *text++ = stack[--sp];
        }

        /* wk を登録 */
        if (len > 1 && freeNode < NODE_SIZE) {
            node[freeNode].parent  = w;
            node[freeNode].chr     = k;
            node[freeNode].brother = node[w].child;
            node[freeNode].child   = 0;
            node[w].child = freeNode++;
        }
        w = rv;
    }
    return len;
}


int main()
{
    unsigned char data[256];
    unsigned char hoge[1000];
    int len;

    int i;
#if 0
    for(i=0;i<200;i++){
        data[i]=1+rand()%10;
    }
#endif
strcpy( data, "にほんごのふつうのてきすとをいれるとそんなにみじかくならへんのはざんねんなことだ。にほんごのふつうのてきすとをいれるとそんなにみじかくならへんのはざんねんなことだ。" );
    
    len = enLZW( hoge , sizeof(hoge),data,strlen(data));

    printf( "len:%d <- %d\n" , len ,strlen(data));

    

}
