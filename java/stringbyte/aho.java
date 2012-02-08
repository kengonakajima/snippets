class aho
{
	public static void main(String args[] )
	{
		byte b[] = { (byte)'a' , (byte)'h' , (byte)'o'};
		char c[] = { 'a' , 'h' , 'o' };
		String s = new String( b, 0);
		System.out.println( "aho".equals("aho"));
		System.out.println( s.equals("aho"));
		System.out.println( c.equals("aho"));
	}

}
