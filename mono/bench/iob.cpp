#include <iostream>
#include <string>
using namespace std;


int numofbits(long bits) {
  bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
  bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
  bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
  bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
  return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
}

string convert(int n)
{
	static const string st[] = { "C", "G", "A", "T" };
	return st[ n % 4];
}
int main( int argc, char **argv )
{
	int N = atoi(argv[1]);
	for(int i=0;i<N;i++){
		int num = numofbits(i);
		cout << convert(num);
	}
	return 0;
}