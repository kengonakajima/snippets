#include <stdio.h>
#include <pthread.h>

pthread_mutex_t m1;
int a = 100;


pthread_t t1, t2;
int A()
{
    fprintf( stderr , "A start\n" );
    pthread_mutex_lock( &m1);
    a = 2000;
    sleep(4);
    pthread_mutex_unlock(&m1);
    fprintf( stderr , "A end\n" );    
}

int B()
{
    fprintf( stderr , "B start\n" );    
    pthread_mutex_lock(&m1);
    fprintf( stderr , "%d\n", a );
    pthread_mutex_unlock(&m1);
    fprintf( stderr , "B end\n" );        
}

int main()
{
    pthread_mutex_init( &m1, NULL );
    
    pthread_create( &t1, NULL, A , NULL);
    pthread_create( &t2, NULL, B , NULL);

    pthread_join( t1 , NULL );
    pthread_join( t2 , NULL );    
}

