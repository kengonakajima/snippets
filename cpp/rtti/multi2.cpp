#include <iostream>
using namespace std;

struct A {
	int a;
	A(){
		a = 10;
	}
};

struct B: virtual A{
	void inc(){ a++; }
};

struct C: virtual A{
	void dec(){ a--; }
};

struct D: B,C {
	
};

int main()
{
	D d;
	
	cout << "test:" <<  d.a << endl;
	
}