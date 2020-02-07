#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <iostream>
#include <sstream>

template <class T>
struct HogeAllocator {
    using value_type=T;
    HogeAllocator(){}
    template <class U>
    HogeAllocator(const HogeAllocator<U>&) {}
    T* allocate(std::size_t n) {
        std::cerr << "mallocing size:" << sizeof(T) << "," << n << std::endl;        
        return reinterpret_cast<T*>(std::malloc(sizeof(T)*n));
    }
    void deallocate(T* p, std::size_t n) {
        //        static_cast<void>(n);
        std::cerr << "freeing size:" << sizeof(T) << "," << n << std::endl;
        std::free(p);
    }
};

template <class T, class U>
bool operator==(const HogeAllocator<T>&, const HogeAllocator<U>&)
{ return true; }

template <class T, class U>
bool operator!=(const HogeAllocator<T>&, const HogeAllocator<U>&)
{ return false; }


int main() {
    std::vector<int,HogeAllocator<int>> v = {1,2,3};
    std::list<int,HogeAllocator<int>> l = {1,2,3};
    std::map<int,float, std::less<int>, HogeAllocator<std::pair<const int,float> > > m ;
    std::unordered_map<int,float, std::hash<int>,std::equal_to<const int>, HogeAllocator<std::pair<const int,float> > > um;
    // わからない: std::stringにカスタムアロケータを入れる方法。 basic_stringならできた
    std::basic_string< char,std::char_traits<char>,HogeAllocator<char> > s("hoge"); 
    std::cout << s << std::endl;
    // std::stringstreamにカスタムアロケータを入れる方法もわからない。
    std::basic_stringstream< char,std::char_traits<char>,HogeAllocator<char> > sstr;    
    sstr << "poo" << std::endl;
    std::cout << sstr.str() << std::endl;

    //    std::transform t;  // アロケータ不要
    
    return 0;    
}
