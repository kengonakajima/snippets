import java.awt.*;
public class aho extends java.applet.Applet
{
	Frame f;
	Image i;
	public void init()
	{
		 f = new Frame("another window");
		 f.resize(200,200);
		 f.show();
		 i = getImage( getDocumentBase() , "kawai.jpg" );
	}
	public void paint(Graphics g)
	{
		f.getGraphics().drawImage( i , 0 , 0 , this );
	}
	

}
