#include <map>
#include <string>
#include <iostream>

using namespace std;

int main()
{
        map<string,int> ms;

        ms["a"] = 1111;
        ms["b"] = 22;
        ms["c"] = 333;

        cout << ms["b"] <<endl;
        
        ms.erase("b");

        cout << ms["b"] << endl;
}
