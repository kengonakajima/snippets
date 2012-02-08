#include <iostream>
using namespace std;


void foo( int a, int b = 0);

void foo( int a, int b )
{
	cout << "A:" << a << "," << b << endl;
}

int main()
{
	foo(1);
	
}