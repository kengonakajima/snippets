%{

#include <stdio.h>
#include "aho.tab.h"
    
    
int yywrap(void)
{
    return 1;
}

int yyerror(void)
{
    printf( "SYNTAX ERROR!!!!\n");
    return 0;
}
    
%}    


%%

[0-9]+ { yylval = atoi(yytext); return INT; }


walk { return WALK; }
talk { return TALK; }
progname { return PROGNAME; }
goto { return GOTO; }


set { return SET; }


[a-z]+ { yylval = yytext[0]; return STRING; }


; { return ';'; }

\n {;}
\r {;} 

. {;}

%%


