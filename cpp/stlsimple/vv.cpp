
#include <vector>
#include <iostream>

using namespace std;
class X
{
 public:
    int a,b,c,d,e,f;    
    X(int hoge){
        a = b = c = d = e = f = hoge;
    }

};



int main()
{
    X* x, *xx;
    typedef vector<X*> il;
    il l;
    l.push_back(new X(100));
    l.push_back(new X(999));
    l.push_back(xx=new X(99));
    l.push_back(new X(88));        
    l.push_back(x = new X(11));
    l.push_back(new X(10));

    for( i=l.begin(); i!=l.end(); i++ ){
        X* cur = *i;
        if( cur == xx ){
            vector<X*>::iterator endit;
            endit = remove( l.begin(), l.end(), x );
    l.erase( endit, l.end() );
    
    endit = remove( l.begin(), l.end(), xx );
    l.erase( endit, l.end() );

    
    vector<X*>::iterator i = l.begin();

    
        cout << "D:" << (*i)->a << endl;
    }
        
}
