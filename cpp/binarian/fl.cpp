#include <iostream>
int main()
{
	std::cout << std::hex;
	std::cout << -1.75 << std::endl;
	float f = -1.75;
	int *i = (int*)&f;
	std::cout << *i << std::endl;	
}