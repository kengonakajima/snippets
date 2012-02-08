import java.net.*;
import java.io.*;

class aho
{
	public static void main( String args[ ] )
	{
		Serv s = new Serv( "murasaki" ,9000 );
	}

}
class Serv implements Runnable
{
	Thread t;
	ServerSocket ss;
	Socket s;
	InputStream in;
	OutputStream out;

	Serv( String hostname , int port)
	{
		t = new Thread(this );
		t.start();

		try{
			ss = new ServerSocket( port );
		

			s = ss.accept();
		
			System.out.println("accepted.");

			in = s.getInputStream();
			out = s.getOutputStream();
		}catch( IOException e ){}
		
	}

	public void run()
	{
		while( true )
		{
			try{
				System.out.println( in.read() );
			}catch( IOException e ){}
	
		}

	}

}
