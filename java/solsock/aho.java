import java.net.*;
import java.io.*;

class sss 
{
	public static void main(String args[] ){
		sss s = new sss( 9000 );
	}
	

	ServerSocket ss ;
	Socket sock;
	InputStream in;
	OutputStream out;

	// server constructor
	sss( int port ){
		try{
			ss = new ServerSocket( port );
			sock = ss.accept();
			in = sock.getInputStream();
			out = sock.getOutputStream();

			while(true)
			{

				int c;
				c = in.read();
				out.write(c );
				System.out.println( (char) c );
			}
		}catch(Throwable t ){}

	}
}



