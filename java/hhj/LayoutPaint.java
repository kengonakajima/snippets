import  java.applet.*;
import  java.awt.*;

public
class LayoutPaint extends Applet {
        Image           img;
        Image           offimg = null;
        Graphics        offg = null;

        public void layout(){
                Dimension       d = size();
                if( d.width != 0 && d.height != 0 ){
                        this.offimg = createImage( d.width, d.height );
                        this.offg   = this.offimg.getGraphics();
                }
//              repaint();
        }

        public void paint( Graphics g ){
                if( this.offg != null ){
                        this.offg.setColor( Color.green );
                        this.offg.fillRect( size().width/2-50, size().height/2-50, 100, 100 );
                        g.drawImage( this.offimg, 0, 0, this );
                }
        }
}
