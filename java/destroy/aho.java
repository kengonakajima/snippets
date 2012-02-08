public class aho extends java.applet.Applet implements Runnable
{

	Thread t;
	public void init()
	{
		t = new Thread(this);
	}
	public void run()
	{

	}
	public void destroy()
	{
		t.stop();
	}

}


