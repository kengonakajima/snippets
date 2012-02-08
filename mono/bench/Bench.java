import java.lang.*;

class V
{
	public int value;
	public boolean is_a_prime;
	V( int v, boolean p )
	{
		value = v;
		is_a_prime = p;
	}
}
public class Bench

{ 
	//init
	public static void main(String args[]) {
		int N = Integer.parseInt(args[0]);

		V[] vals = new V[N];
		int i;
		for(i=0;i<N;i++){
			vals[i] = new V( i, true );
		}
		
		// check
		for(i=2;i<N;i++){
			for(int j=2;j<N;j++){
				if( i == j ){
					continue;
				}
				if((vals[j].value % i) == 0 ){
					vals[j].is_a_prime = false;
				}
			}
		}
		// count
		int cnt = 0;
		for(i=2;i<N;i++){
			if( vals[i].is_a_prime ){
				cnt++;
				System.out.print( vals[i].value + " " );
			}
		}
		System.out.println( "done. cnt=" + Integer.toString(cnt) );
	}
}


