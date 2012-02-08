%{

    #include <string.h>
    #include <stdlib.h>

    #include "aho.tab.h"



%}



%%


tile { return TILE; }


\/\*.*\*\/ {;}
-?[0-9]+   {
    return INT;
}

[ \t]  { ; }
\n     { ; }
\r { ; }

"{" { return '{'; }
"}" { return '}'; }
, { return ','; }
; { return ';'; }
= { return '='; }
"(" { return '('; }
")" { return ')'; }



. {
    return 0;
}

%%

int yywrap()
{
    return 1;
}
void yyerror()
{
    fprintf( stderr, "aho\n" );
}
    
