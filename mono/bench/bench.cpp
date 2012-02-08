#include <iostream>


using namespace std;


class V
{
	public:
	int value;
	bool is_a_prime;
};



// eliminate 
int main( int argc, char **argv )
{ 
	int N = atoi(argv[1]);
	int i;
	V vals[N];
	
	// init
	for(i=0;i<N;i++){
		vals[i].value = i;
		vals[i].is_a_prime = true;
	}
	// check
	for(i=2;i<N;i++){
		for(int j=2;j<N;j++){
			if( i == j ){
				continue;
			}
			if((vals[j].value % i) == 0 ){
				vals[j].is_a_prime = false;
			}
		}
	}
	// count
	int cnt = 0;
	for(i=2;i<N;i++){
		if( vals[i].is_a_prime ){
			cnt++;
			cout << vals[i].value << " ";
		}
	}
	cout << "done. cnt=" << cnt << endl;
	return 0;
}
