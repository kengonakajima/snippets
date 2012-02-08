%{

#define yywrap() 1


%}

%%

bgcolor=#ffffff		{ printf("bgcolor=#000000 text=#ffffff");}

%%

main()
{

	yylex();
}
