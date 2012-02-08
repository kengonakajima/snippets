#include <stdio.h>
#include <pthread.h>

pthread_mutex_t m1;
int a = 100;


pthread_t t1, t2;
int A()
{
    while(1){
        pthread_mutex_lock( &m1);
        fprintf(stderr,".");
        pthread_mutex_unlock(&m1);
    }
}
int B()
{
    while(1){
        pthread_mutex_lock( &m1);
        fprintf(stderr,"#");
        pthread_mutex_unlock(&m1);
    }
}
int main()
{
    pthread_mutex_init( &m1, NULL );
    
    pthread_create( &t1, NULL, A , NULL);
    pthread_create( &t2, NULL, B , NULL);    

    pthread_join( t1 , NULL );
    pthread_join( t2 , NULL );
}

