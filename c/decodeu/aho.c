#include <stdio.h>

int urlPostTokenize( char **out , char*in )
{
    int c;
    int count = 0;
    int hatena;



    out[0] = in;

    while( (c = *in ) != '\0'){
        if( c == '&' ){

            *in = '\0';
            out[++count] = (char *)(in + 1);
        }


        in++;
    }
    out[++count] = NULL;

    return count;
}

static int decodeURL( char *out , char *in)
{
    int c;
    while( (c = *(in++ )) != '\0' ){
        if( ( c >= 'a' && c <= 'z' ) ||
           ( c >= 'A' && c <= 'Z' ) ||
           ( c >= '0' && c <= '9' ) ||
           c == '@' || c == '.' || c ==' ' || c =='=' || c == '/'
           ){
            *out++ = c;
            *out = '\0';
        } else if( c == '+' ){
            *out++ = ' ';
            *out = '\0';
        } else if( c == '%' ){
            int c1 , c2;
            c1 = tolower(*(in++));
            c2 = tolower(*(in++));
            if( ( (c1 >= 'a' && c1 <= 'f') || ( c1 >= '0' && c1 <= '9' )) &&
               ( (c2 >= 'a' && c2 <= 'f') || ( c2 >= '0' && c2 <= '9' ))
               ){
                char buf[3];
                long l;

                buf[0] = c1;
                buf[1] = c2;
                buf[2] = 0;

                l = strtol( buf , NULL , 16 );
                if( l == 0 ) return -1;
                *out++ = (char)l;
                *out  = '\0';
            }

        } else {
            return -1;
        }
    }
    return 0;
}
int countChar( char *buf , int c ,int n)
{
    int i;
    int count = 0;
    for(i = 0 ; ;i++){
        if( buf[i] == '\0' ) break;
        if( buf[i] == c ){
            count++;
            if( count == n ) return i;
        }
    }

    return -1;
}




int main()
{
//    char hoge[] = "%65%67";
    char hoge[] = "asdf9a0asd=%33d8d&aa=83494&bb=asd8";
    char fuck[100];
    char s[1111];
    char *aho[100];
    
    strcpy( s , "asdf=hoge&aaaa=98dad");
    urlPostTokenize( aho , s );
    decodeURL( fuck, aho[0] );
    printf( "[%s][%s]\n", fuck ,aho[0]);
    
    decodeURL( fuck, aho[1] );
    printf( "[%s][%s]\n", fuck,aho[1] );    
    
}
