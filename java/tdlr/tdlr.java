import java.awt.*;

public class tdlr extends java.applet.Applet
{

    public void paint(Graphics g)
    {
        for(int i=0;i<size().width;i++){
            for(int j=0;j<size().height;j++){
                g.fillRect( j , i  , 1,1);
				try{
					Thread.sleep(10);
				}catch( InterruptedException e){}
            }
        }

    }

}


