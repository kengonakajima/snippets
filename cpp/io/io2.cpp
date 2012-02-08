#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	ifstream in( "n.txt" );
	if( !in ){
		cout << "error" << endl;
		return 1;
	}
	ofstream  out( "out.txt" );
	if( !out ){
		cout << "error out" << endl;
		return 1;
	}

	const streamsize ssize = 1024;
	char cBuf[ssize];
	while( in.getline( cBuf, ssize ) ){
		out << cBuf << endl;
	}

	in.close();
	out.close();
	
	return 0;
}