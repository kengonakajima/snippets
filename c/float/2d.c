#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 1024
#define RND (random()>>4)

float g_fld[N*N];

int ind(int x,int y) {
    return x + (y*N);
}
void clear() {
    for(int i=0;i<N;i++) g_fld[i]=0;    
}
float get(int x, int y) {
    return g_fld[ind(x,y)];
}
void add(int x, int y, float v) {
    g_fld[ind(x,y)]+=v;
}
// ffmpeg -f rawvideo -pixel_format rgba -video_size 1024x1024 -i out.raw output.png
// ffmpeg -v debug -r 30 -i hoge_%03d.png -vcodec libx264 -pix_fmt yuv420p -r 60 out.mp4     

double now() {
    struct timeval tmv;
    gettimeofday( &tmv, NULL );
    return tmv.tv_sec  + (double)(tmv.tv_usec) / 1000000.0f;
}

void dump(int stp) {
    static char b[N*N*4];
    for(int y=0;y<N;y++) {
        for(int x=0;x<N;x++) {
            float w=get(x,y);
            int c;
            if(w>1) c=255; else if(w>0.01)c=125; else if(w>0) c=50;
            int ind=(x+y*N)*4;
            b[ind]=c;
            b[ind+1]=c;
            b[ind+2]=c;
            b[ind+3]=0xff;            
        }
    }
    char fn[100];
    sprintf(fn,"out/hoge_%03d.rgba",stp);
    FILE *fp=fopen(fn,"wb");
    fwrite(b,1,sizeof(b),fp);
    fclose(fp);
}
void check(int cnt, int n, int moved, double dt) {
    float tot=0;
    for(int i=0;i<N*N;i++) {
        tot+=g_fld[i];
    }
    printf("[%d,%d] DT:%f MOVED:%d TOT:%f center:%f 2:%f 10:%f 100:%f 500:%f\n",
           cnt,n,
           dt,
           moved,tot,
           get(N/2,N/2),
           get(N/2-2,N/2-2),           
           get(N/2-10,N/2-10),
           get(N/2-100,N/2-100),
           get(N/2-500,N/2-500));
}

int step2(int n) {
    int rng=40;
    int div=rng*rng;
    int nn=n/div;
    
    int moved=0;
    for(int i=0;i<nn;i++) {
        int cx=RND % N;
        int cy=RND % N;
        
        for(int j=0;j<div;j++) {
            int dx=-rng+(RND % (rng*2));
            int dy=-rng+(RND % (rng*2));
            int x=cx+dx, y=cy+dy;
            if(x<=0 || x>=(N-1))continue;
            if(y<=0 || y>=(N-1))continue;        

            //            printf("%d %d %d %d %d %d\n",cx,cy,dx,dy,x,y);

            float w=get(x,y);
            float lw=get(x-1,y);
            float rw=get(x+1,y);
            float tw=get(x,y+1);
            float bw=get(x,y-1);

            if(w>0) {
                float avgw;
                switch(RND%4) {
                case 0: avgw=(w+lw)/2.0f; if(lw<avgw) { add(x-1,y,avgw-lw); add(x,y,lw-avgw); } moved++; break;
                case 1: avgw=(w+rw)/2.0f; if(rw<avgw) { add(x+1,y,avgw-rw); add(x,y,rw-avgw); } moved++; break;
                case 2: avgw=(w+tw)/2.0f; if(tw<avgw) { add(x,y+1,avgw-tw); add(x,y,tw-avgw); } moved++; break;
                case 3: avgw=(w+bw)/2.0f; if(bw<avgw) { add(x,y-1,avgw-bw); add(x,y,bw-avgw); } moved++; break;
                }
            }
        }
    }
    return moved;
    
}
int step(int n) {
    int moved=0;
    for(int i=0;i<n;i++) {
        int x=RND % N;
        int y=RND % N;
        if(x==0 || x==(N-1))continue;
        if(y==0 || y==(N-1))continue;        

        float w=get(x,y);
        float lw=get(x-1,y);
        float rw=get(x+1,y);
        float tw=get(x,y+1);
        float bw=get(x,y-1);

        if(w>0) {
            float avgw;
            switch(RND%4) {
            case 0: avgw=(w+lw)/2.0f; if(lw<avgw) { add(x-1,y,avgw-lw); add(x,y,lw-avgw); } moved++; break;
            case 1: avgw=(w+rw)/2.0f; if(rw<avgw) { add(x+1,y,avgw-rw); add(x,y,rw-avgw); } moved++; break;
            case 2: avgw=(w+tw)/2.0f; if(tw<avgw) { add(x,y+1,avgw-tw); add(x,y,tw-avgw); } moved++; break;
            case 3: avgw=(w+bw)/2.0f; if(bw<avgw) { add(x,y-1,avgw-bw); add(x,y,bw-avgw); } moved++; break;
            }
        }
    }
    return moved;
}

int step3(int n) {
    int cur_ind=0;
    int moved=0;
    for(int i=0;i<n;i++) {
        cur_ind += 2+(RND % 5);
        int x = cur_ind % N;
        int y = (cur_ind/N)%N;
        if(x<=0 || x>=(N-1))continue;
        if(y<=0 || y>=(N-1))continue;        

        //            printf("%d %d %d %d %d %d\n",cx,cy,dx,dy,x,y);

        float w=get(x,y);
        float lw=get(x-1,y);
        float rw=get(x+1,y);
        float tw=get(x,y+1);
        float bw=get(x,y-1);

        if(w>0) {
            float avgw;
            switch(RND%4) {
            case 0: avgw=(w+lw)/2.0f; if(lw<avgw) { add(x-1,y,avgw-lw); add(x,y,lw-avgw); } moved++; break;
            case 1: avgw=(w+rw)/2.0f; if(rw<avgw) { add(x+1,y,avgw-rw); add(x,y,rw-avgw); } moved++; break;
            case 2: avgw=(w+tw)/2.0f; if(tw<avgw) { add(x,y+1,avgw-tw); add(x,y,tw-avgw); } moved++; break;
            case 3: avgw=(w+bw)/2.0f; if(bw<avgw) { add(x,y-1,avgw-bw); add(x,y,bw-avgw); } moved++; break;
            }
        }
    }
    return moved;    
}

int main() {
    clear();
    add(N/2,N/2,1000);
    for(int cnt=0;cnt<500;cnt++) {
        usleep(1*1000);
        double st=now();
        const int n=10000000;
        int moved=step3(n);
        double et=now();
        check(cnt,n,moved,et-st);
        if(cnt%10==0) dump(cnt/10);
    }
}
