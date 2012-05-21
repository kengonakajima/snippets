#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
#if 0    
    write( 1, "AAA\n", 4 );
    write( 2, "BBB\n", 4 );
#else
    pwrite( 1, "AAA\n", 4,0 );
    write( 2, "BBB\n", 4 );
#endif    

}
