// 1000ディレクトリに1000ファイルづつ作るとか。

#include <stdio.h>
#include <sys/time.h>
#include <string.h>

long long t(){
    struct timeval tmv;
    gettimeofday( &tmv,0);
    return (long long)tmv.tv_usec + (long long)tmv.tv_sec*1000000;
}


#define FILESZ  ( 512 * 1024 )
#define PER_DIR 1000
#define NUM_DIR 100

#define TRIAL_NUM 1000000

main(int argc ,char **argv)
{
       
    int i;
    long long s,e;
    
    // random

    s = t();            
	for(i = 0 ; i < TRIAL_NUM; i++ ){
        if( (i%1000)==0){
            fprintf(stderr, ".");
        }


        int index = ( random() / 8 ) % ( NUM_DIR * PER_DIR );
        
        FILE *fp;
        char filename[1000] ;
        sprintf(filename ,"_%d/%d" , index/PER_DIR,index%PER_DIR );
        fp = fopen( filename , "r+" );

        char buf[32];
        
        size_t faddr = (( random() % FILESZ ) / 32 ) * 32; // align
        fseek( fp, faddr, SEEK_SET );
        fwrite( buf, sizeof(buf), 1, fp );
        fclose( fp );

    }
    e = t();
    fprintf(stderr, "\n%.2f sec\n", (float)(e-s) / (float)(1000*1000));        


    return 0;
}
