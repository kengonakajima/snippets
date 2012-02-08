#include <list>
#include <vector>
#include <set>
#include <map>
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
    X* x;
    typedef list<X*> il;
    il l;
    l.push_back(new X(100));
    l.push_back(new X(999));
    l.push_front(x = new X(11));

    list<X*>::iterator ii = l.begin();
    ii++;       
    l.insert( ii, new X(100));

    l.remove( x );
        

    for( list<X*>::iterator i=l.begin(); i!=l.end(); i++ ){
        cout << "D:" << (*i)->a << endl;
    }
        
}
