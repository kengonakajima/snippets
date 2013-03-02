#include <stdio.h>
#include <assert.h>
#include <strings.h>

typedef struct {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;            
} cell_t;

#define W 2048
#define H 2048
#define D 8

int main( int argc, char **argv ) {

    int opt = atoi(argv[1]);

    long long n = W * H * D;
    
    size_t  sz = sizeof(cell_t) * n;
    printf("sz:%lld", (long long int) sz /1024/1024 );

    cell_t *cells = (cell_t*)malloc( sz );
    if(!cells) {
        fprintf(stderr, "cant allocate\n");
        return 1;
    }


    memset( cells, 0, sz ); 
    
    switch(opt){
    case 0:
        {
            int chunksz = 1024;
            int chunkn = n / chunksz;
            
            for(int i=0;i<chunkn;i++){
                for(int k=0;k<100;k++){
                    for(int j=0;j<chunksz;j++){
                        int ind = j + i * chunksz;
                        cells[ind].a ++;
                        cells[ind].b ++;
                        cells[ind].c ++;
                        cells[ind].d ++;
                    }
                }
            }
        }
        break;
    case 1:
        for(int j=0;j<100;j++) {
            fprintf(stderr,".");
            for(long long i = 0; i<n;i++) {
                cells[i].a ++;
                cells[i].b ++;
                cells[i].c ++;
                cells[i].d ++;
            }
        }
        break;
    }


    return 0;
}
