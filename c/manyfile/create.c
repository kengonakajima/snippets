// 1000ディレクトリに1000ファイルづつ作るとか。

#include <stdio.h>
#include <sys/time.h>
#include <string.h>

long long t(){
    struct timeval tmv;
    gettimeofday( &tmv,0);
    return tmv.tv_usec + tmv.tv_sec*1000000;
}

void makeString( char *out, size_t len ){
    int i;
    for( i=0;i<len;i++){
        out[i] = '0' + ( i % 10 );
    }
    out[len-1]='\0';
}

void makeTestData( char *testdata, size_t len ) {
    makeString( testdata, len );
    if( len > 11 ){
        testdata[0] = '\'';
        testdata[1] = '\"';
        testdata[2] = '\\';
        testdata[3] = 'a';
        testdata[4] = 0xe3; //　これ以降の6バイトでutf-8の「ほげ」。不要な変換が入っていないかのテスト。
        testdata[5] = 0x81;
        testdata[6] = 0xbb;
        testdata[7] = 0xe3;
        testdata[8] = 0x81;
        testdata[9] = 0x92;
        testdata[10] = 0x0a;
    }
}



#define FILESZ  ( 512 * 1024 )
#define PER_DIR 1000
#define NUM_DIR 1000

char buf[FILESZ];

main(int argc ,char **argv)
{
    
    makeTestData( buf, sizeof(buf) );
       
	int di,fi;
    long long s,e;

    // create

	for(di = 0 ; di < NUM_DIR; di++ ){
        fprintf(stderr, "di:%d\n",di );
        s = t();        
        for(fi=0; fi < PER_DIR; fi ++ ){
            fprintf(stderr, ".");
            FILE *fp;
            char filename[1000] ;
            sprintf(filename ,"_%d/%d" , di,fi );
            fp = fopen( filename , "w" );
            fprintf(fp , buf );
            fclose( fp );
        }
        e = t();
        fprintf(stderr, "\n%.2f sec\n", (float)(e-s) / (float)(1000*1000));        
	}

    return 0;
}
