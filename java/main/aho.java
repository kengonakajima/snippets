class aho
{
	public static void main( String args[] )
	{
		aho a = new aho("hoge");

		a.func(10000);
	}

	int k=20;
	aho(String arg)
	{
		System.out.println("fufufu" + arg);
	}
	
	void func( int hoge )
	{
		System.out.println( hoge ) ;
	}
}
