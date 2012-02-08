#include <stdio.h>


#include <stdlib.h>


void ahoaho(){
        fprintf( stderr, "aho\n" );
}
int main() {

    if( daemon( 1,0 ) != 0 ){
        fprintf( stderr, "daemon error\n" );
        return 1;
    }
    
    while(1){
        sleep(1);
        ahoaho();
    }
}
