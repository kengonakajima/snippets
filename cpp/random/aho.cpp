#include <iostream>
#include <stdlib.h>

int main()
{
    for(int i=0;i<100000000;i++){
        srand(i);
        //        if( ( random() % 1000000 ) == 0){
        //            std::cerr << ".";
        //        }
    }
}

