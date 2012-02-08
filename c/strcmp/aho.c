
int strcmp_nocase( char *s1, char *s2 )
{
    int i;
    for(i=0;;i++){
        int c1 = tolower(s1[i]);
        int c2 = tolower(s2[i]);

        if( s1[i] == s2[i] && s1[i] == '\0' ) return 0;
        if( c1 > c2 ){
            return 1;
        } else if( c1 < c2 ) {
            return -1;
        }
    }
    return 0;            
}

int main()
{

    char aho1[] = "fuck";
    char aho2[] = "fuCk";

    int i;

    i = strcmp( aho1, aho2 );

    printf( "%d\n",i );
    i = strcmp_nocase( aho1, aho2 );

    printf( "%d\n",i );    
}
