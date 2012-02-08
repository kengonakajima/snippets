
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

	static const string _doc = "CGAATCTAAAAATAGATTCGGACGTGATGTAGTCGTACAAATGAAAAAGTAAGCC";
//	static const string _doc = "CGAATCGAAGTAAGCCTTCGGACGTGATGTAGTCGTACAAATGAAAAAGTAAGCC";
	static const int ITERATIONS = 1000000;
	
	struct timeval started;
	gettimeofday( &started, 0 );	
	long start_msec = (started.tv_sec * 1000 * 1000 + started.tv_usec)/1000;
	
	int length = 1;
	
	for( int i = ITERATIONS; i <= ITERATIONS * 2; i++ ){
		
		if( i == (ITERATIONS + 100 ) ){
			break;
		}
		length = (int) (log( (double)i) / log(4) );
		string matchthis;
		generateWord( matchthis, i, length+1);
		cout << " length:" << length << " i:" << i << "matchthis: '" << matchthis << "'  size=" << matchthis.size() << " ";
		static const boost::regex e( matchthis );
		boost::smatch res;
		bool b = regex_search(_doc, res, e);
		cout << "b:" << b << " ORG: " << _doc << endl;
		if( b ){
			struct timeval ended;
			gettimeofday( &ended, 0 );
			long msec = (ended.tv_sec * 1000 * 1000 + ended.tv_usec )/1000 - start_msec;
			cout << "found " << matchthis << " at " << i << " " << " it took " << msec << "milliseconds" << endl;
		}
	}
	struct timeval ended;
	gettimeofday( &ended, 0 );
	long msec = (ended.tv_sec * 1000 * 1000 + ended.tv_usec )/1000 - start_msec;	
	cout << "C++/boost regex took " << msec << "milliseconds" << endl;	
	return 0;
}
