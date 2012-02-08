#include <iostream>
int aho[10];

int main()
{
    int *k = aho+2;

    *k = 4;

    std::cerr <<  aho[2] << std::endl;
    
}
