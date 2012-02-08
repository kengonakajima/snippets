//JDK1.1 specific

// if the main method ends?
import java.io.*;

class hoge
{

	public static void main(String args[] )
	{
		ByteArrayOutputStream bout = new ByteArrayOutputStream();
		PrintStream po = new PrintStream( bout );
		System.setOut( po );
		System.out.println("fuckfufasdfasdfjh");
		
	}

}
