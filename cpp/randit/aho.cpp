#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

int main()
{
    std::map<int,int> m;
    m[4994] = 1;
    m[849] = 2;
    m[489383] = 3;
    m[4801] = 4;
    m[101] = 5;

    std::map<int,int>::iterator it;

    it = m.begin();

    std::cerr<< m.size()<< std::endl;
    m.erase(101);
    std::cerr<< m.size()<< std::endl;


    std::vector<int> v;
    v.push_back(101);
    v.push_back(1011);    
    v.push_back(10111);
    std::cerr<< v.size()<< std::endl;
    std::vector<int>::iterator end_it = remove( v.begin(), v.end(), 1011 );
    v.erase( end_it, v.end());
    std::cerr<< v.size()<< std::endl;
}
    
