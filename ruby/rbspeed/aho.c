#include <math.h>


int ahofunc( int aaaaa )
{
    int j;
    for(j=0;j<20;j++){
        float ni;
        ni = sqrt( 10.3 );
    }    
}



int (*a)(int b) = ahofunc;

int main()
{
    int i;
    for(i=0;i<30000;i++){
        a(100);
    }
    return 0;
}
