import java.awt.*;

public class aho extends java.applet.Applet
{
	public boolean keyDown ( Event e , int c)
	{
		System.out.println( c + " " + e.id + " " + e.when + " " + e.key + "D!" + e.DELETE + "B" + Event.BACK_SPACE );
		return true;
	}

}
