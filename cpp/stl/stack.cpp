#include <iostream>
using namespace std;

template <typename T>
class Stack
{
	public:
	Stack(int s = 100) : size(s), top(-1)
	{
		data = new T[size];
	}
	~Stack()
	{
		delete [] data;
	}
	void push(const T& x)
	{
		data[++top] = x;
	}
	T pop()
	{
		return data[top--];
	}
	bool isEmpty() const
	{
		return top == -1;
	} 
	bool isFull() const
	{
		return top == (size - 1);
	}
	int getSize() const
	{
		return size;
	}
	private:
	int size;
	int top;
	T* data;
};

int main()
{
	Stack<int> s(5) ;
	s.push(100);
	s.push(200);
	s.push(300);
	s.push(400);
	s.push(500);
	s.push(600);
	s.push(700);
		
	cout << "s:" << s.pop() << endl;
	cout << "size:" << s.getSize() << endl;
	return 0;
}