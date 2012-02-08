class aho
{
	public static void main(String args[] ){
		try{
			throw new Exception("ringo made it");
		}catch( Throwable t ){
			t.printStackTrace();
		}
	}
}
