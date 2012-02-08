%{

#include <stdio.h>

#define yywrap() 1

    char hoge[10000];

int main ( int argc , char * * argv )
{
	
	yylex();
}

/*
\"((\\r)|(\\\')|(\\0)|(\\\\)|(\\n)|(\\\"))|[a-zA-Z0-9\" {
    printf( "HOGE!! '%s'\n", yytext );
}
*/
%}	

%s LITERAL

%%



<LITERAL>\" {
    printf( "STRING!! '%s'\n", hoge );
    BEGIN 0;
}

<LITERAL>. {
    strcat( hoge, yytext );
}

<LITERAL>\\\\ {
    strcat( hoge, "\\\\");    
}
<LITERAL>\\\" {
    strcat( hoge, "\\\"");
}

\" {
    BEGIN LITERAL;
    hoge[0] = '\0';
}

\n { BEGIN 0;}
.	{ printf("OTHER char!! '%c'\n" , yytext[0]);; }







%%




