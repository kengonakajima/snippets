#include <map>
#include <iostream>

using namespace std;

class P
{
        int ppp;
};

class K
{
        int a,b,c,d,e,f;
        map<int,P*> pm;
 public:       
        K(){
                d=1;
        }

        void kkk(){
                cerr << "KKKKKKKKKKK" << pm[0] << endl;
        }
};

int main()
{
        K*k = new K();

        k->kkk();
}
