import ringo.*;
import java.io.*;

class hoge
{
	public static void main(String args[] )
	{
		ConsolePrintStream cs = new ConsolePrintStream();
		PrintStream eo , oo;
		eo = System.err;
		oo = System.out;
		System.setOut( cs );
		System.setErr( cs );
		System.out.println("asdfkjadlkasdaklkjhkh");
		System.err.println("asdfsad");
		byte[]b = new byte[10];
		b[12] = 20;

		
	}

}
