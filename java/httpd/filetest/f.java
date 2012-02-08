import java.io.*;

class f
{
  public static void main(String args[] )
  {
    try{
    FileInputStream fin;
    fin = new FileInputStream( args[0] );
    byte b[] = new byte[100];
    fin.read( b , 0 , 100 );
    fin.close();
    for(int i=0;i <100 ;i++){
      System.out.print( b[i] );
    }
    System.out.println("asldkfjas");
    
    }catch( IOException e ){
      System.out.println( e );
    }
  }
  
}
