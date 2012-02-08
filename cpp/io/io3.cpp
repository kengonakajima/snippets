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

	char c;
	while( in.get(c)){
		out.put(c);
	}


	in.close();
	out.close();
	
	return 0;
}