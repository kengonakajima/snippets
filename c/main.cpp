#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

char buf[10000];

void test(size_t sz){
  
  FILE *fp = fopen( "test.txt", "r" );
  if( !fp ){
    free(buf);
    return;
  }
  size_t rsz = fread( buf, 1, sz, fp );
  for( int i = 0; i < rsz; i++ ){
    if( buf[i] == '"' ){
      
    }
  }
  fclose(fp);

}


int main(){
  struct timeval tv_s;
  struct timeval tv_e;
  gettimeofday( &tv_s, NULL );

  for( int i = 0; i < 1000; i++ ){
          test(1000);
          //   test(1345);
  }
  gettimeofday( &tv_e, NULL );
  std::cout << ((tv_e.tv_sec-tv_s.tv_sec)*1000*1000+tv_e.tv_usec-tv_s.tv_usec)/1000 << "[ms]\n";
}
