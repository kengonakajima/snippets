#include <stdio.h>
#include <signal.h>
#include <assert.h>


FILE *fp;

void hoo ( int d )
{
	printf("hup:%d\n",d);

    if(fp){
        fprintf(fp,"hup.... reopen."
}
main(int argc, char *argv)
{
	char buf[10];
    fp = fopen( "logfile.txt", "w");
    assert(fp);

	signal( SIGHUP , hoo );
	
	fgets(buf , 10 , stdin );
	printf("%s\n" , buf );

    int cnt=0;
    while(1){
        cnt++;
        fprintf(fp,"cnt=%d\n", cnt);
        sleep(1);
    }
}
     
