import java.awt.*;
import java.io.*;


class hoge
{
	static Frame f;

	public static void main(String args[] )
	{
		f = new Frame("sdfasdf");
		f.resize(209,200);
		f.show();
		f.hide();
		f = null;
		System.gc();
	}

}
