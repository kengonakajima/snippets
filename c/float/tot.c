#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define N 1024
#define RND (random()>>4)

float fld[N];


void step(int n, int cnt) {
    int moved=0;
    for(int i=0;i<n;i++) {
        int x=RND % N;
        if(x==0 || x==(N-1))continue;

        float w=fld[x], lw=fld[x-1],rw=fld[x+1];

        if(w>0) {
            float to_left=0, to_right=0;
            if(RND%2==0) { // left and center
                float avgw=(w+lw)/2.0f;
                if(lw<avgw) to_left=avgw-lw;
            } else {
                float avgw=(w+rw)/2.0f;
                if(rw<avgw) to_right=avgw-rw;
            }
            if(to_left>0) { fld[x-1]+=to_left; fld[x]-=to_left; moved++; }
            if(to_right>0) { fld[x+1]+=to_right; fld[x]-=to_right; moved++; }
        }
    }
    float tot=0;
    for(int i=0;i<N;i++) {
        tot+=fld[i];
    }
    int top0=-1, last0=-1;
    for(int i=0;i<N;i++) {
        if(fld[i]>0){top0=i; break;}
    }
    for(int i=N-1;i>=0;i--) {
        if(fld[i]>0) {last0=i; break;}
    }
    printf("[%d,%d] MOVED:%d TOP:%d LAST:%d TOT:%f   %f %.4f %.3f %.2f | %.2f | %.2f %.3f %.4f %f\n",
           cnt,n,
           moved,
           top0, last0,
           tot,
           fld[0],fld[N/2-200],fld[N/2-50],fld[N/2-10],
           fld[N/2],
           fld[N/2+10],fld[N/2+50],fld[N/2+200], fld[N-1]);
    
}

int main() {
    for(int i=0;i<N;i++) fld[i]=0;
    fld[N/2]=100;
    int cnt=0;
    for(;;) {
        usleep(100*1000);
        step(500000,cnt);
        cnt++;
    }
}
