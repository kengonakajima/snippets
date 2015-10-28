#include <map>
#include <iostream>

std::map< int, void* >m;


int main() {
    int a = 10;
    char b = 20;
    m.insert( std::map<int,void*>::value_type( 10, &a ) );
    m.insert( std::map<int,void*>::value_type( 8, &b ) );

    std::cerr << "hoge:" << m[5] << std::endl;
    std::cerr << "hoge:" << m[10] << std::endl;
}
