import java.applet.Applet;
import java.awt.*;
import java.net.*;
import java.io.*;
import java.util.*;
import java.awt.event.*;

public class client extends Applet implements Runnable,ActionListener 
{

	Socket clisock;
	Thread thread;
	Socket so;
	InputStream in;
	OutputStream out;
	DataInputStream din;
  
	Image offscr;
	Graphics offscrg;
	int width , height;
  
	int num;
	int x[] = new int[100];
	int y[] = new int[100];

	Color colortable[] = new Color[100];

	Button bt;
  
	public void init(){

    
		bt = new Button("push");  // Add by ringo
		add(bt);     // Add by ringo
		bt.addActionListener(this); // Add ringo
		
		//offscr = createImage(width = size().width , height = size().height );
		offscr = createImage(200,200);
		offscrg = offscr.getGraphics();
    
		for(int i = 0 ; i < 100; i++){
			colortable[i] = new Color( ( i*105)&255,(i*54)&255,(i*202)&255);
		}
    
		try{
			so = new Socket(getParameter("host") , Integer.parseInt(getParameter("port")));
			in = so.getInputStream();
			din = new DataInputStream( in );
			out = so.getOutputStream();
      
			/*     // Maybe this part is causing error.
				   Button bt;
				   add(bt = new Button("push"));
				   bt.addActionListener(new ActionListener(){
				   public void actionPerformed(ActionEvent e){
        
				   try{
				   int i=3;
				   out.write(i);
				   }
				   catch(IOException ex){
				   System.out.println("IOError");
				   }
				   }
				   });
			*/
      
		}
		catch( IOException e){
			System.out.println("Network error. Server down? or bad hostname/port?");
		}

    
		thread = new Thread(this);
		thread.start();
	}
	
	public void actionPerformed( ActionEvent e )  
	{
		try{
			byte b[] = new byte[1];
			b[0] = 3;
			out.write(b);
		} catch( IOException ex ){
			System.out.println( "hoge\n" );
			System.out.println( ex );
		}
	}
  
	public void paint( Graphics g)
    {
		g.drawImage( offscr , 0, 0, this );
    }
	public void update( Graphics g)
    {
		paint(g);
    }
  
	void dispAll(){
		offscrg.setColor(Color.white);
		offscrg.fillRect(0,0,width,height);
    
		for(int i=0; i < num; i++){
			offscrg.setColor(Color.black );
			offscrg.fillRect( x[i] , y[i] , 8, 8);
		}
	}
  
  
  
	/*
	  public boolean keyDown(Event e , int c)
	  {
      try{
	  out.write(c);
      }
      catch(IOException ex){}
      return true;
	  }
	*/
  
	public void run()
    {
      
		while(true)
			{

				//ForDebug
				/*
				  x[0]=100;
				  y[0]=100;
				  x[1]=70;
				  y[1]=70;
            
				  dispAll();
				  repaint();
				*/
          
				int counter = 0;
       
				while(true)
					{
						int tmpx = 0,tmpy = 0;
						try{
							tmpx = din.readShort();
							tmpy = din.readShort();
						}
						catch(IOException e){
							break;
						}
              
						if( tmpx == -1 && tmpy == -1){

							break;
						}
						else{
							x[counter] = tmpx;
							y[counter] = tmpy;
							counter++;
						}
					}
           
				num = counter;
				dispAll();
				repaint();
           
				try{
					Thread.sleep(100);
				}
				catch(InterruptedException e){}
			}
	}
      
}

