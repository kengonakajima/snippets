/*
   $B4J0W(BHTTP$B%I%-%e%a%s%H%5!<%P!<(B in Java by Kengo Nakajima 12.26.1996

   $B$"$i$f$k%^%7%s$G(BHTTP$B$N%5!<%P!<$rN)$A$"$2$k$3$H$,$G$-$^$9!#(B
   $BBP1~(BOS$B$O!"(BWindows 95,$B3F<o(BUNIX$B$G$9!#(BJDK1.1$BMQ$N%W%m%0%i%`$G$9!#(B
   
   $B%G%U%)%k%H$N%]!<%HHV9f$H$7$F!"(B80$B$r;H$$$^$9$,!"$3$N%]!<%H$r;H$&$3$H$O(B
   $B%9!<%Q!<%f!<%6$K$7$+5v$5$l$F$$$J$$$N$G!"B>$N%]!<%H$r;HMQ$7$F$/$@$5$$!#(B
   $B$=$N>l9g$O!"(BURL$B$N%[%9%HL>$N8e$m$K%]!<%HHV9f$r0J2<$N$h$&$K$7$FIU$1$?$7(B
   $B$^$9!#(B
   http://myhost:9000/hogehoge.html
   $B$J$*!"%&%$%s%I%&%:$G$O!"$I$N%]!<%H$r;H$&$N$bLdBj$"$j$^$;$s!#(B

> java httpd base_directory portnumber

   documentbase$B$K$O!"%U%!%$%k$r8!:w$9$k;~$K$I$N%G%#%l%/%H%j$+$i;O$a$k$+(B
   $B$r;XDj$7$^$9!#(B
   port$B$K$O!";HMQ$9$k%]!<%H$r;XDj$7$^$9!#(B
   
----   
$B%&%$%s%I%&%:$G$N5/F0Nc!'(B
> java httpd C:/home/ringo/public-html 9000
> java httpd C:/home/ringo/public-html


UNIX$B$G$N5/F0Nc!'(B
> java httpd /home/ringo/public-html 9000

$B%(%i!<=hM}$b2?$b$7$F$$$J$$$N$G!"$&$^$/F0$+$J$$;~$,$"$k$+$b$7$l$^$;$s!#(B
$B$=$7$F0BA4@-$bA4$/$"$j$^$;$s!#8x3+MQ$K$O;H$o$J$$$G$/$@$5$$!#(B
rmi$B$N%W%m%0%i%`$r$9$k;~$N<B83MQ$K;H$C$F$/$@$5$$!#(B
*/

import java.net.*;
import java.io.*;

class httpd
{
  public static void main(String args[] )
  {
    httpd_main httpm;
    if( args.length == 1 ){
      httpm = new httpd_main( args[0]  ,80 );
    } else {
      httpm = new httpd_main( args[0] , Integer.parseInt( args[1] ) );
    }
  }
}

class httpd_main
{
  ServerSocket ssock;
  int connection_number=0;
  
  httpd_main( String base , int port )
  {
    try{
      ssock = new ServerSocket( port );
      while(true){
	Socket sock;
	sock = ssock.accept();
	httpd_child httpc = new httpd_child( base , sock );
      }
      
    }catch( IOException e ){
      System.out.println( e );
    }
  }
}

class httpd_child implements Runnable
{
  Thread t;
  InputStream in;
  OutputStream out;
  Socket sock;
  String base;
  
  httpd_child( String base , Socket sock )
  {
    this.base = base;
    this.sock = sock;

    try{
      out = sock.getOutputStream() ;
      in = sock.getInputStream();
    }catch( IOException e ){
      System.out.println( e );
    }
    t = new Thread(this);
    t.start();
  }
  
  public void run()
  {
    try{
      byte b[] = new byte[1000];
      int pointer=0;
      while(true){
	int c;
	c = in.read();

	if( c != -1 ){
	  b[pointer++] = (byte)c;
	  try{
	  if( ( b[pointer-1] == 10 && b[pointer-2] == 13 &&
		b[pointer-3] == 10 && b[pointer-4] == 13 ) ||
	      ( b[pointer-1] == 10 && b[pointer-2] == 10 )
	      ){
	    break;
	  }
	  }catch( ArrayIndexOutOfBoundsException e ){  }
	} else {
	  break;
	}
	try{ Thread.sleep(10);} catch( InterruptedException e ){}
      }
      
      // this program understands only first line of input
      // understand message like this: GET filename HTTP/1.0 
      // deal with raw byte array.

      int first_space=0 , second_space=0;
      for(int i=0;i<1000;i++){
	if( b[i]==' ' ){
	  if( first_space == 0 ) first_space =i;
	  else{
	    second_space = i;
	    break;
	  }
	}
      }
      
      if( first_space == 0 || second_space == 0 ){
	String msg = "<html><body>bad request</body></html>\n";
	writeHeader(msg.length() );
	writeMsg( msg );
	closeSocket();
	return;
      }

      // read file and send
      String filename;
      String ftmp = new String( b , first_space + 1 ,
	second_space - first_space -1); // jdk1.1
      filename = base + ftmp;
      System.out.println( filename );
      FileInputStream fin =null;
      try{
	fin = new FileInputStream( filename );
      }catch( FileNotFoundException e){
	String msg = "<html><body>file not found: " + ftmp  + "</body></html>\n";
	writeHeader( msg.length() );
	writeMsg( msg );
	closeSocket();
	return;
      }
      
      int av;
      av = fin.available();
      byte fileb[] = new byte[av];
      fin.read( fileb , 0 , av );
      fin.close();
      out.write( fileb , 0 , av );
      try{
	Thread.sleep(1000 );
      }catch( InterruptedException e ){ }
      closeSocket();

    }catch( IOException e ){
      System.out.println( e );
    }
  }
  void closeSocket() throws IOException
  {
    in.close();
    out.close();
    sock.close();
    t.stop();
  }
  

  void writeMsg( String s ) throws IOException
  {
    byte b[] = new byte[ s.length() ];
    b = s.getBytes();  // jdk1.1
    out.write( b , 0 , s.length() );
    
  }
  
  void writeHeader(int content_length) throws IOException
  {
    String s = "HTTP/1.0 200 OK\n" + "Server: simplehttpd\n"
      + "Content-type: text/html\nContent-length: " + content_length + "\n\n";
    byte b[] = new byte[s.length()];

    // (jdk1.0.2) s.getBytes( 0 , s.length() , b , 0 );
    b = s.getBytes(); // jdk1.1
    out.write( b , 0 , s.length() );
    
  }
  
}
  
  
