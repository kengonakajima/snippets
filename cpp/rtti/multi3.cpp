#include <iostream>
using namespace std;

struct A {
	int a;
	A(int i ){
		a = i;
	}
};

struct B: virtual A{
	B(int i):A(i){
		a *= 2;
	}
	void inc(){ a++; }
};

struct C: virtual A{
	C(int i):A(i){
		a *= 3;
	}
	void dec(){ a--; }
};

struct D: B,C {
	D(int i):A(i),B(i),C(i){}
};

int main()
{
	D d(1);
	
	cout << "test:" <<  d.a << endl;
	
}