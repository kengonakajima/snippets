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
	ofstream out2( "out2.txt");
	if( !out || !out2 ){
		cout << "error out" << endl;
		return 1;
	}
	

	
	for( int i=0;i<5;i++){
		int j,k;
		in >> j,k;   // k wont be initialized....
		out << j;
		out2 << k;
	}
	in.close();
	out.close();
	
	return 0;
}