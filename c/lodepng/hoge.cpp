#include <stdio.h>
#include "lodepng.h"
#include <stdlib.h>
#include "cumino.h"
#include "snappy/snappy-c.h"
#include <jpeglib.h>

int memCompressSnappy( char *out, int outlen, char *in, int inlen ) {
    size_t maxsz = snappy_max_compressed_length(inlen);
    assertmsg( outlen >= maxsz, "snappy requires buffer size:%d given:%d", maxsz, outlen );
    size_t osz = outlen;
    snappy_status ret = snappy_compress( in, inlen, out, &osz);
    if(ret == SNAPPY_OK ) return (int)osz; else assertmsg(false,"snappy_compress failed. outlen:%d inlen:%d ret:%d", outlen, inlen,ret );
    return 0;
}
int memDecompressSnappy( char *out, int outlen, char *in, int inlen ) {
    size_t osz = outlen;
    snappy_status ret = snappy_uncompress( in, inlen, out, &osz );
    if(ret == SNAPPY_OK ) return osz; else assertmsg(false,"snappy_uncompress failed");
    return 0;
}


int main() {
    //    int width = 256, height = 256;
    int width = 512, height = 384;    
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

        double t1 = now();
        // LodePNG        
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
        fclose(fp);

        double t3 = now();
        // Snappy
        int sr = memCompressSnappy( snappied, snpsize, (char*) image, image_size );

        double t4 = now();
        // LZ4
        int lr = memCompressLZ4( snappied, snpsize, (const char*)image, image_size );
        double t5 = now();

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
        jpeg_set_quality( &cinfo, 95, true );
        jpeg_start_compress( &cinfo, true );
        jpeg_write_scanlines( &cinfo, jsimg, height );
        jpeg_finish_compress( &cinfo );
        fclose(jfp);
        double t6 = now();
        
        print("img:%f png:%f pngsz:%d snp:%f snpsz:%d lz4:%f lz4sz:%d jpeg:%f",t1-t0,t2-t1, writesz, t4-t3, sr, t5-t4, lr, t6-t5 );
        
        
    }
    return 0;
}
