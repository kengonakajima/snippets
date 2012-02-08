#include <queue>
using namespace std;

int main(){
        queue<int> q;
        q.push(10);
        q.push(20);
                q.push(30);
                q.push(40);                
        q.pop();

        printf("%d\n", q.front() );
        printf("%d\n", q.size());
        queue<int>::iterator qi;
        qi = q.begin();
}
