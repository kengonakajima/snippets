import  java.applet.*;
import  java.awt.*;
import  java.lang.*;

public
class LayoutPaint extends Applet implements Runnable {
        Image           img;
        Image           offs = null;
        Graphics        offg = null;
        Thread          kicker;
        int             second = 0;

        public void init(){
                setBackground( Color.white );
        }

        public void start(){
                kicker = new Thread( this );
                kicker.start();
        }

        public void stop(){
                kicker.stop();
                kicker = null;
        }

        public void run(){
                while( kicker != null ){
                        repaint();
                        this.second++;
                        try {
                                Thread.sleep( 1000 );
                        } catch ( InterruptedException e ){
                                break;
                        }
                }
        }

        public void layout(){
                Dimension       d = size();

                if( d.width != 0 && d.height != 0 ){
                        this.offs = createImage( d.width, d.height );
                        this.offg   = this.offs.getGraphics();
                }
        }

        public void update( Graphics g ){
                redraw( g );
        }

        public void paint( Graphics g ){
                setBackground( Color.white );
                redraw( g );
        }

        public void redraw( Graphics g ){
                if( this.offg != null ){
                        double  angle = ( 90 - 6 * this.second ) * Math.PI / 180.0;
                        int     dx = (int)( 100 * Math.cos( angle ) );
                        int     dy = (int)( 100 * Math.sin( angle ) );
                        int     cx = size().width  / 2;
                        int     cy = size().height / 2;

                        this.offg.setColor( Color.red );
                        this.offg.drawLine( cx, cy, cx+dx, cy-dy );
                        g.drawImage( this.offs, 0, 0, this );
                }
        }
}
