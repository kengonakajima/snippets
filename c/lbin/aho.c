
#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
  512byte 10万ブロックで r-f* : 13sec r-l* : 12sec drink-f* : 3 drink-l* : 1
  4096byte 5万ブロックで r-f* : 50sec r-l* : 50sec drink-f* : 16 drink-l* : 22

  fwrite の方が、write より倍ぐらい速いのはいろいろ工夫されてるせいか。
  順に書きこんで w/a で速度変化はない。
  プロセスの使用メモリは、どっちも300kくらいで、差はない。CPUも。
  ほとんどsystem だ。
  
 */
int main()
{

    char buf[4096];
    int num = 50000;
    time_t t;
    int i;

    /* f* version */
#if 1
    FILE *fp;
    fp = fopen( "/opt/ringo/hoge" , "a");   
    time( &t);fprintf( stderr , "%s\n" , ctime( &t ) );
    for(i=0;i<num;i++){
        fseek( fp , i * sizeof( buf ) , SEEK_SET );
        fwrite( buf , sizeof(buf),1 , fp );
    }
    time(&t);fprintf( stderr , "%s\n" , ctime( &t ) );    
    fclose(fp);
#endif

    /* read/write/lseek version */
#if 0
    {
        int fd;
        
        fd = open( "/opt/ringo/hoge" , O_RDWR );

        time( &t);fprintf( stderr , "%s\n" , ctime( &t ) );
        for(i=0;i<num;i++){
            lseek( fd , i * sizeof( buf ) , SEEK_SET );
            write( fd , buf , sizeof( buf ));
        }
        time( &t);fprintf( stderr , "%s\n" , ctime( &t ) );        
        close(fd);
    }
    
#endif
}







