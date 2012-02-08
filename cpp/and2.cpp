#include <iostream>

int main() {
    int a=1, b=2, c=0;
    
    if( a==1 && (++c)){
        std::cerr << "c:" <<c << std::endl;
    }
}
