

int b[10] = {3,2,1,4,3,32,2};
      
void aho( int **f)
{

	*f = b;
	
}
main()
{

	int *a;
	int p;
	
	aho( &a );

	printf( " %d\n" , a[2] );
}

