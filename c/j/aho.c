int main()
{
	struct aaa {
		int a;
	};
	struct aaa a;
	struct aaa b;

	a.a = 100;
	b.a = 100;

	if( a == b ){
		printf("fuck\n" );
	}
		
	
}
