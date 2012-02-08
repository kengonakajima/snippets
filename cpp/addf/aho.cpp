#include <string>
using namespace std;

class basic_string
{
 public:
	int to_i()
	{
        return atoi( this->c_str());
	}

};

int main()
{
        string s = "123";
        cerr << "asdf:" << s.to_i << endl;
}
