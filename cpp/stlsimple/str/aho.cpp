#include <string>
#include <iostream>

using namespace std;


void hoge(string &s) 
{
	s += "B";
	cout << s << endl;;
}
int main()
{
//	string s = "akjsdhfkasjdhkfa";
//	hoge(s);
        printf("%s\n", (std::string("map/data") + "asdf" ).c_str() );
}
