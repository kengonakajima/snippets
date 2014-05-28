#include <stdlib.h>
#include <stdio.h>
#include <gd.h>

int main ( int argc, char **argv ) {
  gdImagePtr im;
  int white,black,red;

  /* 100x100 のイメージを作成する */
  im = gdImageCreate ( 100, 100 );

  /* 白色を割り当てる. 最初の割当だから, 白色が背景色になる */
  white = gdImageColorAllocate ( im, 255, 255, 255 );

  /* 黒色と赤色を割り当てる */
  black = gdImageColorAllocate ( im,   0,   0,   0 );
  red   = gdImageColorAllocate ( im, 255,   0,   0 );

  /* (0,0)-(100,100) に斜めの黒い線を描く */
  gdImageLine ( im, 0, 0, 100, 100, black );  

  /* (0,100)-(100,0) に斜めの赤い線を描く */
  gdImageLine ( im, 0, 100, 100, 0, red );  

  /* 標準出力にイメージを gif フォーマットで出力する */
  gdImageGif ( im, stdout );

  /* 作成したイメージを破棄する */
  gdImageDestroy ( im );

  return ( 0 );
}
