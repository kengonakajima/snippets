#include <stdio.h>
#include <stdlib.h>

main(int argc , char **argv)
{
	int i;
	char text[1000] = "asdlfahsdkjlkfjhaslkdjfhaklsdjfaskjdfklasjdflkjasdhasdfkjhaslkdkfjh"
			  "asdkfjhaslkdfjhaslkdjfhklasdjfklajsdflkasdflkahjsdfasdf\nasdfjhaskldfj"
			  "asdfkjkjahsdfkjasdfasdfasdfasdfasdfasjkdfhalskjdfhlkasjdhfkasjhdfkahsd"
			  "fkjhasdfasaskdjfhaksdjfasdklfkjahsdlfkjhasdklfjhasldkfjaskldfaslkjha"
			  "sdfkjhasdkjfjhaskldfjhaslkdjfkasjdflkashdfkahjdfkjasalskdjfhlkasdj";
	
	for(i = 0 ; i < atoi( argv[1]) ; i++ ){
		FILE *fp;
		char filename[1000] ;
		sprintf(filename ,"0%d" , i );
		fp = fopen( filename , "w" );
		fprintf(fp , text );
		fclose( fp );
	}
					      
}

