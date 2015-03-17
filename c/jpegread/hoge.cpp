#include <stdio.h>
#include <stdlib.h>

#include "cumino.h"
#include "jpeglib.h"


#define SCREEN_HEIGHT 544
#define SCREEN_WIDTH 960

void decodeFrame( unsigned char * jpgdata, size_t jpgdata_len, unsigned char *out ) {

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
        for ( j = 0; j < SCREEN_WIDTH; j++ ) {
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
    for(int y=0;y<SCREEN_HEIGHT;y++) {
        for(int x=0;x<SCREEN_WIDTH;x++) {
            int ind = (x+y*SCREEN_WIDTH)*3; // RGB in Image
            out[ind+0] = img[y][x*3]; // R
            out[ind+1] = img[y][x*3+1]; // G
            out[ind+2] = img[y][x*3+2]; // B
            out[ind+3] = 0xff; // A
        }
    }
    
    for ( int i = 0; i < SCREEN_HEIGHT; i++ ) FREE( img[i] );
    FREE(img);
}



void encodeFrame( unsigned char *pixels, int index ) {
    
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    size_t outsz = 1024*256;
    unsigned char *outbuf = (unsigned char*) MALLOC(outsz);
    jpeg_mem_dest(&cinfo, &outbuf, &outsz );

    cinfo.image_width = SCREEN_WIDTH;
    cinfo.image_height = SCREEN_HEIGHT;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults( &cinfo );
    jpeg_set_quality( &cinfo, 75, 1 );
    jpeg_start_compress( &cinfo, 1 );

    JSAMPROW row_pointer;
    int y=0;
    while( cinfo.next_scanline < cinfo.image_height ) {
        row_pointer = pixels + y * SCREEN_WIDTH * 3;
        jpeg_write_scanlines( &cinfo, &row_pointer, 1 );
        y++;
    }
    jpeg_finish_compress(&cinfo);

    printf( "outlen: %d\n", (int)outsz );
    char path[100];
    snprintf(path,sizeof(path), "out%02d.jpg", index );
    FILE *fp = fopen( path,"w");
    fwrite( outbuf, 1, outsz, fp);
    fclose(fp);
}



int main() {
    static char buf[1024*1024];
    

    for(int i=4;i<=14;i++){
        size_t sz = sizeof(buf);
        char path[30];
        snprintf(path, sizeof(path), "img%02d.jpg",i );
        bool ret = readFile( path, buf, &sz );
        assert(ret);

        unsigned char raw[3*SCREEN_WIDTH*SCREEN_HEIGHT];
        
        double st = now();
        decodeFrame((unsigned char*)buf,sz, raw );
        double et = now();
        print("dec[%d]:%f", i, (et-st) );

        st = now();
        encodeFrame(raw,i);
        et = now();
        print("enc[%d]:%f", i, (et-st));
    }
    return 0;
}
