#include <stdio.h>
#include <fcntl.h>


#define BUFSZ	1024*1024*40 




#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

int fd;
int bufsize;

void memRead( char *buf , int ofs , int bytes )
{
	lseek( fd , ofs , SEEK_SET );
	read( fd , buf , bytes );
}
void memWrite( char *buf , int ofs , int bytes )
{
	lseek( fd , ofs , SEEK_SET );
	write( fd , buf , bytes );

}

char data[512] ;

void createEmptyFile( char *fname , int size )
{
	int i;

	FILE *fp;
	fp = fopen( fname , "w+" );
	
	for(i=0;i<size;i++ ){
		fwrite( data , 512 , 1 , fp );
	}
	fclose(fp);
	
}



int test()
{
#define WINDOWSIZ 100
	
	char buf[WINDOWSIZ];
	char ttt[100];
	int i;


	getctime( ttt );
	printf( "start 1 %s\n" ,ttt);
	for(i=0;i<BUFSZ/1024;i+=100 ){
		buf[0] = i;
		memWrite( buf , i * 1024   , WINDOWSIZ );
	}

	for(i=0;i<BUFSZ/1024;i+=100 ){
		buf[0] = i;
		memRead( buf , (i + 50) * 1024   , WINDOWSIZ );
	}
	getctime(ttt);
	printf( "start 2 %s\n" ,ttt);

	for(i=0;i<BUFSZ/1024;i+=100 ){
		buf[0] = i;
		memWrite( buf , (i + 10 )* 1024   , WINDOWSIZ );
	}

	for(i=0;i<BUFSZ/1024;i+=100 ){
		buf[0] = i;
		memRead( buf , (i + 10) * 1024   , WINDOWSIZ );
	}
	getctime(ttt);
	printf( "start 3 %s\n" ,ttt);

	
	
	
}


void getctime( char *text )
{
    time_t t;
    char *buf;
    int i;

    time( &t );
    buf = ctime( &t );

    strcat( buf , ":" );

    strcpy( text , buf );
}


int main()
{

/*	createEmptyFile( "/lifestorm/hoge" , BUFSZ/512 ); */
	
	fd= open( "/lifestorm/hoge" , O_RDWR );
	if( !fd  ) return;

	test();
	
}

