#include <vector>
#include <algorithm>
#include <functional>


class A
{
 public:
        int mema;
        int memb;
        A(int a,int b){
                mema = a;
                memb = b;
        }
};


int  main()
{
        std::vector<int> vi;
        vi.push_back(1);
        vi.push_back(8);
        vi.push_back(4);
        vi.push_back(1);
        sort( vi.begin(), vi.end(), std::greater<int>());
        
        std::vector<A> va ;
        va.push_back( A(1,2));
        va.push_back( A(7,8));
        va.push_back( A(5,6));
        va.push_back( A(3,4));
		sort( va.begin(),va.end(), greater<A>() );
}
