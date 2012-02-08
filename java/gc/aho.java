class hao
{

	public static void main(String args[] )
	{
		int aho[] = new int[10];
		int hoge[] = new int[200];
		int kasu[] = new int[50];
		Object hage[] = new Object[100];

		hage[3] = aho;

		((int[])hage[3])[2] = 808;

		System.out.println( aho[2] );

/*
		aho[0] = 808;
		aho[2] = 909;

		hoge = aho;

		System.out.println( hoge[2] );
		System.out.println( aho[2] );

		for(int i=0;i<10000;i++){
			aho = new int[2000];
			aho = new int[3000];
			

			System.gc();
		}
*/

		
	}

}
