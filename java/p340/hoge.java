import ringo.*;
class hoge
{
	public  static void main(String args[] ){
		ConsolePrintStream cs = new ConsolePrintStream();
		System.setOut( cs );
		System.setErr( cs );
		System.out.println( "fguc you ");
		throw new ArrayIndexOutOfBoundsException();
	}
}
										
