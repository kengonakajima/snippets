import java.applet.Applet;
import java.awt.*;
import java.awt.*;
import java.io.*;

/*
  IRIX$B$G$O!"%m!<%+%k$N$b$N$rFI$_$3$a$P!"%U%!%$%k$K<+M3$K=q$-$3$_$G$-$?!#(B
  http$B$GFI$_$3$`$H!"$@$a$@$C$?!#(B
  Windows$B$G$b$=$&$@$C$?!#(B

  Macintosh$B$G$O!"%m!<%+%k$G$b%M%C%H%o!<%/$G$b!"%;%-%e%j%F%#%(%i!<$@$C$?!#(B
  $B$3$l$O%P%0$@!#(B

  $B$A$J$_$K!"(BIOException$B$,=P$?>l9g$O!"%;%-%e%k%F%#%(%i!<$O=P$F$$$J$$!#(B

  UNIX$B$G$O!"(BFileOutputStream$B$N%U%!%$%kL>;XDj$N$H$3$m$G!"@dBP%Q%9$r(B
  $B;XDj$9$l$P!"LdBj$J$/%"%/%;%9$G$-$k!#(B
  WIndows$B$G$O!"(B /$B$O!"(BC:\ $B$N$3$H$@!#(BUNIX$B$HF1$8$h$&$K(B/$B$r;H$C$F!"%G%#%l%/%H%j(B
$B$r$?$I$C$F$$$1$k!#(BMac$B$G$O!"$G$-$J$+$C$?$N$G!"$o$+$i$J$$!#(B


*/

  
public class f extends Applet
{

	TextField t;
	Button b;
	Label l;

	public void init() 
	{
		l = new Label("File name:");
		t = new TextField(30);
		b = new Button("write");

		setLayout( new BorderLayout() );
		add( "North" , l );
		add( "Center" , t );
		add( "South" ,b );

	}
	public void paint( Graphics g ){
		System.out.println( t.getText());
		
	}
	public boolean action( Event e , Object o )
	{
		if( e.target == b ){
			try{ 
				FileOutputStream fo = new FileOutputStream( t.getText());

				String s = t.getText();
				byte buf[]= new byte[ s.length()];				
				s.getBytes( 0 , s.length() , buf ,0 );
				fo.write( buf );
				fo.write( 10 );
				fo.close();
			}catch( IOException ex ){
				System.out.println("Fucking IO.");
			}
			
		}
		return true;
	}

}
