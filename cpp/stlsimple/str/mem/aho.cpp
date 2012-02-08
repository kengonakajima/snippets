#include <string>
#include <iostream>
#include <vector>

using namespace std;

namespace Test{
template <class T>
class allocator
{
 public:
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;

        void construct( pointer p, const T& value ){
                new ( (void*)p ) T(value);
        }
        
        pointer allocate( size_type num, void *p = NULL ){
                cerr << "allocate:" << num << endl;
                pointer newp = (pointer)new char[num*sizeof(T)];
                return newp;
        }

        void deallocate( pointer p, size_type num ){
                cerr << "deallocate:" << num << endl;
                ::operator delete( (void*)p);
        }
        void destroy(pointer p){

        }
        
};
};


        
void fuck(string s)
{
        int i;
        for(i=0;i<15000000;i++){
                s += "asdasdf";
        }
}

vector<int, Test::allocator<int> > vfuck(vector<int, Test::allocator<int> > v)
{
        int i;
        for(i=0;i<15000000;i++){
                v.push_back( i );
        }
        return v;
}

vector<int, Test::allocator<int> >  vvfuck(
        vector<int, Test::allocator<int> > v,
        vector<int, Test::allocator<int> > v2,
        int i)
{
        v.push_back(i);
        v2.push_back(i);        
        return v;
}

        
int main()
{
        vector<int, Test::allocator<int> > v;
        v.push_back(1);
        vvfuck(v,v,10);
        
                
#if 0        
        int i=0;
 hoge:
        i+=1;
        if(i>10)return 0;
                vector<int, Test::allocator<int> > v;
                v.push_back(1);
                printf("asdf\n");
                goto hoge;
#endif
                
#if 0
        
        int i;

        cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl;
        string s = "asdf";
        vector<int, Test::allocator<int> > v;
        v.push_back(1);
        for(i=0;i<5;i++){
                cerr << "---------------------------"<<endl;
//                fuck(s);
                vfuck(v);
        }

        cerr << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" << endl;
#endif
        
}
