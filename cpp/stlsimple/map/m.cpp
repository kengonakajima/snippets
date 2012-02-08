#include <map>
#include <string>
#include <iostream>

using namespace std;

int main()
{
        map<string,int> ms;

        cout << ms["asdf"] << endl;
        cout << "---" << endl;
        ms["asdf"] = 1;
        ms["asdf"] = 2;
        ms["addsdf"] = 3;

        cout << ms["asdf"] << endl;
}
