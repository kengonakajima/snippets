#include <map>
#include <iostream>

std::map< int, int >m;


int main() {
    m.insert( std::map<int,int>::value_type( 5, 10 ) );
    m.insert( std::map<int,int>::value_type( 8, 100 ) );

    std::cerr << "aa:" << m[5] << std::endl;
    //    std::cerr << "bb:" << m[6] << std::endl;
    m.insert( std::map<int,int>::value_type( 6, 0 ) );
    m.insert( std::map<int,int>::value_type( 6, 100 ) );
    m[6]=100;
    std::cerr << "bb:" << m[6] << std::endl;    
}
