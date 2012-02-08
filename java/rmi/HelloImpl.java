package examples.hello;

import java.rmi.*;
import java.rmi.server.UnicastRemoteObject;

// public�ʥ�⡼�ȥ��󥿡��ե�������implements���롣
public class HelloImpl extends UnicastRemoteObject implements Hello
{
	private String name;
	
	public HelloImpl( String s ) throws RemoteException
	{
		super();
		name = s;
	}
	int counter=0;
	public String sayHello() throws RemoteException
	{
		counter++;
		return "Hello World!" + counter;
	}
	public static void main(String args[] )
	{
		// �ᥤ��롼����Ǥϡ��ޤ��������ƥ��ޥ͡������new���ʤ���Фʤ�ʤ���
		System.setSecurityManager( new RMISecurityManager());

		try{
			HelloImpl obj = new HelloImpl( "HelloServer");
			//Naming.rebind("//ns2.titan.co.jp/HelloServer" , obj );
			Naming.rebind( "//" + args[0] + "/HelloServer" ,obj);
			System.out.println("HelloServer boud in registry");
		}catch( Exception e ){
			System.out.println("HelloImpl err: " + e.getMessage());
			e.printStackTrace();
		}
	}

}
