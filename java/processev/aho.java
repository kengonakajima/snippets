import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;

public class aho extends Applet implements KeyListener
{

	Panel p;
	public void init()
	{
		p = new Panel();
		p.addKeyListener( this );
		add(p );
		
	}
	public void keyTyped(KeyEvent e ){
		System.out.println("type");
	}
	public void keyPressed(KeyEvent e ){
		System.out.println("press");
	}
	public void keyReleased( KeyEvent e ){
		System.out.println("release");
	}


}
