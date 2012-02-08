using System;


class V
{
	public int value;
	public bool is_a_prime;
	public V( int v, bool p )
	{
		value = v;
		is_a_prime = p;
	}
}

public class Bench
{  

  public static void Main( String[] args)
  {
		int N = int.Parse(args[0]);
		//init
		V[] vals = new V[N];
		int i;
		for(i=0;i<N;i++){
			vals[i] = new V( i, true  );
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
				Console.Write( vals[i].value + " " );
			}
		}
		Console.WriteLine( "done. cnt=" + cnt );
  }     
} 




/*

	}
}


*/