


void qsort(int *v , int left , int right )
{
	int i , last;

#define SWAP( v, a , b )   { int tmp; tmp=(v[a]);v[a]=(v[b]);v[b]=tmp; }
	if( left >= right ) return;

	SWAP( v , left , (left + right)/2 );

	last = left;

	for(i=left + 1; i<= right ; i++ ){
		if( v[i] < v[left] ){
			last++;
			SWAP( v , last , i );
		}
	}
	SWAP( v , left , last );
	qsort( v, left, last-1 );
	qsort( v, last+1 , right );
	
}




main()
{
	int fuck[10]={1,5,676,32,5,
				  10,345,4444,2,0 };
	int i;
	
	qsort( fuck ,  0, 9  );
	

	for(i=0;i<10;i++){
		printf( "%d\n" , fuck[i] );
	}
	
}

