
public class hoge extends java.applet.Applet implements Runnable
{
	Thread t;

	public void init()
	{
		t = new Thread( this );
		t.start();
	}
	public void run()
	{
		System.out.println( "fuck");
	}

}
