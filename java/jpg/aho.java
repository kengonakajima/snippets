import java.applet.Applet;
import java.awt.*;

public class aho extends Applet
{
	Image img;
	public void init()
	{
		img = getImage( getDocumentBase(),"m9.jpg");

	}
	public void paint(Graphics g ){
		g.drawImage( img , 0,0,null);
	}


}
