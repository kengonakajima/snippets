#include <unistd.h>


int main()
{
    int i;

    for(i=0;i<10000;i++){
    
    char path[] = "hoge";
    char n[] = "fuck";

    rename( path , n );
    rename( n , path );
    }
    
}
