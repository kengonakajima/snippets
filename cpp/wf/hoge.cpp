#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int W=32;
const int H=32;
char fld[W*H];
int ind(int x,int y) { return y*W+x;}
char *get(int x, int y) { return &fld[ind(x,y)]; }
void set(int x, int y,char v) { fld[ind(x,y)]=v; }

const char AIR=0;
const char WALL=-1;
const char WATER_MAX=9;

void fill(int x0, int y0, int x1, int y1, char v ) {
    for(int y=y0;y<y1;y++) for(int x=x0;x<x1;x++) set(x,y,v);
}
inline float maxf( float a, float b ){
    return (a>b) ? a:b;
}
inline float maxf( float a, float b, float c ) {
    return maxf( maxf(a,b), c );
}
inline float maxf( float a, float b, float c, float d ) {
    return maxf( maxf(a,b), maxf(c,d) );
}
inline double maxd( double a, double b ){
    return  (a>b) ? a:b;
}
inline double mind( double a, double b ){
    return (a<b) ? a:b;
}
inline float minf(float a, float b) {
    return (a<b) ? a:b;
}
inline int maxi( int a, int b ){
    return  (a>b) ? a:b;
}
inline int mini( int a, int b ){
    return (a<b) ? a:b;
}


inline double range( double a, double b ) {
    long r = random();
    double rate = (double)r / (double)(0x7fffffff);
    double _a = mind(a,b);
    double _b = maxd(a,b);
    return _a + (_b-_a)*rate;
}
inline int irange( int a, int b ) {
    double r = range(a,b);
    return (int)r;
}

void sim(int x, int y) {
    if(x<1||y<1||x>=W-1||y>=H-1)return;
    char *c=get(x,y);
    if(*c==0)return;
    // down
    char *dc=get(x,y-1);
    if(*c==1) {
        if(*dc==0) {
            (*dc)++;
            (*c)=0;
        } else {
            /*            
            char *uc=get(x,y+1);
            if( (*uc)==AIR && (*dc)>0&&(*dc)<WATER_MAX) {
                (*c)--;
                (*dc)++;
            }
            */            
        }
        return;
    } else if(*c>1){
        if( (*dc)!=WALL && (*dc)<WATER_MAX) {
            (*dc)++;
            (*c)--;
        }
    }
    // compress
    char *uc=get(x,y+1);
    char *uuc=get(x,y+2);
    if(*c>0 && *c<WATER_MAX && *uc==1 && *uuc==AIR) {
        (*c)++;
        (*uc)=0;
    }
    // horiz
    if(range(0,1)<0.5) {
        // R
        char *rc=get(x+1,y);
        if(*rc==WALL)return;
        if(*rc==*c)return;
        if(*rc<*c-1) {
            (*rc)++;
            (*c)--;
        }
    } else {
        // L
        char *lc=get(x-1,y);
        if(*lc==WALL)return;
        if(*lc==*c)return;
        if(*lc<*c-1) {
            (*lc)++;
            (*c)--;
        }
        
    }
}
void display() {
    for(int y=H-1;y>=0;y--){
        for(int x=0;x<W;x++) {
            char ch='?';
            char *cell=get(x,y);
            if(*cell==AIR)ch=' ';
            if(*cell==WALL)ch='#';
            if(*cell>0 && *cell<=WATER_MAX) ch='0' + *cell;
            fprintf(stderr,"%c",ch);
        }
        fprintf(stderr,"\n");
    }
}
int main() {

/*
H
  ww
h  --+
     |
     |
     +--+
        |
   -----+
0,0     W
 */    
    fill(0,0,W,H,AIR);
    fill(0,0,W,5,WALL);
    fill(0,0,12,20,WALL);
    fill(0,0,1,H,WALL);
    fill(1,20,5,27,WATER_MAX);
    fill(W-1,0,W,H,WALL);
    fill(1,12,15,18,AIR);
    fill(10,15,W,16,WALL);
    int nstep=1000;
    for(int i=0;i<nstep;i++) {
        for(int j=0;j<H*W;j++) {
            int x=irange(0,W), y=irange(0,H);
            sim(x,y);   
        }
        for(int i=0;i<30;i++) fprintf(stderr,"\n");
        display();
        fprintf(stderr,"step:%d\n",i);
        usleep(50*1000);
        
    }
}
