#include <stdio.h>


struct CHAR_base
{
    int use;
    int aaa;
};

struct CHAR
{
    struct CHAR_base dummy;
    int hp , mp;
};

struct NPC
{
    struct CHAR_base dummy;    
    int fuck, shit;
};
struct PET
{
    struct CHAR_base dummy;        
    int asdalsdjkasdfasda;
};


struct fuck
{
    int a;
    
};


int main()
{
    struct fuck f[100];
    struct fuck *fp;

    fp = &f[i];

    f[i].a = 10;

    fp->a = 10;
    fp->b = 102;
}

{
    struct CHAR *c;
    struct NPC *n;

    if( ( c = CHAR_GETPOINTER( index ) ) == NULL ){ return;}

    c->use = 0;
    c->hp = 10;
    c->mp = 30;
    
}
