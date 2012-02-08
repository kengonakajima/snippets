#include <stdio.h>

main()
{
    char b[1000];
FILE*fp=fopen("aaaa" , "r" );
FILE*fwp=fopen("whrrrrdvdd" , "w");
    int i;

    while( fgets(b,sizeof(b) , fp )){
        fputs(b,fwp);
    }
    
    fclose(fwp);
    fclose(fp);
    
}
