#include <boost/pool/object_pool.hpp>
#include <iostream>

class Hoge
{
 public:
    int a[100];
    Hoge(int aa){ a[0]=aa; }
};

int main()
{
    boost::pool<> p( sizeof(Hoge) );
#if 0
    for(int i=0;i<1000000;i++){
        //        if( (i%1000)==0)std::cerr << ".";
        //        usleep(10*1000);
        Hoge *h = p.construct( 100 );
        boost::pool::details::PODptr<size_type> pod = boost::pool::details::find_POD(h);
        


        std::cerr << "gn:"<< p.get_next_size() << " asz:" << std::endl;
    }
#endif
}
