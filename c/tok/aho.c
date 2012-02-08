#include <stdio.h>

main(int argc , char **argv )
{


	char *boke[20];

	fprintf(stderr,"%d\n" , 	 urlPostTokenize( boke , argv[1] ));
	
	{
		int i;
		for(i = 0 ; ; i ++ ){
			if( boke[i] == NULL ) break;
			fprintf(stderr, boke[i] );
			fprintf(stderr,"\n" );
		}
	}
	
}



/***
  urlDecodingに特化したトークナイザー。
  char型のポインタの配列に出力する。
  返り値は、トークンの数。
  
**/  
int urlPostTokenize( char **out , char*in )
{
	int c;
	int count = 0;
	
	out[0] = in;
	
	while( c = *in){
		if( c == '&' ){

			*in = '\0';
			out[++count] = (char *)(in + 1);
		}
		

		in++;
	}
	out[++count] = NULL;
	return count;
}
				 


