main()
{
	char b[100];
	int i;
	gethostname( b, &i );
	printf("%d\n" , i );
	printf( b );
}
