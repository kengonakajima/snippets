#include <boost/regex.hpp>
#include <string>
#include <iostream>
#include <typeinfo>

using namespace std;


string convert(int value)
{
	switch(value){
		case 0: return "A";
		case 1: return "G";
		case 2: return "T";
		case 3: return "C";
		default: return "0";
	}
}



string generateWord( int value, int length )
{
	string buf = "";
	int current = value;
	for(int i=0;i<length;i++){
		int v = current % 4;
		current = current / 4;
		buf += convert(v);
	}
	return buf;
}

int main()
{
	string q;
	
	q = ".+" + generateWord( 10000, 5 ) + ".+";
	cout << "q:" << q << endl;
	
	static const string s = "CGAATCGAAGTAAGCCTTCGGACGTGATGTAGTCGTACAAATGAAAAAGTAAGCCAAGACAA";

	for(int i=0;i<10;i++){
		static const string matchthis = (static_cast<string>(".+")) + "GAAGTAAGCC" + ".+";
		static const boost::regex e( matchthis );
		bool res = regex_match( s, e );
		cout << "res:" << res << "typeinfo: " << typeid(matchthis).name() << endl;	



		static const boost::regex ee( q );
		res = regex_match( s, ee );
		cout << "res:" << res << "typeinfo: " << typeid(q).name() << endl;	
	}
	
	return 0;
}