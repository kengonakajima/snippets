#include <iostream>
#include <map>
using namespace std;

typedef map<string,string> ht;
int main()
{
	
	ht ss;
	
	ss["asdf"] = "akdjlaksjdhflakjsdf";
	ss["qqq"] = "akjdhfkajsdhfkajhsdkfjahdkfjhakdjfhkasjdhkfaskdfjhaksdf";
	ss["1"] = "9479489uh98402840284029409";

	pair<string,string> s( "asdfa", "asdfa" );
	ss.insert( s );
	
	pair<string,string> t( "asdf", "asdf" );    // ignored
	ss.insert( t );	
		
	ht::iterator it;
	

	for(it=ss.begin(); it != ss.end(); it++ ){
		cout << it->first << ":" << it->second << endl;
		cout << it->first << ":" << ss[it->first] << endl;
	}
	
	
	
	
	return 0;
}