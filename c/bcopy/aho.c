/*

   A performance test of bcopy


   a.out count bsiz
*/

int main(int argc, char **argv )
{
	int i;
	int siz;
	int count;
	char *buf, *buf2;

	siz = atoi( argv[2] );
	count = atoi( argv[1] );
	
	buf = malloc( siz );
	buf2 = malloc( siz );

	for(i=0;i< count;i++ ){
		bcopy( buf , buf2 , siz ); 
	}
	
}
