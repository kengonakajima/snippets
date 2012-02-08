#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"
#include <errno.h>
#include <time.h>
#include <sys/time.h>


/*
  MySQL speed test

  Connect test
  1. real_connect
  2. close.
  
  Create/Drop test
  1. real_connect
  2. create table and drop table many times
  3. close_mysql
  
  Insert/Update/Search test
  1. create table
  2. insert 1Meg entries
  3. update 1Meg entries randomly
  4. simple search random names 1Meg times
  5. drop table 

  Host: localhost
  User: test
  Database: test
  Table: speed ( unsigned int ID, char(32)name, char(32) entry32, blob(128) )

    Don't do appropriate error handling!
  
 */
void connect_test( int loop );
void clean( void );
int persec( struct timeval *s, struct timeval *e , int c);
void insert_update_select_test( int iloop, int uloop, int sloop , int dloop);
void create_drop_test( int loop );


#define HOST "localhost"
#define USER "test"
#define DB "test"

/* この表でPPro200でやると、4000個insert が 2300/sec, update が10/sec,
   selectが30/sec, deleteが12/secだった。それぞれ4000個のエントリをもつ。
   表のデザインを変えるだけで高速化できないか探る。
   まず、検索が、個数の1乗に比例して遅くなる。
   */
#define _CREATE_STRING "create table speed ( id int unsigned not null , name char(32), tx blob );"

/*
  idを primary key にしてみる。そうすると...
  4000個のセッティングで、
  insert: 2100/sec
  update:1000/sec
  select:1400/sec
  delete:1877/sec
  になった。100倍ぐらいはやくなった。さらに、40万個にすると、
  update は2割遅いぐらい、あとはまったく変化なしだった。これは
  IDで検索するからと思われる。今度は名前で検索してみる。
 */
#define __CREATE_STRING "create table speed ( id int unsigned not null , name char(32), tx blob , primary key( id ) );"

/*
  name をindexすると8倍はやくなったが、idの検索よりは遅い。
  エントリの数を40000にすると、7倍遅くなった。まだチューンできるようだ。
 */
#define ___CREATE_STRING "create table speed ( id int unsigned not null , name char(32) not null, tx blob , primary key( id ) , key index_name ( name(32) ) );"
/* key index_name( name(6) )にしても変化なし */
#define CREATE_STRING "create table speed ( id int unsigned not null , name char(32) not null, tx blob , primary key( id ) , key index_name ( name(6) ) );"

int main( int argc , char **argv )
{
    clean();
    
    connect_test( 100 );

    create_drop_test( 100 );

    insert_update_select_test( 4000, 100, 1000 , 4000 );
//    insert_update_select_test( 400000, 100, 1000 , 4000 );    
//  lock_unlock_test(1000);

}
void
clean( void )
{
    MYSQL mysql , *sock;
    int ret;
    
    mysql_init( &mysql );
    sock = mysql_real_connect( &mysql, HOST,USER,NULL,DB,0,NULL,0);
    if( !sock ){
        fprintf(stderr , "clean: cannot real_connect!\n");
        exit(1);
    }
    ret = mysql_query( &mysql ,
                       "drop table speed;" );
    fprintf( stderr , "clean: %s\n", mysql_error( &mysql ) );
    
    mysql_close(sock);
    return;
}

void
connect_test( int loop )
{
    int i;
    struct timeval t,et;
    gettimeofday( &t, NULL );
    
    for(i=0;i<loop;i++){
        MYSQL mysql, *sock; 
        MYSQL_RES *res;
        int ret;

        mysql_init( &mysql );
        sock = mysql_real_connect( &mysql , HOST ,
                                   USER , NULL , DB , 0,NULL,0);
        if(!sock){
            fprintf(stderr , "cannot real_connect!\n");
            exit(1);
        }
        mysql_close( sock );
    }
    gettimeofday( &et, NULL );
    fprintf( stderr ,"connect test finished (%d times,%d/sec)\n" ,
             loop , persec( &t, &et , loop ) );
}
void
create_drop_test( int loop )
{
    int i, ret;
    MYSQL mysql, *sock; 
    MYSQL_RES *res;
    struct timeval t,et;
    gettimeofday( &t, NULL );
    
    mysql_init( &mysql );
    sock = mysql_real_connect( &mysql , HOST ,
                                   USER , NULL , DB , 0,NULL,0);
    if( !sock ){
        fprintf(stderr , "cannot real_connect!\n");
        exit(1);
    }

    for(i=0;i<loop;i++){
        /* create db する SQL文  */
        ret = mysql_query( &mysql , CREATE_STRING );

        if( ret ){
            fprintf(stderr , "%s\n", mysql_error(&mysql) );
            return;
        }

        ret = mysql_query( &mysql, "drop table speed;" );
        if( ret ){
            fprintf(stderr , "%s\n", mysql_error(&mysql) );
            return;
        }
    }
    mysql_close( sock );        

    gettimeofday( &et, NULL );
    fprintf( stderr ,"create drop test finished (%d times,%d/sec)\n" ,
             loop , persec( &t, &et , loop ) );
}

void create_random_name( char *out, int len );

