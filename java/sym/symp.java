import java.applet.Applet;
import java.awt.*;

public class symp extends Applet implements Runnable
{
	public void init()
	{
		int i=0;
		
		for(i = 0 ; i < 64 ; i++ ){
			int bits[] = new int[6];
			bits[0] = i & 1;
			bits[1] = i & 2;
			bits[2] = i & 4;
			bits[3] = i & 8;
			bits[4] = i & 16;
			bits[5] = i & 32;
			calc( bits );
		}
	}
	void calc( int bits[] )
	{
		if( bits.length != 6 )return;
		System.out.println( ""+bits[0]+bits[1]+bits[2]+bits[3]+bits[4]+bits[5]);
	}


	public void run()
	{
		while( true ){
			try{
				Thread.sleep(100);
			}catch( InterruptedException e  )
			{
			}

		}

	}

}
