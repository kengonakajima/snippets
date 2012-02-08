import java.awt.*;

public class aho extends java.applet.Applet
{


	
	public void paint(Graphics g){	
		double x , y,dx,dy,ddx,ddy;
		x = 0;
		y = 0;
		dy = 0.0;
		dx = 1.0;
		ddx = 0;
		ddy = 0;
		y=100;

		for(int i=0;i<100;i++){

			
			if( i > 20 && i < 50 ) ddy = -0.5; else ddy=0;
			y += dy;
			x += dx;
			dy = dy + ddy - dy * 0.2;
			getGraphics().fillRect( (int)x,(int)(100+dy*10),2,2);
			getGraphics().fillRect( (int)x,(int)(200+ddy*10),2,2);

		}

	}


}
