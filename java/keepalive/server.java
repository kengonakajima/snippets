// KeepAlive HTTP connection test


class server
{
	serverMain sm ;
	
	public static void main(String args[])
	{
		sm = new serverMain();
		

	}

}


class serverMain
{

	ServerSocket ss
	Socket newsock;

	OutputStream out;
	InputStream in;

	serverMain()
	{
		try{
			ss = new ServerSocket( 7777 );
			
			newsock = ss.accept();
			out = ss.getOutputStream();
			in = ss.getInputStream();

		} catch( Exception e ){
			System.out.println( e );
			e.printStackTrace();
		}
	}
}



		
