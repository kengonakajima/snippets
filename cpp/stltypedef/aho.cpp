#include <vector>
#include <iostream>
using namespace std;

typedef vector<int> Tt;

int main()
{

        Tt v;

        v.push_back(1);
        v.push_back(2);        
        Tt::iterator vi;
        for(vi=v.begin(); vi < v.end(); vi++){
                cerr << *vi << endl;
        }
}
