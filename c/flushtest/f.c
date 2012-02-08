/*
  f.c 指定した量のデータをディスクにまんべんなく書きこみ続ける．

  このプログラムを使って bdflush チューンの可能性を探る

  これで 180sx(770MB mem) で以下の設定
  echo 100 40 512 512 5000 500000 500000 0 0 > bdflush

  200MB : 130MB/sec
  300MB : 130MB/sec
  350MB : 130MB/sec
  
  
*/

#include <stdio.h>
#include <time.h>

#define BLOCKSZ ( 1024 * 32 )
#define FILESIZE ( 350 * 1024 * 1024 )

int blocknum = FILESIZE / BLOCKSZ;

char buf[BLOCKSZ];

int  main( int argc, char **argv )
{
    int i,total=0;
    FILE *fp;
    time_t last_print, start_time;
    
    if( argc != 2 ){
        printf( "usage: a.out fn\n");
        return 1;
    }

    fp = fopen( argv[1], "w" );
    if(!fp){
        printf( "cannot open file\n");
        return 1;
    }

    start_time = last_print = time(NULL);
    for(i=0;;i++){
        unsigned int pos;
        total++;

        pos = random() % FILESIZE;

        fseek( fp, pos, SEEK_SET );
        fwrite( buf, 1, BLOCKSZ, fp );

        if( time(NULL) != last_print ){
            last_print = time(NULL);
            fprintf(stderr,
                    "Block:%d Sz:%.0fM /sec  avg: %.0fM/sec\n",
                    i,
                    (float)(i * BLOCKSZ)/1024.0f/1024.0f,
                    ((double)total)*((double)BLOCKSZ)/1024.0/1024.0 /
                    (double)( last_print - start_time )
                    
                );
            i=0;
        }
    }
    fclose(fp);
    return 0;
}
