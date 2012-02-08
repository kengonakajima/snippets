#include <stdio.h>

#include <sys/types.h>

/*

  forkした子が、sigsegvとかで落ちても、waitして復活させる。
  dataservは、並行サーバーでないので、そのようにしやすい。

  それから、npccは、サーバーが死んで再起動されたら、キャラの情報が
  だいぶん変化してしまう可能性があるので、npccも再起動したほうが
  よい。servは並行サーバーなので問題ないとして、npccとdataservが
  復活サーバーになるというのでよい。
  
   
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

	
