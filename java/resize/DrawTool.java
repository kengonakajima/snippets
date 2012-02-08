import  java.applet.*;
import  java.awt.*;
import  java.lang.*;

public
class DrawTool extends Frame {
        Image           img;
        Image           offs = null;
        Graphics        offg = null;

        public static void main(String args[]) {
                DrawTool dt = new DrawTool();
                dt.setTitle("DrawTool Ver 0.0000009");
                dt.resize(400, 400);
                dt.show();
        }

        public void layout(){
                Dimension       d = size();

                if( d.width != 0 && d.height != 0 ){
                        Image img = offs;
                        this.offs = createImage( d.width, d.height );
                        this.offg   = this.offs.getGraphics();
                        this.offg.setColor(Color.white );
                        this.offg.fillRect(0, 0, d.width, d.height);
                        if (img != null)
                                offg.drawImage(img, 0, 0, null);
                }
        }

        public void update( Graphics g ){
                redraw( g );
        }

        public void paint( Graphics g ){
                redraw( g );
        }

        public void redraw( Graphics g ){
                if( this.offg != null ){
                        this.offg.setColor( Color.red );
                        g.drawImage( this.offs, 0, 0, null );
                }
        }
        public boolean mouseDrag(Event evt, int x, int y) {
                offg.fillOval(x, y, 5, 5);
                repaint();
                return true;
        }
}


