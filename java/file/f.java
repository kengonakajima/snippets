import java.applet.Applet;
import java.awt.*;
import java.awt.*;
import java.io.*;

/*
  IRIXでは、ローカルのものを読みこめば、ファイルに自由に書きこみできた。
  httpで読みこむと、だめだった。
  Windowsでもそうだった。

  Macintoshでは、ローカルでもネットワークでも、セキュリティエラーだった。
  これはバグだ。

  ちなみに、IOExceptionが出た場合は、セキュルティエラーは出ていない。

  UNIXでは、FileOutputStreamのファイル名指定のところで、絶対パスを
  指定すれば、問題なくアクセスできる。
  WIndowsでは、 /は、C:\ のことだ。UNIXと同じように/を使って、ディレクトリ
をたどっていける。Macでは、できなかったので、わからない。


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
