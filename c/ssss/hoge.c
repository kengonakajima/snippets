#include <stdio.h>

char*token_list[100];

void lsacproto_splitString( char *src  )
{
    int i,c=0;
    for(i = 0 ; ; i++ ){
        if( src[i] == '\0' ) break;
        if(  i == 0 ){
            token_list[c++] = &( src[i] );
        }
        if( src[i] == ' ' ){
            token_list[c++] = &( src[i+1] );
        }
#if 0        
        if( src[i] != ' ' ){
            if( i > 0 && src[i] == ' ' && src[i-1] == ' ' ){
                token_list[c++] = &( src[i] );
            }
            if( i == 0 || src[i-1] == ' ' ){
                token_list[c++] = & ( src[i] );
            }
        } else {
            
        }
#endif
        
    }
    while( *src ){
        if( ( *src  ) == ' ' || (*src ) == '\n' ||
            (*src ) == '\r' ) *src = '\0';  
        src++;
    }
    token_list[c] = (char*)NULL;
}

int main()
{

 char hoge[1024];

sprintf( hoge , "1 AHO HOGE  fuck" );

lsacproto_splitString( hoge );

printf( "TL0:%x <%s>\n" , token_list[0] ,token_list[0] );
printf( "TL1:%x <%s>\n" , token_list[1] ,token_list[1]);
printf( "TL2:%x <%s>\n" , token_list[2] ,token_list[2]);
printf( "TL3:%x <%s>\n" , token_list[3] ,token_list[3]);
printf( "TL4:%x <%s>\n" , token_list[4] ,token_list[4]);
printf( "TL5:%x <%s>\n" , token_list[5] ,token_list[5]);
}
