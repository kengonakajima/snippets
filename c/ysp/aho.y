%{

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    struct ppp
    {
        int cx,cy;
        int td[32][32];
    };

    struct ppp ahop;
    
%}


%union {
    int ival;
    char sval[16384];
}


%token TILE 
%token <ival> INT
%token <sval> STRING

%%

all: content_members {;}
;

content_members: content_members content_member {;}
| content_member {;}
;
content_member:  tiledata {;}
;


tiledata:  TILE '=' '{' tline_members '}' ';' {;}
| TILE '=' '{' tline_members ',' '}' ';' {;}
;



tline_members: tline_members ',' tline_member {;}
| tline_member {;}
;

tline_member: '{' tiles '}' {
    ahop.cy ++;
}
;

tiles: tiles ',' INT {
    ahop.td[ahop.cy][ahop.cx] = $3;
    ahop.cx ++;
}
| INT {
    ahop.td[ahop.cy][0] = $1;
    ahop.cx = 1;
}
;



%%

int main()
{
    yyparse();

}
