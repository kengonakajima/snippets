#include <stdio.h>
#include <stdlib.h>

#include "cumino.h"
#include "jpeglib.h"


#define SCREEN_HEIGHT 544
#define SCREEN_WIDTH 960

void decodeFrame( unsigned char * jpgdata, size_t jpgdata_len ) {

    // JPEG

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_decompress( &cinfo );

    jpeg_mem_src( &cinfo, jpgdata, jpgdata_len );

    jpeg_read_header( &cinfo, true );
    jpeg_start_decompress( &cinfo );


    JSAMPARRAY img;
    int i, j;
    img = (JSAMPARRAY)MALLOC( sizeof( JSAMPROW ) * SCREEN_HEIGHT );
    for ( i = 0; i < SCREEN_HEIGHT; i++ ) {
        img[i] = (JSAMPROW)MALLOC( sizeof( JSAMPLE ) * 3 * SCREEN_WIDTH );
        for ( j = 0; j < 256; j++ ) {
            // red
            img[i][ j * 3 + 0 ] = 0xff; 
            img[i][ j * 3 + 1 ] = 0;
            img[i][ j * 3 + 2 ] = 0;
        }
    }    

    while( cinfo.output_scanline < cinfo.output_height ) {
        jpeg_read_scanlines( &cinfo,
                             img + cinfo.output_scanline,
                             cinfo.output_height - cinfo.output_scanline
                             );
    }
    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );


    // copy
    size_t outsz = SCREEN_WIDTH * SCREEN_HEIGHT * 4;
    unsigned char *out = (unsigned char*) MALLOC(outsz);
    for(int y=0;y<SCREEN_HEIGHT;y++) {
        for(int x=0;x<SCREEN_WIDTH;x++) {
            int ind = (x+y*SCREEN_WIDTH)*4; // RGBA in Image
            out[ind+0] = img[y][x*3]; // R
            out[ind+1] = img[y][x*3+1]; // G
            out[ind+2] = img[y][x*3+2]; // B
            out[ind+3] = 0xff; // A
        }
    }
    //    g_ss->img->setAreaRaw(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,out,outsz);
    //    g_ss->tex->setImage(g_ss->img);
    
    for ( int i = 0; i < SCREEN_HEIGHT; i++ ) FREE( img[i] );
    FREE(img);
    FREE(out);

}
    

int main() {
    static char buf[1024*1024];
    size_t sz = sizeof(buf);

    for(int i=1;i<10;i++){
        char path[30];
        snprintf(path, sizeof(path), "img%02d.jpg",i );
        bool ret = readFile( path, buf, &sz );
        assert(ret);

        double st = now();
        decodeFrame((unsigned char*)buf,sz);
        double et = now();
        print("elt[%d]:%f", i, (et-st) );
    }
    return 0;
}
