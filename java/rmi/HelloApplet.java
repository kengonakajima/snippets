
package examples.hello;

import java.awt.*;
import java.rmi.*;

public class HelloApplet extends java.applet.Applet 
{
	String message = "";
	public void init(){
		try{
			Hello obj = (Hello)Naming.lookup("//" + 
					 getCodeBase().getHost() + "/HelloServer");
			message = obj.sayHello();
		}catch( Exception e ){
			System.out.println("HelloApplet exception: " +
							   e.getMessage());
			e.printStackTrace();
		}
		System.out.println("sayHello returns:" + message );
	}
}
