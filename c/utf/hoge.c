#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#include "ConvertUTF.h"

//  const UTF32** sourceStart, const UTF32* sourceEnd, UTF8** targetStart, UTF8* targetEnd, ConversionFlags flags);
//typedef unsigned int    UTF32;  /* at least 32 bits */
//typedef unsigned short  UTF16;  /* at least 16 bits */
//typedef unsigned char   UTF8;   /* typically 8 bits */
//typedef unsigned char   Boolean; /* 0 or 1 */


int main( int argc, char **argv ) {
    UTF32 data32[] = { 0x00000041, 0x00000042, 0x00000043, 0x00000044, 0x00000045, 0x00000046, 0x00000047, 0x00000048 };
    UTF32 *src32 = data32;
    UTF8 *out8 = (UTF8*)malloc(1024);
    UTF8 *work8 = out8;
    memset(out8,0xff,1024);
    ConversionResult res;
    res = ConvertUTF32toUTF8( (const UTF32**)&src32, src32+8, &work8, work8+8, 0 );
    printf("ptf:%p to %p\n", src32, src32+7);
    printf("res:%d out:%x %x %x %x %x %x %x %x\n",res,out8[0], out8[1], out8[2], out8[3], out8[4], out8[5], out8[6], out8[7] );
    size_t outlen = work8 - out8;
    printf("len:%d",outlen);
}
