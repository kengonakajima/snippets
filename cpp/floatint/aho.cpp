#include <stdio.h>

#define N 10000

int main() {
    float x=0,y=0;
    float dx=1.2, dy=1.88;

    float fa[N*2];
    int ia[N*2];
    int ia2[N*2]; // only x moves
    short sa[N*2];
    
    // float
    for(int i=0;i<N;i++){
        fa[i*2+0] = x;
        fa[i*2+1] = y;
        ia[i*2+0] = (int)x;
        ia[i*2+1] = (int)y;
        ia2[i*2+0] = (int)x;
        ia2[i*2+1] = i/100;        
        sa[i*2+0] = (short)x;
        sa[i*2+1] = (short)y;
        //        printf("hoge: %f %f %d %d %d %d\n", fa[i*2+0], fa[i*2+1], ia[i*2+0], ia[i*2+1], sa[i*2+0], sa[i*2+1] );
        x+=dx;
        y+=dy;
    }
    FILE *fp = fopen( "fa", "wb");
    fwrite(fa, 4*2*N,1,fp);
    fclose(fp);
    fp = fopen( "ia","wb");
    fwrite(ia, 4*2*N,1,fp);
    fclose(fp);
    fp = fopen( "ia2","wb");
    fwrite(ia2, 4*2*N,1,fp);
    fclose(fp);    
    fp = fopen( "sa","wb");
    fwrite(sa,2*2*N,1,fp);
    fclose(fp);
    
}
