
int
main()
{
	triangle_t t = {
		{ 100,200,150 },
		{ 100,200,200 }
	};
	int k;

	k = check_inside_triangle( 170,190, &t );
	printf( "170,190 : %d\n", k );

	k = check_inside_triangle( 0,0, &t );
	printf( "0,0 : %d\n", k );	

	k = check_inside_triangle( 300,300, &t );
	printf( "300,300 : %d\n", k );


	k = check_inside_triangle( 170,170, &t );
	printf( "170,170 : %d\n", k );

	k = check_inside_triangle( 170,169, &t );
	printf( "170,169 : %d\n", k );				
}
