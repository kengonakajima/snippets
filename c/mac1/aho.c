#define LOG( x)	\
{char msg[1000];\
sprintf( msg , x );\
}\


main()
{
	LOG( ("%d %d\n" ,100,200 ) );
	
}
