#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;


int trim_right( string &str )
{
	string::size_type pos = str.find_last_not_of( " \t" );
	if( pos == str.npos ){
		return 1; // not found
	}
	if( pos == str.size() - 1 ){
		return 1; // hit, but end.
	}
	str.erase( pos + 1);
	return 0;
}

char hogefunc( char &a )
{
	return toupper(a);
}
int main()
{
	char aho[] = "asdf";
	string q = "HOGe";
	string p = "HOGe";
	
	string x = q + aho;

	
	cout << x << endl;
	
	if( q == aho ){
		cout << "equal!" << endl;
	} else {
		cout << "diff" << endl;
	}
	
	if( q == p ){
		cout << "equal!" << endl;
	} else {
		cout << "diff" << endl;
	}
	
	//
	string b = "asdf";
	b.erase(1);
	cout << "b:" << b << ":" << endl;
	
	
	//

	string aaa = "adfasdfasdfasdf \tasdfas \t ";
	trim_right(aaa);
	cout << aaa << ":" << endl;
	
	// transform
	
	string cc = "asdfasd";
	transform( cc.begin(), cc.end(), cc.begin(), hogefunc );
	cout << "up:" << cc << endl;
	
	
	return 0;
}