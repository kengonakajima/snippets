int main()
{
    extern char **environ;
    char *p;

    while(p=*environ++){
        printf( "ENV: '%s'\n", p );
    }
}
