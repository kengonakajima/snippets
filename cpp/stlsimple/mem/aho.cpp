#include <iostream>
#include <string>

using namespace std;

string hoge( int i )
{
	
	string s = "";
	s += i;
	return s;
}
int main(int argc, char **argv )
{
	
	for(int i=0;i<1000;i++){		
		string s = hoge( i );
		cout << &s << endl;
	}
}