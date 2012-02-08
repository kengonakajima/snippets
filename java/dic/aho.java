import java.util.*;

class aho
{
	public static void main( String args[] )
	{
		Hashtable h = new Hashtable();
		String a = "asdfasdf";
		String b = "aaaa";
		h.put( "aho" , a );
		h.put( "ao" , b );
		
		System.out.println( h.keys() );
	}
}

