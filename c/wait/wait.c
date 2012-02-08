#include <stdio.h>

#include <sys/types.h>

/*

  fork$B$7$?;R$,!"(Bsigsegv$B$H$+$GMn$A$F$b!"(Bwait$B$7$FI|3h$5$;$k!#(B
  dataserv$B$O!"JB9T%5!<%P!<$G$J$$$N$G!"$=$N$h$&$K$7$d$9$$!#(B

  $B$=$l$+$i!"(Bnpcc$B$O!"%5!<%P!<$,;`$s$G:F5/F0$5$l$?$i!"%-%c%i$N>pJs$,(B
  $B$@$$$V$sJQ2=$7$F$7$^$&2DG=@-$,$"$k$N$G!"(Bnpcc$B$b:F5/F0$7$?$[$&$,(B
  $B$h$$!#(Bserv$B$OJB9T%5!<%P!<$J$N$GLdBj$J$$$H$7$F!"(Bnpcc$B$H(Bdataserv$B$,(B
  $BI|3h%5!<%P!<$K$J$k$H$$$&$N$G$h$$!#(B
  
   
*/
main()
{
	int pid;


	for(;;){

		int pid;

		pid = fork();
		fprintf(stderr,"forked.\n");
		
		if( pid == 0 ){
			int ret;
			
			/* i am a parent */
			waitpid( pid , &ret , 0 );
			fprintf(stderr,"return value: %d\n" , ret );
		} else {
			char *p = 0;

			*p = 4;
		}

	}

}

	
