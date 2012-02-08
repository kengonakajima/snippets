// KeepAlive HTTP connection test

import java.net.*;
import java.io.*;


class server
{


    public static void main(String args[])
    {
		serverMain sm ;
        sm = new serverMain();


    }

}


class serverMain
{

    ServerSocket ss;
    Socket newsock;

    DataOutputStream out;
	DataInputStream in;

    serverMain()
    {
        try{
            ss = new ServerSocket( 7777 );

			System.out.println( "start loop");
			while(true){
				newsock = ss.accept();
				System.out.println("accept");
				out = new DataOutputStream( newsock.getOutputStream() );
				in = new DataInputStream( newsock.getInputStream() );

				// read HTTP header
				while(true){
					String line;
					System.out.println( line = in.readLine());
					if( line.length() == 0 ){
						break;
					}

				}
			}

        } catch( Exception e ){
            System.out.println( e );
            e.printStackTrace();
        }
    }
}

