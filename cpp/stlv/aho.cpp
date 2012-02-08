#include <vector>
#include <iostream>

using namespace std;

int main()
{
        vector<int> vv;

        vv.push_back(10);
        vv.push_back(10);
        vv.push_back(10);        
        vv.push_back(20);
        vv.push_back(10);                
        vv.push_back(90);        
        vv.push_back(10);
        
        vector<int>::iterator vi;

        for(vi=vv.begin(); vi < vv.end(); vi++){
                if( *vi == 10 ){
						cerr << "DEL" << *vi << endl;
                        vv.erase(vi);
                }
        }

        for(vi=vv.begin(); vi != vv.end(); vi++){
		        cerr << *vi << endl;                
        }

}
