
#include <string>
#include <iostream>
#include <math.h>
#include <boost/regex.hpp>

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



void generateWord( string &buf, int value, int length )
{
	buf = "";
	int current = value;
	for(int i=0;i<length;i++){
		int v = current % 4;
		current = current / 4;
		buf += convert(v);
	}
}

int main( int argc, char **argv )
{ 

//	static const string _doc = "CGAATCTAAAAATAGATTCGGACGTGATGTAGTCGTACAAATGAAAAAGTAAGCC";
	static const string _doc = "CGAATCGAAGTAAGCCTTCGGACGTGATGTAGTCGTACAAATGAAAAAGTAAGCCA";
	static const int ITERATIONS = 1000000;

	int length;
	
	length = (int) (log( (double)ITERATIONS) / log(4) );
	cout << "length:" << length << endl;
	
	string w;
	generateWord( w, ITERATIONS, length+1);
	cout << "x: '" << w << "'" << endl;
	string matchthis = (static_cast<string>(".*")) + w + ".*";
	static const boost::regex e( matchthis );
	bool b = regex_match(_doc, e);
	if( b ){
		cout << "found " << endl;
	} else {
		cout << "not found" << endl;
	}
	////////////////////////////////////
	string matchthis2 = (static_cast<string>(".*")) + "AAAGTAAGCC" + ".*";
	static const boost::regex e2( matchthis2 );
	bool b2 = regex_match(_doc, e2);
	if( b2 ){
		cout << "found " << endl;
	} else {
		cout << "not found" << endl;
	}	
	////////////////////////////////////
	string w3;
	generateWord( w3, ITERATIONS, length+1);
	string matchthis3 = (static_cast<string>(".*")) + w3 + ".*";
	static const boost::regex e3( matchthis3 );
	boost::smatch res;
	bool b3 = regex_search(_doc, res, e3);
	if( b3 ){
		cout << "found " << endl;
	} else {
		cout << "not found" << endl;
	}		
	
	return 0;
}
