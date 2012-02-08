#include <iostream>
#include <string>


template<class T>  void addparam(  T a )
{
    std::cerr << "test:" << a << std::endl;
    
}

int main()
{
    addparam<int>( 100 );
    addparam<float>( 100 );
    addparam<char>( 100 );
    addparam<char*>( "aho" );
    addparam<std::string>( "aho" );

}
