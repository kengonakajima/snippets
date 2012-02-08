
struct item
{
    int used;
    char data[800];
    struct item *next;
};

struct chardata1
{
    char fuck_data[1024 * 400 ];
    struct item ia[100];
};

struct chardata2
{
    char fuck_data[1024*400];
    struct item *ia_top;
};

#define CHARNUM 5000

#define TEST_NUM 10000

#define AVG_ITEM_N 10

int main(int argc, char **argv)
{
    int i;
    int testtype = atoi(argv[1]) ;

    if( testtype == 1 ){
        int j;
        int sz =sizeof(struct chardata1) * CHARNUM;
        struct chardata1 * ch1a = malloc( sz );
        memset( ch1a, 0, sz );
        
        for(j=0;j<CHARNUM;j++){
            int k;
            int n = random() % AVG_ITEM_N;
            for(k = 0; k < n; k++ ){
                ch1a[k].used = 1;
            }
        }
        

    }
