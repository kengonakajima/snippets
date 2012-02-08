/*
  int を２のべき乗で割るとunsigned intより遅いか？
  たいしてかわらん。
  -Oしたらもっとかわらん
 */

#include <stdio.h>


int main(){
    unsigned int tot=0,tot2=0;
    int i;
    for(i=0;i<1000000000;i++){
        tot += i / 2;
    }
    printf("%u\n", tot );
}

