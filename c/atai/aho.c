

void
hoge( int *a )
{
	int k = *a;

	printf( "%d\n", k );

	*a = 100;
}

int main()
{

	int a;
	a = 20;
	hoge( &a );
	printf( "%d\n", a );
	return 0;
}


	
