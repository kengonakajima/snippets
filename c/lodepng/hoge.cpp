#include <stdio.h>
#include "lodepng.h"
#include <stdlib.h>
#include "cumino.h"

int main() {
    int width = 256, height = 128;
    size_t image_size = width * height * 4;
    unsigned char *image = (unsigned char*) malloc( width * height * 4 );

    for(int i=0;i<100;i++) {
        double t0 = now();
        
        for(int i=0;i<width*height;i++) {
            int x = i % width;
            int y = height - 1 - (i/width);
            int ii = y * width + x;
            // rgbaにならべるが、メモリは、 abgrにならんでいるのと、上下逆
            image[ii*4+3] = 0xff; // alpha
            image[ii*4+0] = x%0xff;
            image[ii*4+1] = y%0xff;
            image[ii*4+2] = (x+y)%0xff;
        }

        double t1 = now();
        

        std::vector<unsigned char> png;    
        unsigned e = lodepng::encode( png, image, width, height );
        if(e) {
            print("lodepng::encode error:%s", lodepng_error_text(e) );
            return false;
        }
        double t2 = now();

        unsigned char *ptr = & png[0];
        FILE *fp = fopen( "hoge.png", "w");
        size_t writesz = fwrite( ptr, 1, png.size(), fp );
        print("img:%f enc:%f wsz:%d",t1-t0,t2-t1, writesz);
        fclose(fp);
        
    }
    return 0;
}
