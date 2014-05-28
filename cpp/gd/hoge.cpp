#include <stdlib.h>
#include <stdio.h>
#include <gd.h>

#include "cumino.h"

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

  int n = 5000;

  for(int i=0;i<n;i++) {
      for(int j=0;j<200;j++) {
          int x = irange(0,600), y = irange(0,400);
          gdImageCopy( scr, im, x,y, 0,0, 32,32 ); // スプライトを描画
      }
      //      gdImagePng ( scr, stdout );
      void *out;
      int sz;
      out = gdImagePngPtr( scr, &sz );
      //      print("outsz:%d",sz);
      if( i==5) {
          writeFile( "poo.png", (char*) out,sz );
      }
      gdFree(out);
  }
  

  /* 作成したイメージを破棄する */
  gdImageDestroy ( im );
  gdImageDestroy ( scr );
  
  return ( 0 );
}
