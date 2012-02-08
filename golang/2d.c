#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int doit( int *data ) {
    int total = 0;
    int x,y;
    for(y=0;y<1024;y++){
        for( x=0;x<1024;x++){
            data[y*1024+x] ++;
            total += data[y*1024+x];
        }
    }
    return total;
}
int main() {
    int *data = (int*)malloc( sizeof(int) * 1024 * 1024 );
    int i;
    for(i=0;i<100;i++){
        doit(data);
    }
}
