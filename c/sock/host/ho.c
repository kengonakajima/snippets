
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

extern int errno;
extern int h_errno;

int main(int argc,char **argv)
{
	char *ptr;
	char *host_err_str();
	struct hostent *hostptr;

	while( --argc >0){
		ptr = *++argv;
		if( (hostptr = gethostbyname(ptr)) == NULL){
			err_ret("gethostbyname error for host: %s %d",ptr,h_errno);
			continue;
		}
		printf("official host name: %s\n",hostptr->h_name);

		/* do alias */

		while( ( ptr = *(hostptr->h_aliases)) != NULL){
			printf("        alias: %s\n",ptr);
			hostptr->h_aliases++;
		}
		printf("       addr type = %d,  addr length = %d\n",hostptr->h_addrtype,hostptr->h_length);

		switch(hostptr->h_addrtype){
			case AF_INET:
				pr_inet(hostptr->h_addr_list,hostptr->h_length);
				break;
			default:
				err_ret("unknown address type.\n");
				break;
		}
	}
}
void pr_inet(char **listptr,int length)
{
	struct in_addr *ptr;

	while( ( ptr = (struct in_addr *) *listptr++) != NULL){
		printf("    In integer: %d\n",inet_addr(inet_ntoa(*ptr)) );
		printf("    Internet address: %s\n",inet_ntoa(*ptr));
		
	}
}
void err_ret(char * a)
{
	fprintf(stderr,"%s",a);
	exit(0);
}
