import java.applet.Applet;
import java.awt.*;
import java.awt.image.*;
import java.util.*;


public class game extends Applet implements Runnable
{


	Thread thread;
	Image double_buffer;
	Graphics dg;

	int cron=0;

	public final int width ,height;
	public final int interval = 500;	/* milli second */

	public void init()
	{


		thread = new Thread(this);
		double_buffer = createImage( width , height );
		dg = double_buffer.getGraphics();

		width = size().width;
		height = size().height;

		thread.start();

	}

	public void paint( Graphics g )
	{
		g.drawImage( double_buffer , 0 , 0 , null );

	}
	public void update( Graphics g )
	{
		paint( g );
	}
	public void run()
	{
		while( true )
		{
			
			cron++;

			try{
				Thread.sleep( interval );
			} catch( InterruptedException e ){}

			doIt();
			repaint();
		}


	}


	// $B%2!<%`$N>pJs(B
	URL cgi_url;
	int score;
	int life;
	String playername;

	// $B%\!<%k$N>pJs(B
	int numberOfBalls;   
	boolean alive[];
	float x[] ,y[], dx[] , dy[];
	float x1 , y1,x2,y2;
	float xsize =8.0, ysize =8.0;
	Color col[];

	
	void initBalls(int num )
	{
		this.numberOfBalls = num;
		x1 = 0.0;
		y1 = 0.0;
		x2 = (float) size().width;
		y2 = (float) size().height;

		alive = new boolean[num];
		x = new float[num];
		y = new float[num];
		dx = new float[num];
		dy = new float[num];

		for(int i = 0 ;i < num ;i++ ) alive[i] = false;
	}		

	private void addBall(int i )
	{
		for(int i = 0 ;i < numberOfBalls ;  i++ ){
			if( alive[i] == false ){
				x[i] = (float)x1+(float)(x2-x1)*Math.random();
				y[i] = y1;
				dx[i] = (float)(x2-x1)/100.0*(Math.random()-0.5);
				dy[i] = (float)(y2-y1)/100.0*(Math.random()-0.5);
				col[i] = new Color(
						128+(int)( 128.0 * Math.random() ),
						128+(int)( 128.0 * Math.random() ),
						128+(int)( 128.0 * Math.random() )
						);
			}
		}
		
	}
	public void paintBalls(Graphics g)
	{

		for(int i=0;i<num;i++ ){
			if( alive[i] == true ){
				g.drawOval( (int)x[i] , (int)y[i] , 
					   (int)xsize,(int)ysize );
			}
		}

	}
	private void moveBalls()
	{
		for(int i = 0 ; i < num ; i++ ){
			if( alive[i] == false )continue;
			x[i] += dx[i];
			y[i] += dy[i];
			dy[i] += 0.004;   // $B47@-%"%k%4%j%:%`(B($B=ENO2CB.EY(B)
			if( x[i] < x1 ){ x[i] = x1; dx[i] = - dx[i]; }
			if( x[i] > (x2-xsize) ){ x[i] = x2-xsize;dx[i] =-dx[i];}
			if( y[i] < y1 ){ y[i] = y1; dy[i] = -dy[i]; }
			
			if( y[i] > (y2-ysize) ){
				life--;
				y[i] = y2-ysize;
				dy[i] = -dy[i];
			}
			
		}
	}

	final int TITLE = 1;
	final int GAME = 2;
	final int HIGHSCORE = 3;
	int gamemode = TITLE;            // $B>e5-$NCM$rF~$l$k!#(B


	Color bg = new Color( 0x00,0x16,0x29);
	Color fg1 = new Color( 0xff ,0xee,0x44 );
	Color fg2 = new Color( 0xff , 0xbb, 0x33 );

	void doIt()
	{
		// $B%*%U%9%/%j!<%s$r>C$9(B
		g.setColor( bg );
		g.fillRect( 0,0,size().width, size().height );

		switch( gamemode ){
			case TITLE:
			dispayTitle();
			break;
			case GAME:
			break;
			case HIGHSCORE:
			break;
		}
		
	}
	public boolean keyDown( Event e , int c )
	{
		switch( gamemode ){
			case TITLE:
			// $B%?%$%H%k$G$O!"%-!<F~NO$OL>A0$NF~NO$G!"%j%?!<%s%-!<(B
			// $B$r2!$9$HH4$1$k!#(B
			if( c == '\n' )gameStart(); 
			else {
				
			}
			break;
			case GAME:
			break;
			case HIGHSCORE:
			break;
			
		}
	}

	void displayTitle()
	{
		// $B$"$/$^$G$b%3%s%]!<%M%s%H$O;H$o$J$$!#(B

		g.setColor( fg1 );
		g.drawString( "Bound balls (CGI test)",50,50);
		g.setColor( fg2 );
		g.drawString( "Input your name " , 60,100 );
		g.setColor( fg1 );
		g.drawString( playername , 60, 140 );
		g.setColor( fg2 );
		g.drawLine( 60 , 142 , 200,142 );
	}
	

	

}

/*
  $B$^$:%\!<%k$N>pJs$r07$&JQ?t$r7h$a$k!#(B
  $B=i4|2=ItJ,$r:n$k(B

*/
























