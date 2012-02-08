import java.awt.*;
public class components extends java.applet.Applet
{
	Panel p;

	Button b = new Button("OK");
	Button c = new Button("Cancel");
	TextArea d = new TextArea("",30,20);
	public void init()
	{
		p = new Panel();
		p.setLayout( new BorderLayout() );
		p.add( "East" , c );
		p.add( "West" , b );
		setLayout( new BorderLayout() );
		add( "Center" , d );
		add( "South" , p );

	}

	
}
