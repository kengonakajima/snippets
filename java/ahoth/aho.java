import java.util.*;

class aho
{
  public static void main(String args[] )
  {
	mm m = new mm();
    int i = 0;
	while(true){
	  i++;
      System.out.println("i" + i);
	  try{ Thread.sleep(1); }catch( Throwable t ){}

	  m.mmmm(false);
	  
	}

  }

}

class mm implements Runnable
{
   Thread t;
  mm()
  {
	t=new Thread(this);
	t.start();
  }
  int j=0;
  public void run()
  {

	while(true){
	  j++; 
	  mmmm(true);
	  try{ Thread.sleep(new Random().nextInt()&0xfff); }catch( Throwable t ){}
      
	}

  }
  synchronized void mmmm(boolean b)
  {

	if(b){backup; System.out.println("J"+j);return;}
    System.out.println("ahostart");

	for(int i = 0;i<3000000;i++);// use backup

    System.out.println("ahoend");
	
  }


}
