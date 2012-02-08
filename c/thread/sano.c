#include <stdio.h>

#include <pthread.h>
#include <sched.h>

void *sano_funk( void *a )
{
    while(1){
        sleep(4);
        printf( "sano desu\n" );
    }
}
void ringo_funk( void *a )
{
    while(1){
        sleep(1);
        printf( "ringo deseu\n" );
    }
}

int main( int argc, char **argv )
{
    pthread_attr_t newthread_attr;
    pthread_t newthread_id;
    
    pthread_attr_init( &newthread_attr );

    pthread_create( &newthread_id , &newthread_attr , sano_funk, NULL);
    pthread_create( &newthread_id , &newthread_attr , ringo_funk , NULL);

    sleep(100000);
}
