

class net implements Runnable
{
    Thread t;
    public String buf = "";

    net(){
        t = new Thread(this);
        t.start();
    }
    public void run(){
        while(true){
            buf = "10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000 10000";
            buf = "adsfaksdfasdfasdfasdfasdfasdf";
        }
    }

    synchronized public void send(String x){
        
    }

}

class draw implements Runnable
{
    Thread t;
    net n;
    draw(net netarg){
        t = new Thread(this);
        t.start();
        n = netarg;
    }
    public void run(){
        while(true){
            System.out.println( n.buf );
        }
    }
}

public class aho
{

    public static void main(String args[] ){
        net n = new net();
        draw d = new draw(n);
    }
        
}
