#include <map>
#include <stdio.h>
int main()
{
    std::map<int,int>m;

    m[1] = 1;
    m[2] = 2;

    printf("%d %d\n", m[2], m.size() );    
    m.erase(2);
    printf("%d %d\n", m[2], m.size() );
    printf("%d %d\n", m[2], m.size() );    
}
