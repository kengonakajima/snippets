#include <string>
#include <iostream>

using namespace std;

int main()
{
        string s = "aksdjhfkajshdkfjashd,asdfasd,asdf,";
        string q = "AAAAAAAAAAAAAA";
        cerr << s.length() << endl;
        s.erase( s.length()-1);
        cerr << s << endl;
        cerr << s.length() << endl;
}
