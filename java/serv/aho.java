import java.net.*;
import java.io.*;

class aho 
{
	static public void main(String args[] )
	{
		try{
			ServerSocket ss;
			ss = new ServerSocket( 10000 ,5);
			Socket s;
			s = ss.accept();
			InputStream in;
			OutputStream out;
			in = s.getInputStream();
			out = s.getOutputStream();
			
			while(true)
			{
				if( in.available() > 0 ){
					System.out.println( (char )in.read() );
				}
			}
		}catch( Throwable t ){
			System.out.println("what is the fuck?" + t );
		}
	}
}
