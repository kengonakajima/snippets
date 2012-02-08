#include <stdio.h>

#define TRYNUM (1000 * 10000)

#define BITS_CHAR 8

#define N ( 1024 * 1024 * 32 )

int ia[N];
char ia2[N/BITS_CHAR];


void setBit( char *a, int n )
{
    int index = n / BITS_CHAR;
    switch( n % BITS_CHAR ){
    case 0:        a[index] |= 0x01;        break;
    case 1:        a[index] |= 0x02;        break;
    case 2:        a[index] |= 0x04;        break;
    case 3:        a[index] |= 0x08;        break;
    case 4:        a[index] |= 0x10;        break;
    case 5:        a[index] |= 0x20;        break;
    case 6:        a[index] |= 0x40;        break;
    case 7:        a[index] |= 0x80;        break;        
    }
    
}

int main(int argc, char **argv)
{
    fprintf(stderr, "aho\n");
    
    if( argv[1][0] == '0' ){
        int i;
        for(i=0;i<TRYNUM;i++){
            ia[random()%N]=1;
        }
        fprintf( stderr,"%d\n",ia[N-1]);
        return 0;
    } else if( argv[1][0] == '1'){
        int i;
        for(i=0;i<TRYNUM;i++){
            setBit( ia2, random()%N );
        }
        fprintf(stderr, "%d\n", ia2[N/BITS_CHAR-1]);
    } else {
        fprintf(stderr, "fuck\n");
    }
}
