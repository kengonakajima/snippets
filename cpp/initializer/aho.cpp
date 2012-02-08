#include <iostream>
using namespace std;

class Hoge
{
 public:
        int a;
        int b;
        Hoge(){
                a = 8;
                b = 8;
        }
};


int main()
{
        Hoge h;

        cout << h.a << endl;
        
}
