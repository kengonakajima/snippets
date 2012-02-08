#include <stdio.h>

main(int argc ,char **argv)
{
	char text[1000] = "8793874129384716923874691283746876192837469128734691827346918273649187"
			  "389475629384756928347569823475692387456923875692387456923875692834756"
			  "847569238475629384756923847562938475692347569238475692834576923487";
	int i;
	
	for(i = 0 ; i < atoi( argv[1] ) ; i++ ){
		FILE *fp;
		char filename[1000] ;
		sprintf(filename ,"0%d" , i );
		fp = fopen( filename , "a" );
		fprintf(fp , text );
		fclose( fp );
	}
}
