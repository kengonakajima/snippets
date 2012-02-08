import java.net.*;
import java.io.*;

class acceptor implements Runnable
{
  ServerSocket servsock;
  server parent;
  Thread t;

  acceptor(ServerSocket ss, server parent)
    {
      servsock = ss;
      this.parent = parent;
      t = new Thread( this );
      t.start();
    }
  public void run()
    {
      System.out.println("wait for new connection.");
      while(true)
        {
          Socket tmpsock;
          try
            {
              tmpsock = servsock.accept();
            }
          catch(IOException e)
            {
              System.out.println("accept error.");
              tmpsock = null;
            }
          System.out.println("New socket. " + tmpsock);
          parent.addSocket( tmpsock );
        }
    }
}

class server implements Runnable
{
  public static void main( String args[] )
    {
      if( args.length == 0)
        {
          System.out.println("Usage: java server portno");
          System.exit(0);
        }
      server serv;
      serv = new server( Integer.parseInt(args[0]) );
    }

  int playerno = 5;
  Thread thread;
  ServerSocket servsock;
  Socket sock[] = new Socket[playerno];
  boolean using[] = new boolean[playerno];
  InputStream in[] = new InputStream[playerno];
  OutputStream out[] = new OutputStream[playerno];
  int timeout[] = new int[playerno];

  int timeout_max = 300;

  acceptor ac;

  int x[] = new int [playerno];
  int y[] = new int [playerno];

  server(int port)
    {
      try
        {
          ServerSocket servsock = new ServerSocket( port );
          ac = new acceptor(servsock , this );
        }
      catch(IOException e)
        {
          System.out.println("Can't make ServerSocket.");
        }

      for(int i = 0 ; i < playerno ; i++)
        {
          using[i] = false;
          x[i] = y[i] = 50;
          timeout[i] = 0;
        }

      thread = new Thread(this);
      thread.start();
    }

  public void addSocket(Socket soc)
    {
      if( soc == null ) return;
      for(int i = 0 ; i < playerno ; i++)
        {
        if(using[i] == false)
          {
            sock[i] = soc;
            try
              {
                in[i] = soc.getInputStream();
                out[i] = soc.getOutputStream();
              }
            catch(IOException e )
              {
                System.out.println("getting IN/OUT stream error.");
              return;
              }
            using[i] = true;
            System.out.println("player " + i + " is added.");
          return;
          }
        }
      System.out.println("player full.");
      try
        {
          soc.close();
        }
      catch(IOException e ){}
    }


  void deletePlayer(int index )
    {
      timeout[index] = 0;
      try{
        sock[index].close();
        in[index].close();
        out[index].close();
      }
      catch(IOException exc ){}
      using[index] = false;
      System.out.println("Closed socket. player="+index);
    }

  void playerMove(int index,int key)
    {
      int dx,dy;

      switch(key)
        {
        case 1:
          dx = -5;
          dy =  0;
          break;

        case 2:
          dx =  0;
          dy =  5;
          break;

        case 3:
          dx =  0;
          dy = -5;
          break;

        case 4:
          dx = 5;
          dy = 0;
          break;

        case 5:
          dx = dy = 0;
          deletePlayer( index );
          return;

        default:
          dx = dy = 0;
          break;
        }
      x[index] += dx;
      if(x[index] < 0 ) x[index] = 0;
      if(x[index] >200) x[index] = 200;

      y[index] += dy;
      if(y[index] < 0 ) y[index] = 0;
      if(y[index] > 200) y[index] = 200;

      timeout[index] = 0;
    }

  void sendForAll()
    {

      short sendbuf[] = new short[(playerno+1)*2*2];
      int counter = 0;
      for(int i = 0; i < playerno ; i++){
        if(using[i] == true){
          sendbuf[counter++] = (short)x[i];
          sendbuf[counter++] = (short)y[i];
        }
      }
      sendbuf[counter++] = -1;
      sendbuf[counter++] = -1;

      for(int i = 0; i < playerno ; i++){
        if(using[i] == true)
          {
            try{
              DataOutputStream dout;
              dout = new DataOutputStream( out[i] );
              for(int j = 0; j < counter ; j++){
                dout.writeShort( sendbuf[j] );
              }
            }catch(IOException e){
              System.out.println("dout ex.");
              deletePlayer( i );
            }
          }
      }
    }
  public void run()
    {
      while(true)
        {
          try{
            Thread.sleep(200);
          }
          catch(InterruptedException e){}

          for(int i = 0; i < playerno; i++){
            if(using[i])System.out.print("ON");
            else System.out.print("OFF");

            System.out.print(":"+x[i]);
            System.out.print(":"+y[i]);
            System.out.print(":"+timeout[i]);
            System.out.print(" ");
          }
          System.out.println("");

          for(int i = 0; i < playerno ; i++){
            if(using[i] == true ){
              if( (++timeout[i]) > timeout_max){
				  System.out.println( "AHOAHO\n" );
                deletePlayer( i );
                continue;
              }
              try{
				  System.out.println( "AAA\n" );
               if( in[i].available() > 0 ){

                  int r;
				  System.out.println( "BBB\n" );
                  r = in[i].read();
				  System.out.println( "CCC\n" );
                  playerMove( i, r);
				  System.out.println( "DDD\n" );				  
                  sendForAll();
               }
              }
              catch( IOException e)
                {
					System.out.println( "adflaksdfas\n" );
					System.out.println( e );
                  deletePlayer( i );
                }
            }
          }
        }
    }
}

