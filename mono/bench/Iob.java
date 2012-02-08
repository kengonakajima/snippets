import java.lang.*;

public class Iob
{

	public static int numofbits(int bits) {
  		bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
  		bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
  		bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
  		bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
  		return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
	}
	public static String[] st = { "C", "G", "A", "T" };
	public static String convert(int n)
	{
		return st[ n % 4];
	}
	
	public static void main( String args[])
	{
		int N = Integer.parseInt(args[0]);
		for(int i=0;i<N;i++){
			int num = numofbits(i);
			System.out.print( convert(num) );
		}
	}
}