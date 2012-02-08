
char * hoge()
{ 
static char aho[100] = "ahoahoahaoh";   
    return aho;
}

int main()
{
    char *p;
    p = hoge();
    printf( "%s\n",p );
}
