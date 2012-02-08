class aho
{
	public static void main(String args[] )
	{
		hoge h = new hoge(100);
		hoge nh = new hoge(h);

		nh.kasu();
	}

}

class hoge
{
	int k ;

	hoge(int i)
	{
		k = i;
	}

	public void kasu()
	{
		System.out.println( k );
	}

}
