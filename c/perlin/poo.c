#include <stdio.h>
#include "simplexnoise1234.h"
#include "sdnoise1234.h"
#include "srdnoise23.h"

int main() {
        int x, y;
    //    static float ary[1024][1024];
    static unsigned char rgb[1024*1024*3];
    for(y=0; y<1024; y++) {
        for(x=0; x<1024; x++) {
            float r0=0.02;
            float x0=x*r0, y0=y*r0;
            //            float p=snoise2(xx, yy);
            //            float p=sdnoise2(xx,yy,0,0);
            float p0=srdnoise2(x0,y0,0,0,0);
            p0*=0.5;
            p0+=0.5;

            float r1=0.01;
            float x1=x*r1,y1=y*r1;
            float p1=srdnoise2(x1,y1,0,0,0);
            p1*=0.5;
            p1+=0.5;


            float r2=0.005;
            float x2=x*r2,y2=y*r2;
            float p2=srdnoise2(x2,y2,0,0,0);
            p2*=0.5;
            p2+=0.5;

            float r3=0.0016;
            float x3=x*r3,y3=y*r3;
            float p3=srdnoise2(x3,y3,0,0,0);
            p3*=1.2;
            p3*=0.5;
            p3+=0.5;
            
            float pp=(p0+p1+p2+p3)*0.25;
            
            //            printf("%f\n",p);
            unsigned char val=pp*255;
            int ind=x+y*1024;
            unsigned char r=val, g=val, b=val;
            if(val<105) { r=0x2f; g=0; b=0x2f; }
            //            else if(val>145) { r=239; g=200; b=163; }
            //            else if(val>165) { r=0xff; g=0xff; b=0xff; }
            //            else { int k=val-120; r=107+k; g=190+k; b=50+k;}
            
            rgb[ind*3]=r;
            rgb[ind*3+1]=g;
            rgb[ind*3+2]=b;
        }
    }

    FILE *fp=fopen("hoge.rgb","w");
    fwrite(rgb,1,1024*1024*3,fp);
    fclose(fp);


}
