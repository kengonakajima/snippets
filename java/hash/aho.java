import java.io.*;

class hoge
{
	public static void main(String args[] )
	{
		try{


		DataInputStream in = new
		DataInputStream( new FileInputStream("links" ) );
		
		int hash[] = new int[10000];
		String sa[] = new String[10000];

	
		for(int i=0;;i++){
			String s = in.readLine();
			if( s == null ) break;
			sa[i] = s;
			hash[i] = s.hashCode();
			for(int j=0;j<i;j++){
				if( hash[i] == hash[j] ){
					System.out.println( sa[i] + " equals ");
					System.out.println( sa[j] +" CODE:" + hash[i] );
				}
			}
		}
		
		}catch( Throwable t ){
			System.out.println( t );
		}
	}
}
