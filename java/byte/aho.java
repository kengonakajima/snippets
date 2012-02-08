class boke
{

	public static void main( String args[] )
	{
		int a;
		byte c;
		int d;
		byte e;

		for(a = 0 ; a < 256 ; a++ ){
			
			c = (byte ) a;
			System.out.print( a + " " + c );
			d = (int ) (c & 255 );
	
			System.out.println( ":" + d  );
		}

	}

}
