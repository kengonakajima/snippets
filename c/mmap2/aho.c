#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <stdio.h>

int main()
{
    char *v;
    int fd = open( "hoge.txt" , O_RDONLY );
    
    if(fd < 0 ){
        perror("fuck");
        return 1;
    }

    v = mmap( 0 , 1000000 , PROT_READ , MAP_PRIVATE, fd,0 );

    fprintf(stderr , "%x\n", (int)v);

    {
        int i;
        for(i=0;i<1000000;i++){    
            printf("%d " , v[i]);
        }
    }
    close(fd);
        
}

        
