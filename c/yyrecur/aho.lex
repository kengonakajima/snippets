%{
    
/*
  $B:F5"E*$J9=B$$r$b$D%G!<%?7?$N<B83(B
 */
#include <string.h>
#include <stdlib.h>

#include "aho.h"    
#include "aho.tab.h"

#define yywrap() 1

    
int blaces_depth = 0;

 
    
%}

%s STRUCT_STATE

%%


Struct {
    BEGIN STRUCT_STATE;
	return STRUCT;
}

[ \t]  { ; }
\n     { ; }
\r { ; }

<STRUCT_STATE>[0-9]+ { yylval.ival = atoi( yytext ); return INTEGER; }
<STRUCT_STATE>"{" { blaces_depth ++; return '{'; }
<STRUCT_STATE>"}" { blaces_depth --; return '}'; }
<STRUCT_STATE>Member { return MEMBER; }
<STRUCT_STATE>. { return yytext[0]; }
. {;}


%%
