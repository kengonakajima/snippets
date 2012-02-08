void cutDotsTail( char *s )
{
    int l = strlen( s );
    int i;

    for(i=l-1;i>=0;i--){
        if( s[i] == '.' ){
            s[i] = 0;
        } else {
            break;
        }
    }
        
}

int main()
{
    char s[100];

    strcpy( s , "hoge...." );

    cutDotsTail(s);


    printf( "S:%s\n" , s );

}
