
hoho( char **aho)
{
	printf("%s %s %s\n" , aho[0] , aho[1] , aho[2] );
	
}

main()
{
	char data[10][10]={"aho","boke","hoge"};
	char **aho;
	char *hoge[10];

	hoge[0] = data[0];
	hoge[1] = data[1];
	hoge[2] = data[2];

	aho  = hoge;
	hoho( aho );
	printf("'%s' '%s' '%s'\n" , hoge[0], hoge[1] , hoge[2]);
	
}