void
insert_update_select_test( int iloop, int uloop, int sloop , int dloop)
{
    MYSQL mysql,*sock;
    int ret,i;
    struct timeval t,et;
    int loop;
    
    mysql_init( &mysql );
    sock = mysql_real_connect( &mysql, HOST,USER,NULL,DB,0,NULL,0);
    if( !sock ){
        fprintf(stderr , "clean: cannot real_connect!\n");
        exit(1);
    }
    /* create db */
    ret = mysql_query( &mysql, CREATE_STRING );
    if( ret ){
        fprintf(stderr , "insert_update_select_test: %s\n",
                
                mysql_error(&mysql) );
        return;
    }

    
    /* insert */
    loop = iloop;
    gettimeofday( &t, NULL );    
    for(i=0;i<loop;i++){
        char a[32];
        char q[1000];
        create_random_name(a,sizeof(a) );
        snprintf( q,sizeof(q),
                  "insert into speed "
                  "values ( %d, \"%s\" , \"aasdfjkhasdf\" );" ,
                  i, a );
        ret = mysql_query( &mysql, q );
        if( ret ){
            fprintf( stderr , "insert_update_select_test: %s",
                     mysql_error(&mysql ));
            break;
        }
        if( (i % 1000 ) == 0 )fprintf( stderr,"#");
    }
    gettimeofday( &et, NULL );
    fprintf( stderr ,"insert finished (%d entries,%d/sec)\n" ,
             loop , persec( &t, &et , loop ) );


    /* update */
    loop = uloop;
    gettimeofday( &t, NULL );    
    for(i=0;i<loop;i++){
        char a[32];
        char q[1000];
        create_random_name(a,sizeof(a));
        snprintf(q,sizeof(q),
                 "update speed set name=\"%s\" where id=%d;",
                 a, random() % loop );
        ret = mysql_query( &mysql, q );
        if( ret ){
            fprintf( stderr , "insert_update_select_test: %s",
                     mysql_error(&mysql ));
            break;
        }
        if( (i % 1000 ) == 0 )fprintf( stderr,"#");
    }
    gettimeofday( &et, NULL );
    fprintf( stderr ,"random update finished (%d entries,%d/sec)\n" ,
             loop , persec( &t, &et , loop ) );

    /* select by ID*/
    loop =sloop;
    gettimeofday( &t, NULL );
    for(i=0;i<loop;i++){
        char a[32];
        char q[1000];
        snprintf(q,sizeof(q),
                 "select * from speed where id=%d;" ,
                 random() % loop );
        ret = mysql_query( &mysql,q);
        if( ret ){
            fprintf( stderr , "insert_update_select_test(select): %s",
                     mysql_error(&mysql ));
            break;
        } else {
            MYSQL_RES *result;
            result = mysql_store_result(&mysql);
            if(result){
                mysql_free_result(result);
            }
        }
        if( (i % 1000 ) == 0 )fprintf( stderr,"#");        
    }
    gettimeofday( &et, NULL );
    fprintf( stderr ,"random id search finished (%d entries,%d/sec)\n" ,
             loop , persec( &t, &et , loop ) );

    /* select by name */
    loop =sloop;
    gettimeofday( &t, NULL );
    for(i=0;i<loop;i++){
        char a[32];
        char q[1000];
        create_random_name(a,sizeof(a));
        snprintf(q,sizeof(q),
                 "select * from speed where name=\"%s\";" ,a );
        ret = mysql_query( &mysql,q);
        if( ret ){
            fprintf( stderr , "insert_update_select_test(select): %s",
                     mysql_error(&mysql ));
            break;
        } else {
            MYSQL_RES *result;
            result = mysql_store_result(&mysql);
            if(result){
                mysql_free_result(result);
            }
        }
        if( (i % 1000 ) == 0 )fprintf( stderr,"#");        
    }
    gettimeofday( &et, NULL );
    fprintf( stderr ,"random name search finished (%d entries,%d/sec)\n" ,
             loop , persec( &t, &et , loop ) );

    /* delete */
    loop = dloop;
    gettimeofday( &t, NULL );
    for(i=0;i<loop;i++){
        char a[32];
        char q[1000];
        snprintf(q,sizeof(q),
                 "delete from speed where id=%d;" ,
                 random() % loop );
        ret = mysql_query( &mysql,q);
        if( ret ){
            fprintf( stderr , "insert_update_select_test(select): %s",
                     mysql_error(&mysql ));
            break;
        }
        if( (i % 1000 ) == 0 )fprintf( stderr,"#");        
    }
    gettimeofday( &et, NULL );
    fprintf( stderr ,"random delete finished (%d entries,%d/sec)\n" ,
             loop , persec( &t, &et , loop ) );

    
    mysql_close( sock );
}

int
persec( struct timeval *s, struct timeval *e , int c )
{
    double dt =
        ( e->tv_sec  - s->tv_sec ) +
        ( ( e->tv_usec -
            s->tv_usec ) / (double)1E6 );
    return 1.0 / dt * c ;
}

void
create_random_name( char *out, int len )
{
    char *cand[] = {
        "ane", "baru", "char", "del", "empo", "fuck", "gon", "hai",
        "illi", "jit" , "k" , "mas", "i" , "s", "u", "e" , "m" ,
        "r", "rr", "roo", "mo" , "te" , "hi", "ta", "fu" ,
        "thel" , "Galomono" , "turbolin" , "ux" , "ted",
        "mako" , "saru", "a" , "pu", "Back", "chop", "punch" };
    
    int ar = sizeof(cand)/sizeof(char *);
    
    switch( random() % 5)
    {
    case 0:
        snprintf( out, len, "%s%s%s",
                  cand[random()%ar],
                  cand[random()%ar],
                  cand[random()%ar] );
        break;
    case 1:
    case 2:
        snprintf( out, len, "%s%s",
                  cand[random()%ar],
                  cand[random()%ar] );
        break;
    case 3:
    case 4:
        snprintf( out, len, "%s", cand[random()%ar] );
        break;
    }
}

