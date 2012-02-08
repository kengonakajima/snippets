#include <pthread.h>
#include <stdio.h>
#include <readline/readline.h>


/*
  #ifdef によってシングルスレッドバージョンになり、pthreadがいらなくなる
  ようにしたい。
  シングルスレッドバージョンはconnect->accept->read->write->close
  する。
  */  
/* must be mutex'ed */
pthread_mutex_t buflock;
static char mt_cmd[1000], mt_result[1000];
int read_ok=0;
pthread_cond_t cond_read_ok;
pthread_mutex_t lock_read_ok;


/*
  parent thread.
  
  do readline
 */
int parent_mt( void )
{
    while(1){
        char result[1000];
        char *p = readline( "aho>" );
        if( p ==NULL )exit(0);
        add_history( p );

        pthread_mutex_lock( &buflock );
        strcpy( mt_cmd , p );
        pthread_mutex_unlock( &buflock );

        /* wait for result is set up */
        pthread_mutex_lock( &lock_read_ok );
        while( read_ok != 1 ){
            pthread_cond_wait( &cond_read_ok , &lock_read_ok );
        }
        pthread_mutex_lock( &buflock );
        printf( "result: <%s>\n", mt_result );
        pthread_mutex_unlock( &buflock );
        read_ok = 0;
        pthread_mutex_unlock( &lock_read_ok );
    }
    
    return 0;
}

/*

  child thread, do important things

 */
int child_mt( void )
{
    int k = 0;

    while(1){
        char buf[100];

        usleep(200*1000);
        /* poll command line message is in buffer. */
        pthread_mutex_lock( &buflock );
        if( mt_cmd[0] ){
            int a;
            /* do something takes some time */
            for(a=0;a<3;a++){sleep(1);fprintf( stderr,".");}
            /* and set result */
            sprintf( mt_result , "%s:%d", mt_cmd,k++ );

            pthread_mutex_lock( &lock_read_ok );
            read_ok = 1;
            pthread_cond_broadcast( &cond_read_ok );
            pthread_mutex_unlock( &lock_read_ok );

            mt_cmd[0] = 0;
        }
        pthread_mutex_unlock( &buflock );
    }
    return 0;
}



/*
 */
int
main( int argc, char **argv )
{
    pthread_t pt , ct;
    pthread_attr_t pa , ca;

    pthread_attr_init( &pa );
    pthread_attr_init( &ca );

    pthread_mutex_init( &buflock , NULL );

    pthread_mutex_init( &lock_read_ok , NULL );
    pthread_cond_init( &cond_read_ok , NULL );
        
    pthread_create( &pt , &pa , (void*) &parent_mt , (void*)NULL );
    pthread_create( &ct , &ca , (void*) &child_mt , (void*) NULL );

    pthread_join( pt , NULL );
    pthread_join( ct , NULL );

    return 0;
}
