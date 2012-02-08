#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>


int main()
{
    char *p;

    while(1){
        p = readline( "aho>" );
        printf ("%s\n" , p );
    }
    
}
