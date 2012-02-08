/*
  ばーちゃるアシン
  
 */

#include <stdio.h>

#define ARRAYOF(x) (sizeof(x)/sizeof(x[0]))


void ringofunc(char *a )
{
    printf( "ringofunc says: %s\n" , a );
}



void takagifunc(char *a )
{
    printf( "takagifunc says: answer is : %s\n" , a );
}



void fuckfunc(char *a )
{
    printf( "fuckfunc: %s\n" , a );
}

void thaifunc( char *a)
{
    printf("thai says: %s\n", a );
}


void addnazofunc( char *a );

struct hoge
{
    char funcname[100];
    void (*funcpointer )( char *a  );
};
struct hoge h[7] =
{
    { "ringo" , ringofunc },
    { "takagi" , takagifunc },
    { "fuck" , fuckfunc },
    { "thai" , thaifunc },
    { "addnazo" , addnazofunc },
    { "nazo" , nazofunc },
    { "" , NULL }
};


void nazofunc( char *a )
{
    printf( "nazofunc says: %s\n",  a );
}


void addnazofunc( char *a )
{
    /* add nazofunc in runtime */

    int i;
    printf("asdfasdfas");
    for(i=0;i<ARRAYOF(h);i++){
        if( h[i].funcname[0] == 0 ){
            strcpy( h[i].funcname , a );
            h[i].funcpointer = nazofunc;
            printf("added %s\n", a );
            break;
        }
    }
}


void call_a_func( char *name )
{
    int i;
    for(i=0;i<ARRAYOF(h);i++){
        if( strcmp( h[i].funcname , name ) == 0 ){
            h[i].funcpointer( name );
            break;
        }
    }
}

int main()
{


    while(1){
        char line[100];
        char param[100];
        fgets(line,sizeof(line),stdin );
        fgets(param,sizeof(param),stdin);
        
        /* kill \n in the end of the line */
        line[strlen(line)-1]=0;
        param[strlen(param)-1]=0;
        
        if( strcmp( line , "ringo" ) == 0 ){
            ringofunc( param );
        }
        if(strcmp( line , "takagi" ) == 0 ){
            takagifunc( param );
        }
        if(strcmp( line , "fuck" ) == 0 ){
            fuckfunc( param );
        }
        if(strcmp( line , "thai" ) == 0 ){
            thaifunc( param );
        }
    }

    
}
