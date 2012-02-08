#include <iostream>
using namespace std;

class Aho
{
 public:
        Aho( int i ){
        }

        ~Aho(){
                cerr << "Destr" << endl;
        }
                
};

int main()
{
        Aho a(1);

        
}
