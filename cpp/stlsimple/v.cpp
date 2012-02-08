#include <vector>
#include <list>
#include <set>
#include <map>

#include <sys/time.h>
#include <iostream>

#define foreach( a,b,c ) for(a::iterator c=(b).begin(); (c)!=(b).end(); (c)++ )

struct timeval tp;

#define S gettimeofday( &tp, NULL )
#define E {\
        float dt;\
        struct timeval tp2;\
        gettimeofday( &tp2, NULL ); \
        dt = (tp2.tv_sec - tp.tv_sec )*1000000;\
        dt += (tp2.tv_usec - tp.tv_usec );\
        cout << "dt:" << dt << endl;\
}
        
                 
using namespace std;


bool odd( int x )
{
        return (x % 2 ) > 0;
}

//bool positive( int x )
//{
//        return x > 0;
//}
binder2nd< greater<int>  > positive(  greater<int>(), 0 );



int main()
{
        S;
        
//vector
        typedef vector<int> intv;
        
        intv v (10);

        v[5] = 100;
        
        cout << "hoge" << endl;

        v.push_back(10099);
        v.push_back(10099);

        cout << "A:" << v.size() << endl;

        for(vector<int>::iterator i = v.begin(); i != v.end();  i++ ){
                *i+= 7;
                cout << "B:" << *i << endl;
        }
        foreach(vector<int>,v,i){
                *i += 9;
                cout << "C:" << *i << endl;                
        }
        
        cout << "VF:" << *find( v.begin(), v.end(), 107 ) << endl;        
        cout << "VF:" << *find( v.begin(), v.end(), 116 ) << endl;        

        cout << "VAF:" <<  *adjacent_find(v.begin(), v.end() ) << endl;
        cout << "COUNT:" << count(v.begin(), v.end(), 16 ) << endl;

        sort( v.begin(), v.end() );
        
        foreach(vector<int>,v,i){
                cout << "SORTed:" << *i << endl;                
        }        

        vector<int>::iterator it = find_if( v.begin(), v.end(), &odd );
        cout << "ODD:" << *it << endl;
        
        fill( v.begin(), v.end(), 10);
        cout << "COUNT:" << count(v.begin(), v.end(), 16 ) << endl;

// array
        int aho[] = { -4,-3,-2,-1,0,1,2,3,4,5,6,7,8 };
        int *iit = find_if( aho, aho+9, positive );
        if(iit){
                cout << "ARRAY_ODD:" << *iit << endl;
        }
        return 0;

//list
        typedef list<int> il;
        il l;
        l.push_back(999);
        l.push_back(888);
        l.push_front(11);

        list<int>::iterator ii = l.begin();
        ii++;       
        l.insert( ii,100);

        
        foreach(il,l,i){
                cout << "D:" << *i << endl;
        }
        for( list<int>::iterator i=l.begin(); i!=l.end(); i++ ){

        }
        

        
        return 0;
}
