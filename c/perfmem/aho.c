/*

ある量のメモリをゼロクリアする．

100M を 1000回と
1000M を 100回と，どうちがうか．
  
 */

#include <stdlib.h>

int main(int argc, char **argv)
{
        
        int i;

        if( strcmp(argv[1],"small")==0){
                char *small;
                small = (char*)malloc( 100 * 1024 * 1024 );
                for(i=0;i<100;i++){
                        memset( small, 0, 100 * 1024 * 1024 );
                }
        } else {
               char *big;
               big = (char*)malloc( 1000 * 1024 * 1024 );
               for(i=0;i<10;i++){
                       memset( big, 0,1000 * 1024 * 1024 );
               }
        }
        return 0;
}
