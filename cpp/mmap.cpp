#include <map>
#include <algorithm>
#include <iostream>

int main()
{
    std::multimap<int,int> mm;
    typedef std::pair<int,int> ii_t;
    
    mm.insert( ii_t( 100, 123456 ));
    mm.insert( ii_t( 70, 234567 ));
    mm.insert( ii_t( 200, 345678 ));
    mm.insert( ii_t( 70, 567890 ));


    // 全部表示
    std::multimap<int,int>::iterator it;
    for( it = mm.begin(); it != mm.end(); it ++ ){
        std::cerr << "val:" << (*it).first << "," << (*it).second << std::endl;
    }

    // 条件あってるやつ見つける
    std::pair< std::multimap<int,int>::iterator,  std::multimap<int,int>::iterator > itp = mm.equal_range(70);
    std::multimap<int,int>::iterator it2;
    for( it2 = itp.first; it2 != itp.second; ++it2 ){
        std::cerr << "val2:" << (*it2).first << "," << (*it2).second << std::endl;
        if( (*it2).second == 234567 ){
            mm.erase( it2 );
            break;
        }
    }
    
    // 全部表示    
    for( it = mm.begin(); it != mm.end(); it ++ ){
        std::cerr << "val:" << (*it).first << "," << (*it).second << std::endl;
    }    
    
}
