#include <memory>

using namespace std;



int main()
{
        void *p = malloc(sizeof(int));
        int *aho = (int*)p;
        aho[0] = 10;
        auto_ptr<int> x( aho );
        auto_ptr<int> y;
        y = x;

        printf("%d\n", *x.get());
}
