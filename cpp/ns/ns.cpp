#include <stdio.h>


namespace aho
{
    int hoge()
    {
        printf("akdfhlakjsdf\n" );
    }
    class SV
    {
    public:
        int pag()
        {
            printf("pag\n");
        }
    };
}

#if 0 // とおらない
void aho::fuga()
{
        printf("akdfhlakjs-0000df\n" );    
}
#endif

int main()
{

    aho::SV *sv = new aho::SV();
    sv->pag();
    aho::hoge();
}
