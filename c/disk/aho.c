
#include <stdio.h>

/* disk access test */  

int main()
{
        int i;
        FILE *fp;

        for(i=0;i< 1000; i++ ){
                char aho[100];
                snprintf(aho,sizeof(aho), "fuck/%d", i );
                fp = fopen( aho, "w" );
                if(fp){
                        fprintf( fp, "aksdjhakdfhkajdhfkjadkjasdkfad\n" );
                        fclose(fp);
                }
                
        }

}

