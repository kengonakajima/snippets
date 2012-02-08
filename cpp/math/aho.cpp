#include <stdio.h>

bool calc( int d[9] ){
        unsigned int upper = (d[8] * 10000) + (d[7] * 1000) + (d[6] * 100) + (d[5] * 10) + d[4];
        unsigned int lower = (d[3] * 1000) + (d[2] * 100) + (d[1] * 10) + d[0];
        fprintf( stderr, "%u + %u = %u\n",  upper, lower, (upper+lower));
        if( (upper + lower ) == 33333 ){
               fprintf( stderr, "ANS: %u + %u = %u\n",
                        upper, lower, (upper+lower));
               return true;                 
        } else {
//        fprintf(stderr,"%d %d %d %d %d + %d %d %d %d = %u\n",
//                d[8],d[7],d[6],d[5],d[4],d[3],d[2],d[1],d[0], (upper+lower) );
                return false;
        }
}
bool again( int d[9])
{
        int i, k[10];
        for(i=0;i<10;i++){
                k[i]=0;
        }
        
        for(i=0;i<9;i++){
                k[d[i]]++;
                if(k[d[i]] >= 2){
//                        fprintf(stderr,"A");
                        return true;
                }
        }
        if(k[0]>0){
                return true;
        }
//        fprintf(stderr,"%d %d %d %d %d %d %d %d %d\n",
//                d[8],d[7],d[6],d[5],d[4],d[3],d[2],d[1],d[0] );
//        fprintf(stderr, "F");        
        return false;
}

        
int main()
{
        unsigned int i=0;
        for(i=100000000;i<=399999999;i++){
                int d[9];

                if((i%1000000)==0){
                        printf("%u\n", i );
                }
                
                d[0] = i%10;
                d[1] = (i/10)%10;
                d[2] = (i/100)%10;
                d[3] = (i/1000)%10;
                
                d[4] = (i/10000)%10;                
                d[5] = (i/100000)%10;
                d[6] = (i/1000000)%10;
                d[7] = (i/10000000)%10;
                d[8] = (i/100000000)%10;

                if( again(d) == true ){
                        continue;
                }
                if( calc(d) == true ){
                        fprintf(stderr,"%d %d %d %d %d    %d %d %d %d\n",
                                d[8],d[7],d[6],d[5],d[4],d[3],d[2],d[1],d[0] );
                }
        }
}
