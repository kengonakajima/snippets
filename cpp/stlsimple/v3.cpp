#include <vector>
#include <algorithm>

int main()
{
    std::vector<int>v;
    v.push_back(1);
    v.push_back(2);
    remove( v.begin(), v.end(), 1 );
    //    remove( v.begin(), v.end(), 2 );
    v.erase( v.end() );
}
