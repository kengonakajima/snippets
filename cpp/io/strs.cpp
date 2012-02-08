#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main()
{
	stringstream ss;
	ss << " :aaa bbb  / ccc  , aaa , bbb \t  d \n ddd:";
	string sz_string, sz_one;
	while( ss >> sz_one){
		sz_string += "*" + sz_one;
	}
	cout << "|" << sz_string << "|" << endl;
	return 0;
}