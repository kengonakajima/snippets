#include <pthread.h>



int count = 0;


pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t count_hit_threshold;

void *inc_count( void *idp )
{

    while(1){
        pthread_mutex_lock( &count_lock );
        count++;
        printf( "inc_count: %d\n", count );
        if(count == 4 ){
            pthread_cond_broadcast( &count_hit_threshold );
        }
        if( count == 10 ){
            pthread_mutex_unlock( &count_lock );
            break;
        } else {
            pthread_mutex_unlock( &count_lock );
        }
    }
}

void *watch_count( void *idp )
{
    pthread_mutex_lock( &count_lock );        
    while( count < 4 ){
        gprintf( "fucking wait\n");
        pthread_cond_wait( &count_hit_threshold, &count_lock );
    }
    printf( "watch_count: %d\n", count );
    pthread_mutex_unlock( &count_lock );

    return NULL;
}

main()
{
    int i;
    pthread_t threads[3];

    pthread_cond_init( &count_hit_threshold , NULL );
    pthread_create( &threads[0] , NULL , inc_count, (void*)NULL );
    pthread_create( &threads[1] , NULL , watch_count, (void*)NULL );

    pthread_join( threads[0] , NULL );
    pthread_join( threads[1] , NULL );    
}

