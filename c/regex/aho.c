#include <sys/types.h>
#include <regex.h>


int main()
{
    regex_t p;
    regmatch_t aho[10];
    char fuck[1000];
    
    int r;

    memset( &p, 0 ,sizeof(p));
    r = regcomp(  &p, "123", REG_EXTENDED );

    printf( "REGCOMP:%d\n", r);

    strcpy( fuck ,  "09u8123jkk23423423498234239123" );

    r = regexec( &p, fuck ,  10 , aho, 0 );
    printf( "REGEXEC: %d %d\n", r , REG_NOMATCH );
    printf( "0: %d %d\n", aho[0].rm_so , aho[0].rm_eo );
    printf( "1: %d %d\n", aho[1].rm_so , aho[1].rm_eo );
    printf( "2: %d %d\n", aho[2].rm_so , aho[2].rm_eo );            
}
