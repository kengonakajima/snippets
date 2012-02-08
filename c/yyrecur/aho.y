%{
#include "aho.h"
    s_node *alloc_s_node( void );
    void free_s_node( s_node*s);
    void yyerror( char *m );
%}

%union {
    int ival;
    s_node * snval;
}

%token <ival> INTEGER
%token STRUCT MEMBER


%type <snval> i_member struct struct_members 


%%
struct: STRUCT '{' struct_members '}' { fprintf(stderr,"Struct OK\n"); }
| STRUCT '{' '}' { fprintf(stderr,"Struct OK with no member\n"); }
;

struct_members: struct_members i_member {;}
| struct_members struct {;}
| struct {
    s_node *sn = alloc_s_node();
    if( !sn ){ yyerror( "alloc fail\n"); return 0; }
    sn->member_num = 
    ;
}
| i_member {
    /* 初のmember */
    s_node *sn = alloc_s_node();
    if( !sn){ yyerror( "alloc fail\n"); return 0; }
    sn->member_num = 1;
    sn->member[0] = $1->member[0];
    $$ = sn;
}
;


i_member: MEMBER INTEGER ';' {
    s_node *sn = alloc_s_node();
    if( !sn ){       yyerror( "alloc fail\n");        return 0;    }
    sn->member_num = 1;
    sn->member[0] = $2;
    $$ = sn;
}
;



%%

s_node *top_node = NULL;

int
main( int argc, char**argv )
{
    yyparse();
}
  

void yyerror( char *m )
{
    fprintf(stderr, "YYERROR %s\n", m );
}
s_node *alloc_s_node( void )
{
    s_node *s;
    s = (s_node*) malloc( sizeof( s_node ));
    if( !s ) return s;
    memset( s, 0,sizeof(s_node));
    return s;
}
void free_s_node( s_node*s)
{
    free(s);
}
