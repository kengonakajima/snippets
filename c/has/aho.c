#include <stdio.h>

int hashCode( char *buf )
{


	    int i;

		    int blen;
		    int v=0;

		    blen = strlen(buf );

		    for(i = 0 ; i < blen ; i++){
				        int x;

						        x = buf[i]&0xf;
						        v += (x << ( i%32));
					}

    return v;

}

int a[65536];

main()
{
	int i;
	char buf[23412];

	
	while( fgets( buf , sizeof( buf) , stdin)){
		int i;
		printf("%d\n" , i = hashCode( buf ));
		a[i%(sizeof(a)/sizeof(int))] ++;
	}

	for(i=0;i< 65536;i++){
		fprintf(stderr,"%d " , a[i] );
	}
}
