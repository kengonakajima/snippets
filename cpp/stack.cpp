#include <iostream>


int main()
{
    int a=2;
    std::cerr << "a:" << (unsigned long long)( &a ) << std::endl;
    switch(a){
    case 0:
        {
            int b=2;
            std::cerr << "b:" << (unsigned long long)( &b ) << std::endl;            
        }
        break;
    case 1:
        {
            int c=2;
            std::cerr << "c:" << (unsigned long long)( &c ) << std::endl;                        
        }
        break;
    case 2:
        {
            int b=2;
            std::cerr << "b:" << (unsigned long long)( &b ) << std::endl;                        
        }
        break;
    default:
        {
        }
        break;
    }
}
