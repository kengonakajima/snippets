%{
    #include <stdlib.h>
    #include <stdio.h>
    

    typedef enum {
        OPNOOP = 0,
        OPWALK = 1,
        OPTALK = 2,
        OPSQRT = 3,
        OPGOTO = 4
    } OPTYPE;

    struct operation {
        OPTYPE op;
        int whati;
        float whatf;
        char what[100]; /* walk:0~3 talk:text */
    };
    struct method {
        int onum;
        struct operation oplist[100];
    };

    struct program {
        char name[100];
        struct method m;
    };

#define MAX_PROGS 10
    struct program programs[MAX_PROGS];
    struct program *current_reading_prog;

%}

%token WALK TALK INT STRING PROGNAME GOTO SET



%%

program : progname method { printf( "OK!\n"); }
|
;

progname : PROGNAME STRING ';' {
    current_reading_prog->name[0]= $2;
    current_reading_prog->name[1]= 0;
}
method: operation_list
;

operation_list: operation_list operation {}
| operation {} 
;

operation: WALK INT ';' {
    printf( "match walk\n");
    current_reading_prog->m.oplist[current_reading_prog->m.onum].op = OPWALK;
    current_reading_prog->m.oplist[current_reading_prog->m.onum].what[0] = $2;
    current_reading_prog->m.onum++;
    
}
| TALK STRING ';' {
    printf( "match talk\n");
    current_reading_prog->m.oplist[current_reading_prog->m.onum].op = OPTALK;
    current_reading_prog->m.oplist[current_reading_prog->m.onum].what[0] = $2;
    current_reading_prog->m.onum++;
}
| GOTO INT ';' {
    current_reading_prog->m.oplist[current_reading_prog->m.onum].op = OPGOTO;
    current_reading_prog->m.oplist[current_reading_prog->m.onum].whati = $2;
    current_reading_prog->m.onum++;    
}
;

%%

void exec_method( struct method *m );
void exec_program( char *progname )
{
    int i;
    for(i=0;i<MAX_PROGS;i++){
        if( strncmp(programs[i].name, progname,1)==0){
            printf( "executing program '%s'\n", programs[i].name );
            exec_method(  &programs[i].m );
        }
    }
}
    

void exec_method( struct method *m )
{
    int i;
    for(i=0;i<m->onum;i++){
        switch( m->oplist[i].op ){
        case OPNOOP:
            ;
        case OPWALK:
            printf( "Walk to %d\n", m->oplist[i].what[0] );
            break;
        case OPTALK:
            printf( "Talk to '%c'\n", m->oplist[i].what[0] );
            break;
        case OPGOTO:
            i = m->oplist[i].whati;
            i--;
            break;
        default:
            // error
            break;
        }
    }
}
int main()
{
    int i;
    
    current_reading_prog = &programs[0];    
    yyparse();

    exec_program( "a" );
    
}
