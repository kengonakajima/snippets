#include <iostream>

#define GENLOG 1

#if GENLOG
#define HOGE(expr) expr

#else
#define HOGE(expr)

#endif

int main()
{
    HOGE( std::cerr << "aaho" << "oakdjfoas" << std::endl );
}
