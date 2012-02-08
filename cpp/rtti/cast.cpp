#include <iostream>

class Base 
{
	int b;
};
class Derived : public Base 
{
	
};


class Shape {
	public:
	virtual void set() { }
	
};

class Drawable {
	public:
	virtual void draw() { std::cout << "drawing" << std::endl; }
	
};

class Circle : public Shape, public Drawable {
	public:
//	void draw();
//	virtual void set();
};


int main()
{
	int ival;
	long lval = static_cast<long>(ival);
	int *iptr = reinterpret_cast<int*>(lval);
	
	const int * ciptr;
	int* iiptr = const_cast<int*>(ciptr);


	Base base;
	Derived derived;

//	Derived* pd1 = dynamic_cast<Derived*>(&base);
	Derived* pd2 = dynamic_cast<Derived*>(&derived);
	Base* pb = dynamic_cast<Base*>(&derived);

	Shape * shape = new Circle;
	Drawable *drawable = dynamic_cast<Drawable*>(shape);
	if( drawable ){
		drawable->draw();
	}
}