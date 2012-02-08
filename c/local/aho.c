int a;


main()
{
	int b[100];

	printf("%d %d\n" , a ,b[55] );

	aho( b );

	
}

aho( int pointer[100] )
{
	int i;

	for(i = 0 ;i < 32 ; i++){
		if( .... < .... ){
			continue;
		}
		pointer[i] = 2;
	}
	
	return;
}
