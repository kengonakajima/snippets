#include <stdio.h>
#include <stdlib.h>

#include <jpeglib.h>

#include "cumino.h"


int main() {
    //    int width = 256, height = 256;
    int width = 256, height = 256;    
    size_t image_size = width * height * 4;
    unsigned char *image = (unsigned char*) malloc( width * height * 4 ); // PNG用の画像
    // JPEG用の画像
    JSAMPARRAY jsimg = (JSAMPARRAY) malloc( sizeof(JSAMPROW) * height );
    for(int y=0;y<height;y++) {
        jsimg[y] = (JSAMPROW) malloc( sizeof(JSAMPLE)*3*width);
    }
    
    size_t snpsize = image_size * 2;
    char *snappied = (char*) malloc( snpsize);
    for(int i=0;i<100;i++) {
        double t0 = now();
        
        for(int i=0;i<width*height;i++) {
            int x = i % width;
            int y = height - 1 - (i/width);
            int ii = y * width + x;
            // rgbaにならべるが、メモリは、 abgrにならんでいるのと、上下逆
            image[ii*4+3] = 0xff; // alpha
#if 0
            // これでやるとsnappyとlz4は、最悪ケースになるようだ。。
            image[ii*4+0] = x%0xff; // r
            image[ii*4+1] = y%0xff; // g 
            image[ii*4+2] = (x+y)%0xff; // b
#else

            image[ii*4+0] = x%0xff;
            image[ii*4+1] = (y/16)*15;
            image[ii*4+2] = (x/16)*15;
#endif

            // JPEG用
            jsimg[y][x*3+0] = x%0xff; 
            jsimg[y][x*3+1] = y%0xff;
            jsimg[y][x*3+2] = (x+y)%0xff;
        }

        // libjpeg
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress( &cinfo);
        FILE *jfp = fopen( "hoge.jpg", "w");
        jpeg_stdio_dest(&cinfo,jfp);
        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
        jpeg_set_defaults( &cinfo);
        jpeg_set_quality( &cinfo, 75, true );
        jpeg_start_compress( &cinfo, true );
        jpeg_write_scanlines( &cinfo, jsimg, height );
        jpeg_finish_compress( &cinfo );
        fclose(jfp);
    }
    return 0;
}
