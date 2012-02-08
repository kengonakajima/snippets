void make64BaseString( int i , char *dest )
{
	char data[64]= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#";
	char dddd[8]={0,0,0,0,0,0,0,0};
	int tmp;

	if( i == 0 ){
	      strcpy( dest , "0" );
	      return;
	}
	
	tmp = i;

	printf( "A%d\n" , tmp );
	dddd[0] = tmp / ( 64*64*64*64*64 );		
	tmp -= dddd[0]*64*64*64*64*64;
	printf( "A%d\n" , tmp );
	dddd[1] = tmp/(64*64*64*64);
	tmp -= dddd[1]*64*64*64*64;
	printf( "A%d\n" , tmp );
	dddd[2] = tmp /( 64*64*64 );
	tmp -= dddd[2]*64*64*64;
	printf( "A%d\n" , tmp );
	dddd[3] = tmp /( 64*64 );
	tmp -= dddd[3]*64*64;
	printf( "A%d\n" , tmp );
	dddd[4] = tmp /( 64 );
	tmp -= dddd[4]*64;
	printf( "A%d\n" , tmp );
	dddd[5] = tmp;

	for(i=0;i<6;i++){
		if( ( tmp == 1 ) || ( dddd[i] > 0 && dddd[i]<=63 ) ){
			*dest++ = data[dddd[i]];
			tmp = 1;
		}
	}
	*dest = '\0';
}
	
	
main()
{
	char buf[100];
	make64BaseString( 0 , buf );
	printf( "%s\n" , buf );
}
