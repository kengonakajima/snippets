#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>

int
main( int argc, char **argv )
{

    char *p;



    while(1){
        p = readline( "aho>" );
        add_history( p );
    }
}
