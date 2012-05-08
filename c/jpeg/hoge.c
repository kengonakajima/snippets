#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"



int main() {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);
    size_t sz = 1024*1024;
    unsigned char *outbuf = malloc(sz);
    unsigned long outsize = sz;
    jpeg_mem_dest(&cinfo, &outbuf, &outsize );

    cinfo.image_width = 128;
    cinfo.image_height = 128;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults( &cinfo );
    jpeg_set_quality( &cinfo, 75, 1 );
    jpeg_start_compress( &cinfo, 1 );

    JSAMPROW row_pointer;

    unsigned char pixels[ 128 * 3 ] ;
    int i;
    
    for(i=0;i<sizeof(pixels);i++){
        switch(i%3){
        case 0:
            pixels[i] = 0xff;
            break;
        case 1:
            pixels[i] = (i%0xff);
            break;
        case 2:
            pixels[i] = 0;
            break;
        }
    }
    while( cinfo.next_scanline < cinfo.image_height ) {
        row_pointer = pixels;
        jpeg_write_scanlines( &cinfo, &row_pointer, 1 );
    }
    jpeg_finish_compress(&cinfo);

    printf( "outlen: %d\n", (int)outsize );
    FILE *fp = fopen("./out.jpg","w");
    fwrite( outbuf, 1, outsize, fp);
    fclose(fp);
    return 0;
}
