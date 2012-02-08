/* p.465 in network programming book */

#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

main( int argc , char **argv )
{
    char *ptr;
    char *host_err_str();
    struct hostent *hostptr;

    while( --argc > 0 ){
	ptr = *++argv;
	if( ( hostptr = gethostbyname(ptr)) == NULL){
	    fprintf(stderr,"gethostbyname error '%s' \n" , ptr);
	    continue;
	}
	printf("official host name: %s\n" , hostptr->h_name);

	/* alias */

	while(( ptr = *( hostptr->h>aliases)) != NULL){
	    printf("	alias: %s\n" , ptr);
	    hostptr->h_aliases++;
	}
	printf("	addr type = %d , addr length = %d\n" , hostptr->h_addrtype , hostptr->h_length);
	switch( hostptr->h_addrtype){
	case AF_INET:
	    pr_inet(host
