#include <stdlib.h>
#include <stdio.h>
#include <gd.h>

#define SCRW 640
#define SCRH 480

int main ( int argc, char **argv ) {
  gdImagePtr scr;
  gdImagePtr im;
  int white,black,red;


  scr = gdImageCreate ( SCRW, SCRH ); // 出力
  im = gdImageCreate ( 32,32 ); // スプライトのつもり

  
  white = gdImageColorAllocate ( im, 255, 255, 255 ); // 最初の割当だから, 白色が背景色になる 
  black = gdImageColorAllocate ( im,   0,   0,   0 );
  red   = gdImageColorAllocate ( im, 255,   0,   0 );

  // スプライトの絵を作る
  gdImageLine ( im, 0, 0, 32, 32, black );  
  gdImageLine ( im, 0, 32, 32, 0, red );  

  // スプライトを描画
  gdImageCopy( scr, im, 100,100, 0,0, 32,32 );
  gdImageCopy( scr, im, 200,200, 0,0, 32,32 );
  gdImageCopy( scr, im, 100,200, 0,0, 32,32 );    
  
  gdImagePng ( scr, stdout );

  /* 作成したイメージを破棄する */
  gdImageDestroy ( im );
  gdImageDestroy ( scr );
  
  return ( 0 );
}
