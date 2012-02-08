import java.lang.*;

class hoge implements Cloneable
{
	hoge()
	{
	}
	protected Object clone()
	{
		try{
			return super.clone();
		}catch( Throwable t){
		}
		return null;
	}

}

class aho
{


	aho()
	{
		hoge h = new hoge();
		hoge h2 = (hoge) h.clone();
	}
}

class kasu
{
	public static void main(String args[] )
	{
		aho a = new aho();
	}
}
