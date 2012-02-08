#include <stdio.h>
//#include </opt/chiaki/mysql-3.23.51/include/mysql.h>
#include <mysql.h>

int main( void )
{
    MYSQL sql_server;
    MYSQL *d_mysql;
    MYSQL_ROW sql_row;
    MYSQL_RES *result;
    char query[2048], m_name[64];

    /* initialize */
    mysql_init( &sql_server );
        
    /* connect to MySQL */
    d_mysql = mysql_real_connect( &sql_server, "localhost", "chiaki", "", "domain", 0, NULL, 0 );
    
    if( d_mysql==NULL )
    {
        printf( "@@@@@@ cannot connect @@@@@@@\n");
        exit(1);
    }
    /* select DB */
    if(mysql_select_db(d_mysql, "domain")!=0)
    {
        printf( "@@@@@@ cannot select DB @@@@@@\n");
        exit(1);
    }

    /* query */
    snprintf( query, 2048, "select * from test");
    mysql_query( d_mysql, query );

    result = mysql_store_result( d_mysql );
    if( !result )
    {
        printf( "@@@@@@ query error @@@@@@\n");
        free(query);
        mysql_free_result(result);
        exit(1);
    }
    sql_row = mysql_fetch_row( result );
    sprintf( m_name, "%s", sql_row[0]);
    mysql_close(d_mysql);
    return 0;
}    
