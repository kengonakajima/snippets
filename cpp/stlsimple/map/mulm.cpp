#include <map>
#include <string>
#include <iostream>

using namespace std;

int main()
{
      multimap<string,int> ms;
      
      printf("%d\n", sizeof(ms));
    /*
      multimap<string,int> ms;

        ms.insert(pair<string,int>(string("a"), 1111));
        ms.insert(pair<string,int>(string("a"),222));
        ms.insert(pair<string,int>(string("b"),4949));
        ms.insert(pair<string,int>(string("c"),3333));
        cout << ms.size() <<endl;
        
        ms.erase("b");

        multimap<string,int>::iterator i = ms.find("b");
        pair<multimap<string,int>::iterator, multimap<string,int>::ite
        cout << (*i).first << endl;
        cout << ms.size() << endl;
    */
    
}
