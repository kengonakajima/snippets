import java.lang.*;

/*class QQ
{
	String s;
	int l;
	QQ( String sss ){
		s = sss;
		l = s.length();
	}

	public void fuck(){
		System.out.println( s );
	}
}

class Hoge
{
	int a;
	float f;
	QQ q;
	public Hoge(int aa){
		a = aa;
		f = 1.1f;
		q = new QQ( "asdf" );
	}
	public void tomo(){
		q.fuck();
	}
}
*/

class Hoge
{
	int a;
	Hoge( int aa ){
		a = aa;
	}
}
public class Aho
{
	public static void main( String args[])
	{
		Hoge h = new Hoge(10);
	}
}