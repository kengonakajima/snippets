import java.awt.*;
class shshssoshos
{
	public static void main( String args [] )
	{
		hoge h = new hoge();
	}


}


class hoge
{

	hoge()
	{
		
		sm( 1 , (Object) new Integer(10), ( Object)new Integer( 20), (Object) new MenuItem("ahoaha") );
		sm( 2 , (Object) new Button("ahdsfla") , null , null);
	}

	void sm( int type ,Object arg1 , Object arg2 , Object arg3 )
	{
		switch ( type )
		{
			case 1:
			m1( ((Integer)arg1).getValue() , ((Integer) arg2).getValue() ,((Integer)arg3).getValue() );
			break;
			case 2:
			m2( (Button)arg1 );
			break;

		}

	}
	void m1( int a , int b , MenuItem m )
	{

	}
	void m2( Button b[] )
	{
	}

}
