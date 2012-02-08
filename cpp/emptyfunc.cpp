#include <stdio.h>



#define AHOFUNC_BODY
#define HOGEFUNC_BODY
#define FUGAFUNC_BODY

void ahofunc();
void hogefunc();
void fugafunc();

////////////以上が gen側

#undef AHOFUNC_BODY 
#define AHOFUNC_BODY {}
#undef HOGEFUNC_BODY 
#define HOGEFUNC_BODY {}


#define FILL_EMPTY_FUNCS \
    void ahofunc() AHOFUNC_BODY;      \
    void hogefunc() HOGEFUNC_BODY; \
    void fugafunc() FUGAFUNC_BODY;






int main()
{
    ahofunc();
    hogefunc();
    fugafunc();
    
}

FILL_EMPTY_FUNCS
