#include <stdio.h>

class aho
{
        int a;
        public:
        aho(int x){ a = x; }

        void p(){ printf( "%d\n", a ); }
};

int main()
{
        aho *a = new aho(10);

        a->p();
        return 0;
}

 
