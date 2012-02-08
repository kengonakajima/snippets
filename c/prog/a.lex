%{
#include <stdio.h>
	
#define yywrap() 1

main()
{
	
	while(1)printf("%d\n"  ,yylex() );
}

%}





%%

aho	{ return 1; }
[0-9]+	{ return atoi(yytext); }
.	{ return yytext[0] ; }

%%
