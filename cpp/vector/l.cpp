#include <list>


typedef std::list<int> il;

il aho[1000000];

int main()
{
    for(int i=0;i<1000000;i++){
        il l;
        l.push_back( i);
        l.push_back( i);        
        aho[i] = l;
    }
    sleep(1000);
}
