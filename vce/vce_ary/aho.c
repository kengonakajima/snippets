#include "vce.h"
#include <stdio.h>

struct hoge
{
    int id;
    char name[100];
    int fuck;
};

int n = 10000;

int total_allocated = 0;
int current_allocated = 0;
int freed = 0;


void do_test(int mi)
{
    if( current_allocated < n ){
        if( ( random() % 10 ) == 0 ){
            struct hoge *p = vce_alloc_array_object( mi );
            if(!p){
                fprintf(stderr, "fatal: cannot allocate\n" );
                exit(1);
            } else {
                current_allocated ++;
            }
        } 
    } else if( current_allocated > 0 ){
        if( ( random() % 10 ) == 0 ){
            struct hoge *p;
            int rn = random() % n;
            p = vce_get_array_object_pointer_by_index( mi, rn );
            if(!p){
                fprintf(stderr, "fatal: cannot get by index (%d)\n",  rn );
                exit(1);
            } else {
                vce_free_array_object( mi, p );
                current_allocated --;
            }
        }
        if( ( random() % 100 ) == 0 ){
            struct hoge *cur ;
            fprintf(stderr,".");
            ARRAY_SCAN( mi, cur ){
                if( (random() % n) == (n/10)){
                    vce_free_array_object( mi, cur );
                    current_allocated --;
                }
            }
        }
    }
    if( vce_get_array_usenum(mi) != current_allocated ){
        fprintf( stderr, "fatal: differ (%d,%d)\n",
                 vce_get_array_usenum(mi), current_allocated );
    } else {
        static int c = 0;
        c++;
        if( ( c % 100000 ) == 0 ){
            fprintf( stderr , "%d\n", vce_get_array_usenum(mi) );
        }
    }
}
             
int main()
{

    int mi,i;

    vce_initialize();
    
    mi = vce_init_array( sizeof(struct hoge), n, "alkdjhaskjdf" );
    if(mi<0) return 1;

    for(i=0;i<100*100*100*100;i++){
        do_test(mi);
    }
    
    return 0;
}
