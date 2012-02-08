#include <iostream>

using namespace std;

struct A {
	int a;
	A(){ a = 0; }
};

struct B:A {
	void inc(){ a++; }
};

struct C:A {
	void dec(){ a--; }
};

struct D:B,C {
	D(){
		B::a = 10;
		C::a = 20;
	}
};

int main()
{
	D d ;
	cout << "test:" << d.B::a << endl;
	cout << "test:" << d.C::a << endl;	
	return 0;
}

