import java.io.*;
import java.net.*;


class aho
{
	public static void main(String args[] )
	{
		fucker f = new fucker("www.javasoft.com", "/");
	}
}

class fucker
{
	Socket s;
	PrintStream out;

	InputStream in;

	fucker(String host , String filename)
	{
		try{
			s = new Socket( host , 80 );
			out = new PrintStream(  s.getOutputStream() );
			in = s.getInputStream();

			out.println("GET " + filename + " HTTP/1.0" );
		
			while(true)
			{
				System.out.print( (char) in.read() );
			}
		} catch( IOException e ){
			System.out.println( e );
		}

	}

}
