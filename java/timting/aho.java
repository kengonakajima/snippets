import java.awt.*;


class hoge implements Runnable
{
	Thread t;
	hoge()
	{
		t = new Thread( this );
		t.start();
	}

	public void run()
	{
		
	}

}

public class aho extends java.applet.Applet  implements Runnable
{
	boolean flg = false;

	public boolean mouseDown( Event ev , int x , int y ){
		
		flg = true;
		repaint();

		return true;
	}

	public void paint(Graphics g ){
		flg = false;
	}


	Thread t;
	public void init()
	{
		t = new Thread( this );
		t.start();
	}
	public void run()
	{
		while(true)
		{

			long tm = System.currentTimeMillis();
			while(true)
			{

				try{Thread.sleep(10);}catch( InterruptedException e ){}

				if( flg == false ){
					break;
				}
			}
			System.out.println( System.currentTimeMillis() - tm );
		}
	}

}
