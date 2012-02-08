void pf( void *ptr);

#include <stdio.h>
#include <pthread.h>
#include <linux/smp.h>


pthread_t th1 , th2;
int counter =0;
int loopcount = 1000;
pthread_mutex_t counter_m;
int t1_ret =0, t2_ret=0;
main()
{

    

    char *message1 = "hello";
    char *message2 = "world";

    pthread_attr_t pthas1,pthas2;

    {
        printf( "CPu:%d\n ",  smp_num_cpus );
    }
    
    pthread_mutex_init( &counter_m , NULL );
    
    pthread_attr_init( &pthas1 );
    pthread_attr_init( &pthas2 );
    
    pthread_create( &th1 , &pthas1,
                    (void*) &pf , (void*)message1);
    pthread_create( &th2 , &pthas2,
                    (void*) &pf , (void*)message2);

    

    pthread_join( th1 , NULL );
    pthread_join( th2 , NULL );
    
    printf( "counter:%d t1:%d t2:%d\n" , counter ,t1_ret , t2_ret );


    
    exit(0);
}

void pf( void *p )
{
    char *message;
    int i;
    
    message = (char*) p;
    
    for(i=0;i<loopcount;i++){
//        printf( "%s\n" , message );
        pthread_mutex_lock( &counter_m );
        counter++;
        printf( "%s %d\n" , message , i  );
        usleep(500000);
        pthread_mutex_unlock( &counter_m );
        usleep( 190 );
    }
    if( pthread_equal( th1 , pthread_self() ) ){
        t1_ret = i;
        printf( "t1_ret: %d\n" , t1_ret );
    }

    if( pthread_equal( th2 , pthread_self() ) ){
        t2_ret = i;
        printf( "t2_ret: %d\n" , t2_ret );
    }

}

