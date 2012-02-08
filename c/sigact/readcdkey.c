#include <stdio.h>
#include <string.h>

#include "netio.h"


/*
 * readcdkey 
 *
 * char[][] cdkey , char[][] passwd 
 * each should be at least 10000x20 in size.
 */
void readcdkey( char* fname, char** cdkey, char** passwd )
{

  FILE* fp;
  /*  char cdkey[10000][20]; */
  /*  char passwd[10000][20];*/
  char line[1000];
  int i = 0;
  int succeeded = 0;
  
  fprintf( stderr, "Reading cdkey file: %s ...\n", fname );

  if( ( fp = fopen( fname, "r" ) ) == NULL ) {
	fprintf( stderr, "Failed to read %s\n", fname );
	return;
  }

  while( ( fgets( line, sizeof( line ), fp ) ) != NULL ){

	char *t1, *t2;

	t1 = strtok( line, " " );
	fprintf( stderr, "%x %s ## %d ##", cdkey[i], t1, strlen( t1 ) );
//	strncpy( cdkey[i], t1, strlen(t1) );
//	memmove( cdkey[i], t1, strlen(t1) );
	t2 = strtok( NULL, " " );
//	strncpy( passwd[i], t2, strlen(t2) );
//	memmove( passwd[i], t2, strlen(t2) );
	fprintf( stderr, "%d(%d): %s\t%s", succeeded, i, t1, t2 );
	
	i++;
	
  }
	

  fprintf( stderr, " successfully read %d of %d.", succeeded, i );
  
  
}
