#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

const int N=200;
short orig_samples[N]={
    0,0,0,0,0,     0,0,0,0,0,   0,0,0,0,0,     0,0,0,0,0,
    3,6,9,12,15,   12,9,6,3,0,  -3,-6,-9,-12,-15, -12,-9,-6,-3,0,
    1,2,3,4,5,     4,3,2,1,0,  -1,-2,-3,-4,-5,  -4,-3,-2,-1,-0,
    
    0,0,0,0,0,     0,0,0,0,0,   0,0,0,0,0,     0,0,0,0,0,
    0,0,0,0,0,     0,0,0,0,0,   0,0,0,0,0,     0,0,0,0,0,
    0,0,0,0,0,     0,0,0,0,0,   0,0,0,0,0,     0,0,0,0,0,
    0,0,0,0,0,     0,0,0,0,0,   0,0,0,0,0,     0,0,0,0,0,
    0,0,0,0,0,     0,0,0,0,0,   0,0,0,0,0,     0,0,0,0,0,
};
// delay_n: 何サンプル遅らせるか
// intensity: エコーの音量
void add_echo(short *buf,int bufsize, int delay_n, float intensity) {
    for(int i=0;i<bufsize;i++) {
        int to_index=i+delay_n;
        if(to_index<bufsize) {
            buf[to_index]+=(short)(buf[i]*intensity);
        }
    }
}
void dump_samples(short *buf, int n) {
    for(int i=0;i<n;i++) printf("%d ", buf[i]);
    printf("\n");
}
void save(short *buf,int n, const char *path) {
    FILE *fp=fopen(path,"w");
    fwrite(buf,1,n*2,fp);
    fclose(fp);
}
int main() {
    short buf[N];
    for(int i=0;i<N;i++) buf[i]=orig_samples[i]*1000;
    // add noise
    for(int i=0;i<N;i++) {
        const int r=-1000 + (random()%1000);
        buf[i]+=r;
    }
    dump_samples(buf,N);
    save(buf,N,"no_echo.raw");    
    add_echo(buf,N,N/2,0.3);
    dump_samples(buf,N);
    save(buf,N,"with_echo.raw");
}
