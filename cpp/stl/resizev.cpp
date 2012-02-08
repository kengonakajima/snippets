#include <iostream>
#include <vector>

using namespace std;



int main()
{
	vector<int> v;
	
	v.push_back( 10 );
	v.push_back( 20 );
	v.push_back( 30 );
	
	vector<int>::iterator i = v.begin();
	
	cout << v.begin() << endl;
	
	return 0;
	
}