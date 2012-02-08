
import java.applet.Applet;
import java.io.*;

public class aho extends Applet
{
	public void init( )
	{
		while(true)
		{
			int c=0;
			try{

				c = System.in.read();
			}catch( IOException e ){
				System.out.println( e );
			}

			System.out.println( c );
		}

	}

}

