#include <string>
#include <iostream>

using namespace std;


string aho( string s )
{
        return s + "x";
}


int main()
{
        string s = "a";
        string ss = "aa";

        cerr << "sz:" << sizeof(s) << endl;
        cerr << "sz:" << sizeof(ss) << endl;

        const int N = 100000;
        const int M = 200;

        string x;
        for(int i=0;i<N;i++){
                string t;
                for(int j=0;j<M;j++){
                        t += "x";
                        
                }
                x = t;
        }
        cerr << x << endl;

}
