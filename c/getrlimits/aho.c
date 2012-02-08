
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>

struct rlimit t;

int
main()
{
    int r;
    r = getrlimit( RLIMIT_NOFILE, &t );
    fprintf( stderr, "getrlimit:%d %d %d\n", r, t.rlim_cur, t.rlim_max );

	t.rlim_max = 2048;
	r = setrlimit( RLIMIT_NOFILE, &t );
	

	fprintf( stderr, "setrlimit:%d %d %d\n", r, t.rlim_cur, t.rlim_max );

	r = getrlimit( RLIMIT_NOFILE, &t );

	fprintf( stderr, "getrlimit:%d %d %d\n", r, t.rlim_cur, t.rlim_max );
	
}
