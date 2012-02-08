/*
   簡易HTTPドキュメントサーバー in Java by Kengo Nakajima 12.26.1996

   あらゆるマシンでHTTPのサーバーを立ちあげることができます。
   対応OSは、Windows 95,各種UNIXです。JDK1.1用のプログラムです。
   
   デフォルトのポート番号として、80を使いますが、このポートを使うことは
   スーパーユーザにしか許されていないので、他のポートを使用してください。
   その場合は、URLのホスト名の後ろにポート番号を以下のようにして付けたし
   ます。
   http://myhost:9000/hogehoge.html
   なお、ウインドウズでは、どのポートを使うのも問題ありません。

> java httpd base_directory portnumber

   documentbaseには、ファイルを検索する時にどのディレクトリから始めるか
   を指定します。
   portには、使用するポートを指定します。
   
----   
ウインドウズでの起動例：
> java httpd C:/home/ringo/public-html 9000
> java httpd C:/home/ringo/public-html


UNIXでの起動例：
> java httpd /home/ringo/public-html 9000

エラー処理も何もしていないので、うまく動かない時があるかもしれません。
そして安全性も全くありません。公開用には使わないでください。
rmiのプログラムをする時の実験用に使ってください。
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
  
  
