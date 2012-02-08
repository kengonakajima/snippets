#include <stdio.h>

char hoge[20 * 1024 * 1024];

int
main( int argc,char **argv )
{
    int i;
    system("date");
    for(i=0;i<30;i++){
        int k;
        for(k=0;k<sizeof(hoge);k++){
            hoge[k]=i;
        }
    }
    system("date");    
    return 0;
}
