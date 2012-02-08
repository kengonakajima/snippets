#include "boost/pool/pool.hpp"
#include "boost/pool/object_pool.hpp"
#include <iostream>
#include "vce.h"





struct X
{
    int x,y,z;
};
struct Y
{
    char aho[100];
};

#define N 100000

struct X * xptr[N];
struct Y * yptr[N];



int main(int argc,char **argv )
{
    vce_initialize();

    int mode = atoi(argv[1]);

    switch(mode){
    case 0:
        {
            boost::object_pool<X> px;
            boost::object_pool<Y> py;
            std::cerr << "alloc" << std::endl;
            for(int i=0;i<N;i++){
                xptr[i] = px.construct();
                yptr[i] = py.construct();
            }
            std::cerr << "free" << std::endl;            
            for(int i=0;i<N;i++){
                if( ( i % 1000 ) == 0 ){
                    std::cerr << "." ;
                }
                px.free(xptr[i]);
                py.free(yptr[i]);
                
            }
            break;
        }
    case 1:
        {
            std::cerr << "alloc" << std::endl;                        
            for(int i=0;i<N;i++){
                xptr[i] = new X();
                yptr[i] = new Y();
            }
            std::cerr << "free" << std::endl;                        
            for(int i=0;i<N;i++){
                free(xptr[i]);
                free(yptr[i]);
            }
            break;
        }
    case 2:
        {
            int xai = vce_init_array( sizeof(struct X), N, "xary" );
            int yai = vce_init_array( sizeof(struct Y), N, "yary" );

            std::cerr << "alloc" << std::endl;                                    
            for( int i=0;i<N;i++){
                xptr[i] = (X*)vce_alloc_array_object( xai );
                yptr[i] = (Y*)vce_alloc_array_object( yai );
            }
            std::cerr << "free" << std::endl;                                    
            for( int i=0;i<N;i++){
                vce_free_array_object( xai, (void*)xptr[i] );
                vce_free_array_object( yai, (void*)yptr[i] );
            }
            break;
        }
    default:
        break;
    }
    return 0;    
}


