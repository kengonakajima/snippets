public class aho
{
    static void b(int b){
        int j ;
        for(j=0;j<20;j++){
            int k;
            k = b;
            k += j;
        }
    }
    public static void main( String args[] ){

        int i;
        for(i=0;i<300000;i++){
            b(100);
        }
    }
}
