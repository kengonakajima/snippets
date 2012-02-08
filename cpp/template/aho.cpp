#include <iostream>
using namespace std;

template <class T> void ranger( int x, T* dofunc )
{
        for(int j = 0;j<x;j++){
                for(int i = 0;i<x;i++){
                        dofunc->hoge(i,j);
                }
        }
}

class AhoSender
{
public:
        void  hoge(int i, int j){
                cerr << "HOGE" << i << " " << j << endl;
        }
};

//template <class T> class RangeSender 

int main()
{


        AhoSender *b = new AhoSender();
        ranger( 10, b );
}
