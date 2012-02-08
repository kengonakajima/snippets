import java.awt.*;

class MFrame extends Frame
{
	MFrame()
	{
		super("Mframe");
		resize(100,200);
	}
	public void paint(Graphics g ){
		Insets ins = insets();
		g.setColor( Color.black );
		g.fillRect( 0 , 0 , 1000,1000);
		g.setColor( Color.yellow );
		
		g.drawRect( ins.left , ins.top , 
				   size().width-ins.right-ins.left-1,
				   size().height-ins.bottom-ins.top-1);
	}

}

public class aho extends java.applet.Applet
{
	MFrame f;

	public void init()
	{
		f = new MFrame();
		f.show();

	}

	public void paint(Graphics g){
		int right , left , top,bottom;

		Insets ins = insets();
		right = ins.right;
		left = ins.left;
		top = ins.top;
		bottom = ins.bottom;

		System.out.println( ins );
		g.setColor( Color.black );
		g.fillRect( 0 , 0 , 1000,1000);
		g.setColor( Color.yellow );

		g.drawRect( 0 ,0 , size().width-1 , size().height-1 );
	}
}
