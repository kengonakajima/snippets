//using System;
//using System.Runtime.CompilerServices;
using System.Threading;

public class Work
{
	public static void Do(){
		for(int i = 0; i < 10; i++){
			Thread.Sleep(1000);
			System.Console.Write( "child\n");
		}
	}
}
class MonoEmbed {
	[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
	extern static string gimme();
	[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]	
	extern static void sayhello(string s);
	static void Main() {
		string s = gimme();
		sayhello(s);
		Thread myThread = new Thread(new ThreadStart( Work.Do ));
		myThread.Start();
		System.Console.Write( "parent\n");
	}
}
