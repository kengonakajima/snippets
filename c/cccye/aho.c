#include <stdio.h>

main()

{

	fprintf( stderr,"%d\n" , convertYesNoMessageFromText( " yes"  ) );
	fprintf( stderr,"%d\n" , convertYesNoMessageFromText( " hai"  ) );
	fprintf( stderr,"%d\n" , convertYesNoMessageFromText( " iie"  ) );
	fprintf( stderr,"%d\n" , convertYesNoMessageFromText( " no"  ) );

	
}

int convertYesNoMessageFromText( char *text  )
{
	int i;
	int c;

	
	while( c = *text++ ){
		if( c == ' ' || c=='#' || c==',' ){
			c = *text;			/* 次の文字 */
			switch( c )
			{
				case 'y':
				case 't':
				case 'h':
				if( strncmp( text , "yes" ) == 0 ||
				   strncmp( text , "true" ) == 0 ||
				   strncmp( text , "hai" ) == 0 ){
					return 1;
				}

				break;
				case 'n':
				case 'f':
				case 'i':
				if( strncmp( text , "no" ) == 0 ||
				   strncmp( text , "true" ) == 0 ||
				   strncmp( text , "iie" ) == 0 ){
					return 0;
				}
				break;
				default:
				break;

			}
		}
	}

	return -1;
}





