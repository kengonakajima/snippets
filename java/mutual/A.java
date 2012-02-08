public class Network implements Runnable
{
    Character cha[];
    
    Network(Character cha_arg[]){
        cha = cha_arg;
        Thread t = new Thread(this);
        t.start();
    }

    void hoge(){
        cha[1].set_position(1,2);
    }
    void hoge2(Character ch){
        ch.set_position(3,4);
    }
    public void run(){
        while(true){
            try {
                hoge();
            } catch( Exception e ){
                
            }
        }
    }
}

public class Character implements Runnable
{
    public int x,y,hp;
    
    Character(int x, int y, int hp ){
        this.x = x;
        this.y = y;
        this.hp = hp;
        Thread t = new Thread(this);
        t.start();
    }

    void set_position( int x, int y ){
        this.x = x;
        this.y = y;
        System.out.println( "position is set " + x + " " + y );
    }
    public void run(){
        int i;
        while(true){
            for(i=0;i<10;i++){
                System.out.println( "Draw" + x + " " + y );
            }
        }
    }
    
}

public class A
{

    public static void main( String args[] ){
        Network net;
        Character cha[] = new Character[10];        

        int i;
        for(i=0;i<10;i++){
            cha[i] = new Character(0,0,10);
        }
        
        net = new Network(cha);
        net.hoge();
        net.hoge2( cha[1] );
    }
}
