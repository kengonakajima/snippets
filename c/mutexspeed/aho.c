#include <stdio.h>

#include <pthread.h>

#define NUM 1000000

pthread_mutex_t mt[NUM];


void init(void)
{
    int i;
    for(i=0;i<NUM;i++){
        pthread_mutex_init( &mt[i] , NULL );
    }
}
void lockAll(void)
{
    int i;
    for(i=0;i<NUM;i++){
        pthread_mutex_lock( &mt[i] );
    }
}


void unlockAll(void)
{
    int i;
    for(i=0;i<NUM;i++){
        pthread_mutex_unlock( &mt[i] );
    }
}


int main(int argc, char **argv )
{
    int i;
    
    init();


#if 1
    for(i=0;i<100;i++){
        lockAll();
        unlockAll();
        fprintf( stderr , "#" );
    }
#endif

#if 0
    for(i=0;i<100;i++){
        int j;
        for(j=0;j<NUM;j++){
            pthread_mutex_lock( & mt[i] );
            pthread_mutex_unlock( & mt[i] );
        }
        fprintf( stderr , "." );
    }
#endif    
}
