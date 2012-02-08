#include <iostream>
using namespace std;

void ringo( char &a )
{
	cout << "a:" << a << endl;
	*&a  = 52;
}

int main()
{
	char a = 50;
	ringo(a);
	ringo(a);
	return 0;
}