
/*
  2048x2048リニアに取るのか

  32x32 * 64x64 とするのがいいのか
  どっちがはやいか

 */

#include <stdio.h>
#include <stdlib.h>

struct grid
{
    int a0,a1,a2,a3;
    int aa0,aa1,aa2,aa3;
    int aaa0, aaa1;
}; /* 40 bytes */

struct subpage
{
    struct grid g[32*32];
};

#define N 10

int main()
{
    int k;
    
#if 0    
    struct grid *g = malloc( sizeof( struct grid ) * 2048 * 2048 );

    int i,j;

    for(k=0;k<N;k++){
        for(j=0;j<2048;j++){
            for(i=0;i<2048;i++){
                memset( &g[ i + j * 2048], 0, sizeof( struct grid ));
            }
        }
    }
#endif

#if 1
    struct subpage *s = malloc( sizeof(struct subpage) * 64* 64 );
    int i,j;
    for(k=0;k<N;k++){
        for(j=0;j<64;j++){
            for(i=0;i<64;i++){
                int x,y;
                for(y=0;y<32;y++){
                    for(x=0;x<32;x++){
                        memset( & s[i + j * 64].g[x + y * 32] ,
                                0, sizeof(struct grid));
                    }
                }
            }
        }
    }
#endif
    
    return 0;
}

